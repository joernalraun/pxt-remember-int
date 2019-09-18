#ifndef LOGGING_H
#define LOGGING_H

void log(const char* txt);
void logLn(const char* txt);
void logLn();
void logNum(int num);
void logNum(const char *name, int num);
void logHex(uint32_t addr);
void logHex(const char *name, uint32_t addr);
void logPtr(void *ptr);
void logPtr(const char *name, const void *ptr);
void logDumpHex(const void *ptr, int numBytes);
void throwError(const char* errMsg);

#endif // LOGGING_H
