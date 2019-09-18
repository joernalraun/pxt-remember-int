#include "pxt.h"
#include "MicroBit.h"

#include "logging.h"

#include<stdarg.h>

// extern MicroBit uBit;

void log(const char *msg) {
	uBit.serial.send(msg);
}

void logLn() {
	log("\r\n");
}

void logLn(const char *msg) {
	log(msg);
	logLn();
}


void logNum(int num) {
	char buf[20];
	sprintf(buf, "%d", num);
	uBit.serial.send(buf);
}

void logNum(const char *name, int num) {
	char buf[200];
	sprintf(buf, "%s: %d\r\n", name, num);
	uBit.serial.send(buf);
}

void logHex2(int n) {
	char buf[20];
	sprintf(buf, "%02x", (n & 0xFF));
	uBit.serial.send(buf);
}


void logHex(uint32_t addr) {
	char buf[20];
	sprintf(buf, "0x%lx", addr);
	uBit.serial.send(buf);
}

void logHex(const char *name, uint32_t addr) {
	char buf[200];
	sprintf(buf, "%s: 0x%lx\r\n", name, addr);
	uBit.serial.send(buf);
}

void logPtr(const void *ptr) {
	logHex((uint32_t)ptr);
}

void logPtr(const char *name, const void *ptr) {
	logHex(name, (uint32_t)ptr);
}

void logDumpHex(const void *ptr, int numBytes) {
	logPtr("dump", ptr);
	int cnt = 0;
	for (int i=0; i<numBytes; i++) {
		if (cnt >= 16) {
			cnt = 0;
			logLn();
		}
		cnt += 1;
		logHex2(((const char*)ptr)[i]);
		log(" ");
	}
	logLn();
}

void throwError(const char *errMsg) {
	while (true) {
		uBit.display.scroll(errMsg);
        uBit.sleep(1000);
        if ( uBit.buttonA.isPressed() == true ) {
        	uBit.sleep(24*60*60*1000);
        }
	}
}
