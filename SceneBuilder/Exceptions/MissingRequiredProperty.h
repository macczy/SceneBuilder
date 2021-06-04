#pragma once
#include "SceneBuilderException.h"


struct MissingRequiredProperty : public SceneBuilderException {
    MissingRequiredProperty(const std::string& requiredPropertyName, const std::string& ClassRequiringProperty, const Position& pos)
        : requiredPropertyName(requiredPropertyName), ClassRequiringProperty(ClassRequiringProperty), pos(pos) {}

    virtual std::string what(std::function<std::string(Position)> getLineError) const throw() {
        return std::string("Missing required property " + requiredPropertyName + " in " + ClassRequiringProperty + " " + pos.toString() + "\n" + getLineError(pos));
    }
    const std::string requiredPropertyName;
    const std::string ClassRequiringProperty;
    const Position pos;
};