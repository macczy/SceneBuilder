#pragma once
#include <variant>
#include <any>
#include "Color.h"
#include "DecimalValue.h"
#include "HexValue.h"
#include "Point.h"
#include "Identifier.h"
#include "PointArray.h"
#include "TimeDeclaration.h"
#include "ConstantIdentifier.h"

class AnimationProperty;
class Addition;
using AdditionPtr = std::unique_ptr<Addition>;

class Multiplication;
using MultiplicationPtr = std::unique_ptr<Multiplication>;
class TernaryExpression;
using TernaryExpressionPtr = std::unique_ptr<TernaryExpression>;

using Expression = std::variant<DecimalValue, HexValue, Color, Point, Identifier, ConstantIdentifier, AnimationProperty, TimeDeclaration, PointArray, TernaryExpressionPtr, MultiplicationPtr, AdditionPtr>;
using ExpressionPtr = std::unique_ptr<Expression>;
class Comparison;
using ComparisonPtr = std::unique_ptr<Comparison>;
class LogicalExpression;
using LogicalExpressionPtr = std::unique_ptr<LogicalExpression>;
using LogicalSubExpression = std::variant<LogicalExpressionPtr, ComparisonPtr>;
using LogicalSubExpressionPtr = std::unique_ptr<LogicalSubExpression>;

const Position& getExpressionPosition(const Expression& val);

#include "AnimationProperty.h"
#include "Addition.h"
#include "Multiplication.h"
#include "Comparison.h"
#include "LogicalExpression.h"
#include "TernaryExpression.h"