#pragma once
#include <vector>

struct obj_v { float x, y, z; };
struct obj_uv { float u, v; };
struct obj_vn { float norm[3]; };
struct obj_face { int face[3]; };
struct obj_face_word { short face[3]; };

struct obj_verts
{
	std::vector<obj_v> v;
	std::vector<obj_vn> vn;
	std::vector<obj_uv> uv;
};



class eObj {
	obj_verts TheModel;
	std::vector<obj_face> faces;
	std::string textureName;
	std::string folderName;
	std::string meshName;
public:
	bool SaveFile(const char* name, const char* meshName, const char* textureName);
	bool LoadFile(const char* name);
	void SetFolder(const char* name);
	void Print();
	void SetModelData(std::vector<obj_v> verts, std::vector<obj_uv> uv, std::vector<obj_vn> norm, std::vector<obj_face> face);
	std::vector<obj_face> GetFaces();
	obj_verts GetVertexes();
	std::string GetTextureName();
	std::string GetMeshName();
	std::vector<obj_v> gvVerts;
	std::vector<obj_uv> gvMaps;
	std::vector<obj_vn> gvNorm;
	// battle mages specific

	int CalcVertSize();
	int CalcFaceSize();
};