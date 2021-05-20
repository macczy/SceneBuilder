#pragma once
#include "SceneBuilderException.h"

struct SyntaxError : public SceneBuilderException {
    SyntaxError(const std::string& expected, const std::string& got, const Position& pos)
        : expected(expected), got(got), pos(pos) {}
    SyntaxError(const std::string& expected, Token& token) : expected(expected), got(token.getValue()), pos(token.getPosition()) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Expected " + expected + ", but got " + got + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string expected;
    const std::string got;
    const Position pos;
};