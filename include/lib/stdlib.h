#pragma once
#include <lib/stdint.h>
#include <lib/stddef.h>

/// @brief Request allocated memory region of specified size @param size Size of allocation @return Pointer to allocated memory
void* malloc(size_t size);

/// @brief Request allocated memory region of specified size and fill with zeros @param size Size of allocation @return Pointer to allocated memory
void* malloc_c(size_t size);

void* malloc_a(size_t size, uint8_t type);

/// @brief Request allocated memory region on kernel heap of specified size, type, and fill with zeros @param size Size of allocation @param type Type of allocation @return Pointer to allocated memory
void* kmalloc(size_t size, uint8_t type);

/// @brief Free existing allocated memory region @param ptr Pointer to allocated memory
void free(void* ptr);

/// @brief Free exisiting allocated memory region from heap stack @param ptr Pointer to allocated memory
void kfree(void* ptr);

/// @brief Free array of allocated pointers @param ptr Pointer to array @param count Amount of array entries
void freearray(void** ptr, uint32_t count);

/// @brief Convert decimal string to integer value @param str Pointer to string @return Parsed integer value
int atoi(const char* str);

/// @brief Convert decimal string to unsigned integer value @param str Pointer to string @return Parsed integer value
uint32_t atol(const char* str);

/// @brief Convert hexadecimal string to unsigned integer value @param str Pointer to string @return Parsed hexadecimal integer value
uint32_t atox(const char* str);

/// @brief Convert decimal string to floating-point value @param str Pointer to string @return Parsed floating-point value
float atof(const char* str);

/// @brief Random number generator @return Random value between 0-255
int rand();

/// @brief Yield for other threads and processes
void yield();

/// @brief Exit current process
void exit(int code);

/// @brief Convert integer value to string with specified base @param num Integer value @param str Output buffer @param base Integer base @return Output buffer
char* itoa(int num, char* str, int base);

/// @brief Convert unsigned value to string with specified base @param num Unsigned integer value @param str Output buffer @param base Unsigned integer base @return Output buffer
char* ltoa(size_t num, char* str, int base);

/// @brief Convert value to hexadecimal with specified formatting @param value Integer value @param result Output buffer @param prefix Append '0x' to beginning @param bytes Size to print @return Output buffer
char* strhex(uint32_t value, char* result, bool prefix, uint8_t bytes);

/// @brief Execute a system command @param command Command input buffer @return Result of command execution
int system(char* command);