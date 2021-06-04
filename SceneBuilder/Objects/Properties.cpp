#include "Property.h"

Property* getPropertyByName(const std::vector<PropertyPtr>& properties, const std::string& name)
{
	for (auto& prop : properties)
	{
		if (prop->getName() == name)
			return prop.get();
	}
	return nullptr;
}