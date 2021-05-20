#pragma once
#include "SceneBuilderException.h"

struct NotDefined : public SceneBuilderException {
    NotDefined(const std::string& got, const Position& pos = Position())
        : got(got), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Missing definition of " + got + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string got;
    const Position pos;
};