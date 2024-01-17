#include "DCModel.h"

mat_property FindProperty(std::vector<mat_property> prop, std::string type)
{
	unsigned int iFind = 0;
	for (unsigned int i = 0; i < prop.size(); i++)
	{
		if (prop[i].propertyName == type)
		{
			iFind = i;
			break;
		}
	}
	return prop[iFind];
}

mat_property FindFirstTexture(std::vector<mat_property> prop)
{
	unsigned int iFind = 0;
	for (unsigned int i = 0; i < prop.size(); i++)
	{
		if (prop[i].isTexture)
		{
			iFind = i;
			break;
		}
	}
	return prop[iFind];
}

bool AreTexturesPresent(std::vector<mat_property> prop)
{
	unsigned int iFind = false;
	for (unsigned int i = 0; i < prop.size(); i++)
	{
		if (prop[i].isTexture)
		{
			iFind = true;
			break;
		}
	}
	return iFind;
}
