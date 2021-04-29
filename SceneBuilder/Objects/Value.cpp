#include "Value.h"

const Position& getValuePosition(Value& val) {
    switch (val.index()) {
    case VALUE::DECIMAL_VALUE_INDEX:
        return std::get<DecimalValue>(val).getPosition();
    case VALUE::HEX_VALUE_INDEX:
        return std::get<HexValue>(val).getPosition();
    case VALUE::COLOR_INDEX:
        return std::get<Color>(val).getPosition();
    case VALUE::POINT_INDEX:
        return std::get<Point>(val).getPosition();
    case VALUE::IDENTIFIER_INDEX:
        return std::get<Identifier>(val).getPosition();
    default:
        throw std::runtime_error("Uninitilized variant");
    }
}