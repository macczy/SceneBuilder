#pragma once
#include "SceneBuilderException.h"

struct Redefinition : public SceneBuilderException {
    Redefinition(const std::string& got, const Position& pos)
        : got(got), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Redefinition of " + got + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string got;
    const Position pos;
};
