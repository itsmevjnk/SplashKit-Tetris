#ifndef UTILS_H
#define UTILS_H

#include <bits/stdc++.h>

using namespace std;

/**
 * @brief Macro function to find the minimum of two values.
 * 
 */
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))

/**
 * @brief Macro function to find the maximum of two values.
 * 
 */
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

/**
 * @brief Convert an integer to a string, optionally with padding.
 * 
 * @param num The number to be converted.
 * @param padding The number of characters to pad the number to (optional, defaults to 0 for no padding).
 * @param pad_char The padding character (optional, defaults to zero padding).
 * @return string The decimal string representation of num.
 */
string int_to_string(int num, int padding = 0, char pad_char = '0');

#endif