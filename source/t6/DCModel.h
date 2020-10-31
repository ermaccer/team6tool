#pragma once
#include <string>
#include <vector>
/*
    Helping Structs


*/
#pragma pack(push, 1)
struct group_entry {
	int unk;
	int materialID;
	int vertBegin;
	int vertSize;
	int facesBegin;
	int facesSize;
};
#pragma (pop)
struct group_data {
	std::vector<group_entry> vGroups;
};

struct mat_property {
	std::string  propertyName;
	float        values[4];
	bool         isTexture;
	std::string  textureName;
};

struct material_entry {
	std::string  matName;
	std::vector<mat_property> vProp;
};


mat_property FindProperty(std::vector<mat_property> prop, std::string type);
mat_property FindFirstTexture(std::vector<mat_property> prop);
bool         AreTexturesPresent(std::vector<mat_property> prop);