#pragma once
#include <lib/stdint.h>

/// @brief Check if character is alphabetic or numeric @param c Character value @return Result
int isalnum(int c);

/// @brief Check if character is alphabetic @param c Character value @return Result
int isalpha(int c);

/// @brief Check if character is a control character @param c Character value @return Result
int iscntrl(int c);

/// @brief Check if character is numeric @param c Character value @return Result
int isdigit(int c);

/// @brief Check if character is graphic @param c Character value @return Result
int isgraph(int c);

/// @brief Check if character is lower-case alphabetic @param c Character value @return Result
int islower(int c);

/// @brief Check if character is valid for printing @param c Character value @return Result
int isprint(int c);

/// @brief Check if character is punctuation @param c Character value @return Result
int ispunct(int c);

/// @brief Check if character is white-space @param c Character value @return Result
int isspace(int c);

/// @brief Check if character is upper-case alphabetic @param c Character value @return Result
int isupper(int c);

/// @brief Check if character is hexadecimal @param c Character value @return Result
int isxdigit(int c);

/// @brief Convert character to lower-case equivalent @param c Character value @return Lower-case character
int tolower(int c);

/// @brief Convert character to upper-case equivalent @param c Character value @return Upper-case character
int toupper(int c);