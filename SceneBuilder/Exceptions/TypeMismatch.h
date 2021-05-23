#pragma once
#include "SceneBuilderException.h"
#include "../Util/ReturnType.h"

struct TypeMismatch : SceneBuilderException {
    TypeMismatch(const std::string& first, const std::string& second, const Position& pos)
        : first(first), second(second), pos(pos) {}
    TypeMismatch(const ReturnType& first, const ReturnType& second, const Position& pos)
        : first(toString(first)), second(toString(second)), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Type mismatch: " + first + ", and " + second + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string first;
    const std::string second;
    const Position pos;
};