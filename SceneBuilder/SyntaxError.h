#pragma once
#include <exception>
#include <istream>
#include <limits>
#include <algorithm>
#include "Token.h"

struct SyntaxError : public std::exception
{
    SyntaxError(std::string errorMessage, Token::Position position) : errorMessage(errorMessage), position(position) {};
    const char* what() const throw ()
    {
        return errorMessage.c_str();
    }

    std::string visit(std::istream& input) const throw () {
        std::string buffer;
        input.seekg(0);

        for (unsigned int currLineNumber = 0; currLineNumber < position.lineNumber; ++currLineNumber) {
            if (!input.ignore(std::numeric_limits<std::streamsize>::max(), input.widen('\n'))) {
                //throw std::runtime_error("Number of lines in source is less than " + std::to_string(position.lineNumber)); //if there is no line with this line number, throw error
            }
        }
        std::getline(input, buffer);
        std::replace(buffer.begin(), buffer.end(), '\t', ' ');
        buffer += "\n" + std::string(position.columnNumber-1, ' ') + "^";

        return buffer;
    }
private:
    Token::Position position;
    std::string errorMessage;
};