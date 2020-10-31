#pragma once

enum eResourceType {
	RESOURCE_TEXTURE = 1,
	RESOURCE_2,
	RESOURCE_MODEL,
	RESOURCE_FLATOUT_3_TEXTURE = 10001
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
#pragma (pop)
#pragma pack(push, 1)
struct model_info_new {
	int flags[11];
	int vertices;
	int flag;
	char pad[5];
};
#pragma (pop)

#pragma pack(push, 1)
struct model_info_new_char {
	int flags[13];
	int vertices;
	int flag;
	char pad[5];
};
#pragma (pop)


const char* GetResourceType(int resource);