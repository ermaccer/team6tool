#include "DCModel.h"

mat_property FindProperty(std::vector<mat_property> prop, std::string type)
{
	int iFind = 0;
	for (int i = 0; i < prop.size(); i++)
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
	int iFind = 0;
	for (int i = 0; i < prop.size(); i++)
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
	int iFind = false;
	for (int i = 0; i < prop.size(); i++)
	{
		if (prop[i].isTexture)
		{
			iFind = true;
			break;
		}
	}
	return iFind;
}
