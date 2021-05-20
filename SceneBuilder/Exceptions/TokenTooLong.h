#pragma once
#include "SceneBuilderException.h"

struct TokenTooLong : public SceneBuilderException {
    TokenTooLong(const std::string& got, const Position& pos)
        : got(got), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string(got + " exceeded maximum length " + pos.toString() + "\n");
    }
    const std::string got;
    const Position pos;
};