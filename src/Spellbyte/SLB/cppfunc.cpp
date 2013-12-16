#include "cppfunc.h"
#include <iostream>

void doMyPrint(const char *text, size_t text_size)
{
        std::cout << "LUA OUTPUT: " << text << std::endl;
}
