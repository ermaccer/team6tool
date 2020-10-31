#include "DCHeader.h"

const char * GetResourceType(int resource)
{
	switch (resource)
	{
	case RESOURCE_TEXTURE: return "Texture";
		break;
	case RESOURCE_MODEL: return "Model";
		break;
	case RESOURCE_FLATOUT_3_TEXTURE: return "FlatOut3 Texture";
		break;
	default:return "Unknown";
		break;
	}
}
