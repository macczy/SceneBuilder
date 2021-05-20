//
// pch.cpp
// Include the standard header and generate the precompiled header.
//

#include "pch.h"

#include "../SceneBuilder/Util/Token.cpp"
#include "../SceneBuilder/Util/ReturnType.cpp"
#include "../SceneBuilder/Objects/Expression.cpp"
#include "../SceneBuilder/Objects/Comparison.cpp"
#include "../SceneBuilder/Objects/Addition.cpp"
#include "../SceneBuilder/Objects/Multiplication.cpp"
#include "../SceneBuilder/Objects/LogicalExpression.cpp"
#include "../SceneBuilder/Objects/BasicObject.cpp"
#include "../SceneBuilder/Objects/ComplexObject.cpp"
#include "../SceneBuilder/Scanner/Scanner.cpp"
#include "../SceneBuilder/Parser/Parser.cpp"
#include "../SceneBuilder/Analizer/Analizer.cpp"