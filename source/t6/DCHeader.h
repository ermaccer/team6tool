#pragma once

enum eResourceType {
	RESOURCE_TEXTURE = 1,
	RESOURCE_2,
	RESOURCE_MODEL,
	RESOURCE_FLATOUT_3_TEXTURE = 10001
};

enum eTextureFormat {
	TEXTURE_FORMAT_RGBA = 0,
	TEXTURE_FORMAT_RGB = 5,
	TEXTURE_FORMAT_DXT1 = 8,
	TEXTURE_FORMAT_DXT5 = 9
};

struct dc_header_new {
	char header[7];
	int  resourceType;
};

#pragma pack(push, 1)
struct texture_info_new {
	char _pad[8]; // flag1,2
	int  width;   // flag3,4
	int  height;  // 20
	int  flags[5];
	char pad;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct model_info_new {
	int flags[11];
	int vertices;
	int flag;
	char pad[5];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct model_info_new_char {
	int flags[13];
	int vertices;
	int flag;
	char pad[5];
};
#pragma pack(pop)


const char* GetResourceType(int resource);