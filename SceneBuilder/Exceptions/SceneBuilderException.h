#pragma once
#include <functional>
#include <exception>
#include <string>
#include "../Util/Token.h"

struct SceneBuilderException : public std::exception {
    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() = 0;
};