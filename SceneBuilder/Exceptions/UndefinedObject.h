#pragma once
#include "SceneBuilderException.h"


struct UndefinedObject : public SceneBuilderException {
    UndefinedObject(const std::string& name, const Position& pos)
        : name(name), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Undefined object " + name + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string name;
    const Position pos;
};