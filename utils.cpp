#include "utils.h"

using namespace std;

/* int to string with padding */
string int_to_string(int num, int padding, char pad_char) {
    string result = to_string(num);
    if(padding > 0) result = string(padding - result.length(), pad_char) + result;
    return result;
}