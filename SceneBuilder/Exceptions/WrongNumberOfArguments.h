#pragma once
#include "SceneBuilderException.h"


struct WrongNumberOfArguments : public SceneBuilderException {
    WrongNumberOfArguments(int required, int got, const std::string& name, const Position& pos)
        : name(name), required(required), got(got), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Incorrect number of arguments to call " + name + ", provided " + std::to_string(got) +", required: " + std::to_string(required) + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string name;
    int required;
    int got;
    const Position pos;
};