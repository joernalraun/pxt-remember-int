#include "pxt.h"
#include "MicroBit.h"

// https://github.com/calliope-mini/calliope-mini-micropython/blob/master/source/microbit/persistent.c

// https://devzone.nordicsemi.com/f/nordic-q-a/977/able-to-write-only-one-time-in-nvmc
// https://infocenter.nordicsemi.com/index.jsp?topic=%252Fcom.nordic.infocenter.sdk52.v0.9.0%252Fbledfu_memory.html
// https://docs.platformio.org/en/latest/platforms/nordicnrf51.html
// https://docs.platformio.org/en/latest/boards/nordicnrf51/calliope_mini.html#board-nordicnrf51-calliope-mini

#include "flashmem.h"
#include "logging.h"

#include<stdarg.h>

#include "ticker.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "nrf51.h"
#include "nrf_nvmc.h"

#ifdef __cplusplus
}
#endif

#define PXTFLASH_MAGIC 0x4853414c46545850UL



// extern MicroBit uBit;

extern uint32_t __data_end__;
extern uint32_t __data_start__;
extern uint32_t __etext;

static uint32_t  fm_first_page = 0;
static uint32_t  fm_last_page = 0;
static uint32_t  fm_page_size = 0;

static inline uint32_t persistent_page_size(void) {
    return NRF_FICR->CODEPAGESIZE;
}

static inline char *microbit_end_of_rom() {
    return (char *)0x40000;
}

/** The end of the code area in flash ROM (text plus read-only copy of data area) */
static inline char *microbit_end_of_code() {
    return (char *)(&__etext + (&__data_end__ - &__data_start__));
}

static inline char *microbit_mp_appended_script() {
    return (char *)0x3e000;
}

static inline void *microbit_compass_calibration_page(void) {
    if (microbit_mp_appended_script()[0] == 'M') {
        return microbit_mp_appended_script() - persistent_page_size();
    } else {
        return microbit_end_of_rom() - persistent_page_size();
    }
}


const void *getPtr(uint32_t page, uint32_t offset) {
	return (const char *)((fm_first_page+page)*fm_page_size+offset);
}

uint8_t getByte(uint32_t page, uint32_t offset) {
	return *(const uint8_t *)((fm_first_page+page)*fm_page_size+offset);
}

uint16_t getWord(uint32_t page, uint32_t offset) {
	return *(const uint16_t *)((fm_first_page+page)*fm_page_size+offset);
}

uint32_t getDWord(uint32_t page, uint32_t offset) {
	return *(const uint32_t *)((fm_first_page+page)*fm_page_size+offset);
}

uint64_t getQWord(uint32_t page, uint32_t offset) {
	return *(const uint64_t *)((fm_first_page+page)*fm_page_size+offset);
}

void writeDWord(uint32_t page, uint32_t offset, uint32_t dword) {
	writePartial(page, offset, (const char *)&dword, 0, 4);
}

void writeQWord(uint32_t page, uint32_t offset, uint64_t qword) {
	writePartial(page, offset, (const char *)&qword, 0, 8);
}

void writeString(uint32_t page, uint32_t offset, const char *buf, int len) {
	writePartial(page, offset, buf, 0, len);
}


bool isStartPage(uint32_t page) {
	return getQWord(page, 0) == PXTFLASH_MAGIC;
}

void logPage(uint32_t page) {
    logNum("dump page:", page);
	logDumpHex((const void*)((fm_first_page+page)*fm_page_size), 64);
}

void initRootPage() {
//	logPage(0);
	writeQWord(0, 0, PXTFLASH_MAGIC);
	writeDWord(0, 8, fm_last_page);
//	logPage(0);
}

bool isEmptyPage(uint32_t page) {
	for (uint32_t i=0; i<fm_page_size; i+=4) {
		if (getDWord(page, i) != 0xFFFFFFFFu) {
			return false;
		}
	}
	return true;
}

void initFlashMem() {
	if (fm_page_size != 0) {
		return;
	}
    /* First determine where to end */
	fm_page_size = persistent_page_size();
    char *start = microbit_end_of_code() + fm_page_size;
    char *end = (char*)microbit_compass_calibration_page() - fm_page_size;
    fm_first_page = (((uint32_t)start)+fm_page_size) / fm_page_size;
    fm_last_page = ((uint32_t)end) / fm_page_size;

    logPtr("start", start);
    logPtr("end", end);
    logHex("pagesize", fm_page_size);

    char buf[400];
	sprintf(buf, "GUESS: page_size: %d, first_page: %d, last_page: %d, size: %d\r\n", fm_page_size, fm_first_page, fm_last_page, (fm_last_page-fm_first_page+1)*fm_page_size);
	log(buf);

	uint32_t numPages = getNumPages();
	for (uint32_t pg=0; pg<numPages; pg++) {
		if (isStartPage(pg)) {
			fm_first_page = fm_first_page + pg;
			fm_last_page = getDWord(0, 8);
			break;
		}
		if (isEmptyPage(pg)) {
			fm_first_page = fm_first_page + pg;
			numPages = getNumPages();
			for (int j=1; j<numPages; j++) {
				if (!isEmptyPage(j)) {
					fm_last_page = fm_first_page+j-1;
					break;
				}
			}
			initRootPage();
			break;
		}
	}
	numPages = getNumPages();
	sprintf(buf, "FREE: page_size: %d, first_page: %d, last_page: %d, size: %d\r\n", fm_page_size, fm_first_page, fm_last_page, (fm_last_page-fm_first_page)*fm_page_size);
	log(buf);
//	for (uint32_t i=0; i<numPages; i++) {
//		logDumpHex((const void*)((fm_first_page+i)*fm_page_size), 64);
//	}
//	log("previous:");
//	logDumpHex((const void*)((fm_first_page-1)*fm_page_size), 64);
//	log("next:");
//	logDumpHex((const void*)((fm_last_page+1)*fm_page_size), 64);

}


