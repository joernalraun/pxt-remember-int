#ifndef FLASHMEM_H
#define FLASHMEM_H

void initFlashMem();
uint32_t getPageSize();
uint32_t getNumPages();

const void *getPtr(uint32_t page, uint32_t offset);

void readPage(uint32_t page, char *buf);
void readPage(uint32_t page, char *buf, uint32_t start, uint32_t cnt);

void writePage(uint32_t page, const char *buf);
void writePage(uint32_t page, const char *buf, uint32_t start, uint32_t cnt);

void erasePage(uint32_t page);
void writePartial(uint32_t page, uint32_t pageOffset, const char *buf, uint32_t bufOffset, uint32_t length);

#endif // FLASHMEM_H
