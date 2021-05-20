#include "ReturnType.h"


std::ostream& operator<< (std::ostream& stream, const ReturnType& type) {
	stream << toString(type);
	return stream;
}


std::string toString(const ReturnType& type) {
	switch (type) {
	case	ReturnType::NON_TYPE: return "NON_TYPE";
	case 	ReturnType::DECIMAL_VALUE: return "DECIMAL_VALUE";
	case	ReturnType::POINT: return "POINT";
	case	ReturnType::POINT_ARRAY: return "POINT_ARRAY";
	case	ReturnType::COLOR: return "COLOR";
	case	ReturnType::TIME_DECLARATION: return "TIME_DECLARATION";
	case	ReturnType::BUILD_IN_CONSTANT: return "BUILD_IN_CONSTANT";
	//out of scope
	default: return "UNDEFINED TYPE";
	}
}