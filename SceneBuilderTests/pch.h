//
// pch.h
// Header for standard system include files.
//

#pragma once

#include "gtest/gtest.h"

#include <deque>
#include <exception>
#include <sstream>
#include "../SceneBuilder/Exceptions/TokenTooLong.h"
#include "../SceneBuilder/Exceptions/SyntaxError.h"
#include "../SceneBuilder/Exceptions/Redefinition.h"
#include "../SceneBuilder/Exceptions/TokenTooLong.h"
#include "../SceneBuilder/Exceptions/TypeMismatch.h"
#include "../SceneBuilder/Objects/AnimationDeclaration.h"
#include "../SceneBuilder/Util/Token.h"
#include "../SceneBuilder/Util/ReturnType.h"
#include "../SceneBuilder/Objects/Expression.h"
#include "../SceneBuilder/Objects/Comparison.h"
#include "../SceneBuilder/Objects/Addition.h"
#include "../SceneBuilder/Objects/Multiplication.h"
#include "../SceneBuilder/Objects/LogicalExpression.h"
#include "../SceneBuilder/Objects/BasicObject.h"
#include "../SceneBuilder/Objects/ComplexObject.h"
#include "../SceneBuilder/Scanner/Scanner.h"
#include "../SceneBuilder/Parser/Parser.h"
#include "../SceneBuilder/Analizer/Analizer.h"