uint32_t getPageSize() {
	return fm_page_size;
}

uint32_t getNumPages() {
	return fm_last_page-fm_first_page+1;
}

void readPage(uint32_t page, char *buf) {
	logLn("readPage");
	if (fm_first_page+page>fm_last_page) {
		logNum("readPage: invalid page", page);
		throwError("readPage: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size;
    logHex("readPage: addr:", addr);
    logPtr("           buf :", buf);
    logNum("           len :", fm_page_size);
	memcpy(buf, (const char *)addr, fm_page_size);
}

void readPage(uint32_t page, char *buf, uint32_t start, uint32_t cnt) {
	if (fm_first_page+page>fm_last_page) {
		logNum("readPage: invalid page", page);
		throwError("readPage: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size + start;
    logHex("readPage: addr:", addr);
    logPtr("           buf :", buf);
    logNum("           cnt :", cnt);
	memcpy(buf, (const char *)addr, cnt);
}

void writePage(uint32_t page, const char *buf) {
	if ((page == 0) || (fm_first_page+page>fm_last_page)) {
		logNum("writePage: invalid page", page);
		throwError("writePage: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size;
	uint32_t len = fm_page_size >> 2;


    int8_t *data = (int8_t *)buf;
    const uint32_t *convSrc = (const uint32_t *)data;

    logHex("writePage: addr:", addr);
    logPtr("           buf :", buf);
    logPtr("           conv:", convSrc);
    logNum("           len :", len);



	// Writing to flash will stop the CPU, so we stop the ticker to minimise odd behaviour.
	ticker_stop();
    nrf_nvmc_page_erase(addr);
	nrf_nvmc_write_words(addr, convSrc, len);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
	ticker_start();
}

/**
 * use only word alligned calls.
 */
void writePage(uint32_t page, const char *buf, uint32_t start, uint32_t cnt) {
	if ((page == 0) || (fm_first_page+page>fm_last_page)) {
		logNum("writePage: invalid page", page);
		throwError("writePage: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size;
	uint32_t len = (cnt+3) >> 2;

    int8_t *data = (int8_t *)buf;
    const uint32_t *convSrc = (const uint32_t *)&data[start];

    logHex("writePage: addr:", addr);
    logPtr("           buf :", buf);
    logPtr("           conv:", convSrc);
    logNum("           len :", len);

	// Writing to flash will stop the CPU, so we stop the ticker to minimise odd behaviour.
	ticker_stop();
    nrf_nvmc_page_erase(addr);
//	while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
//	ticker_start();
	// Aligned word writes are over 4 times as fast per byte, so use those if we can.
//	ticker_stop();
	nrf_nvmc_write_words(addr, convSrc, len);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
	ticker_start();
}

void erasePage(uint32_t page) {
	if (fm_first_page+page>fm_last_page) {
		logNum("erasePage: invalid page", page);
		throwError("erasePage: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size;

    logHex("erasePage: addr:", addr);

	// Writing to flash will stop the CPU, so we stop the ticker to minimise odd behaviour.
	ticker_stop();
    nrf_nvmc_page_erase(addr);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
	ticker_start();
}

/**
 * use only word alligned calls.
 */
void writePartial(uint32_t page, uint32_t pageOffset, const char *buf, uint32_t bufOffset, uint32_t length) {
	if (fm_first_page+page>fm_last_page) {
		logNum("writePartial: invalid page", page);
		throwError("writePartial: invalid page");
		return;
	}
	uint32_t addr = (page + fm_first_page)*fm_page_size+pageOffset;
	uint32_t len = (length+3) >> 2;

    int8_t *data = (int8_t *)buf;
    const uint32_t *convSrc = (const uint32_t *)&data[bufOffset];

    logHex("writePart: addr:", addr);
    logPtr("           buf :", buf);
    logPtr("           conv:", convSrc);
    logNum("           len :", len);

	// Writing to flash will stop the CPU, so we stop the ticker to minimise odd behaviour.
	ticker_stop();
	nrf_nvmc_write_words(addr, convSrc, len);
	while (NRF_NVMC->READY == NVMC_READY_READY_Busy);
	ticker_start();
}



