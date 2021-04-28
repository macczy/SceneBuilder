#pragma once
#include <exception>
#include <string>
#include <functional>
#include "Scanner/Token.h"

struct SyntaxError : public std::exception
{
    SyntaxError(std::string errorMessage) : errorMessage(errorMessage) {}

    SyntaxError(const std::string& expected, const std::string& got, const Position& pos, std::function<std::string(Position)> getLineError) {
        errorMessage = "Expected " + expected + ", but got " + got + " " + pos.toString() + "\n" + getLineError(pos);
    }

    const char* what() const throw ()
    {
        return errorMessage.c_str();
    }
private:
    std::string errorMessage;
};