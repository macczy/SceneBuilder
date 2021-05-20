#pragma once
#include "SceneBuilderException.h"

struct TypeMismatchException : SceneBuilderException {
    TypeMismatchException(const std::string& first, const std::string& second, const Position& pos)
     : first(first), second(second), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Type mismatch: " + first + ", but got " + second + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string first;
    const std::string second;
    const Position pos;
};