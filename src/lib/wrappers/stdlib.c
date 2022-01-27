#include <lib/stdlib.h>
#include <lib/nos.h>

void* __gxx_personality_v0 = 0;
void* _Unwind_Resume       = 0;

void* malloc(size_t size) { return __malloc(size); }

void* malloc_c(size_t size) { return __malloc_c(size); }

void* malloc_a(size_t size, uint8_t type) { return __malloc_a(size, type); }

void free(void* ptr) { return __free(ptr); }

void freearray(void** ptr, uint32_t count) { return __freearray(ptr, count); }

int atoi(const char* str) { return __atoi(str); }

uint32_t atol(const char* str) { return __atol(str); }

uint32_t atox(const char* str) { return __atox(str); }

float atof(const char* str) { return __atof(str); }

int rand() { return __rand(); }

void yield() { return __yield(); }

void exit(int code) { return __exit(code); }

char* itoa(int value, char* buffer, int base) { return __itoa(value, buffer, base); }

char* ltoa(size_t num, char* str, int base) { return __ltoa(num, str, base); }

char* strhex(uint32_t value, char* result, bool prefix, uint8_t bytes) { return __strhex(value, result, prefix, bytes); }

int system(char* command) { return __system(command); }