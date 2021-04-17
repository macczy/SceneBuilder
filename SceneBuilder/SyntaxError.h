#pragma once
#include <exception>
//#include <istream>
//#include <limits>
#include <string>
#include <algorithm>
#include "Token.h"

struct SyntaxError : public std::exception
{
    SyntaxError(std::string errorMessage) : errorMessage(errorMessage) {};
    const char* what() const throw ()
    {
        return errorMessage.c_str();
    }
private:
    std::string errorMessage;
};