#include "pxt.h"
#include "MicroBit.h"

#include "flashmem.h"

using namespace pxt;

namespace flash {


//% blockId=flash_writeflashnum
//% block="Write Flash Number|page %page|value %value"
//% shim=flash::writeflashnum
void writeflashnum(int page, int value) {
    initFlashMem();
    writePage(page, (char*)(void*)(&value), 0, 4);
}


//% blockId=flash_readflashnum
//% block="Read Flash Number|page %page"
//% shim=flash::readflashnum
int readflashnum(int page) {
    initFlashMem();
    int result = 12345;
    readPage((uint32_t) page, (char*)(void*)(&result), (uint32_t) 0, (uint32_t) 4);
    return result;
}

//% blockId=flash_getnumpages
//% block="Get number of pages"
//% shim=flash::getnumpages
int getnumpages() {
    initFlashMem();
    int result = (int) getNumPages();
    result = result-1;   // because page 0 is used as marker block.
    return result;
}


}
