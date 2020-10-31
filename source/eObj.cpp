#include "eobj.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <filesystem>

bool eObj::SaveFile(const char * name, const char* meshName, const char* textureName)
{
	std::string obj = name;
	obj += ".obj";
	std::ofstream oFile(obj, std::ofstream::binary);
	std::string mtl = name;
	mtl.insert(0, "mtl_");
	mtl += ".mtl";

	std::ofstream mtlFile(mtl, std::ofstream::binary);
	if (!oFile)
	{
		std::cout << "ERROR: Could not open " << name << "for writing !" << std::endl;
		return false;
	}

	oFile << "mtllib " << mtl << std::endl;

	for (int i = 0; i < gvVerts.size(); i++)
	{
		// 3 verts, 3 normals, 2 uv
		oFile << "v " << gvVerts[i].x << " " << gvVerts[i].y << " " << gvVerts[i].z << std::endl;
	}

	for (int i = 0; i < gvNorm.size(); i++)
	{
		oFile << "vn " << gvNorm[i].norm[0] << " " << gvNorm[i].norm[1] << " " << gvNorm[i].norm[2] << std::endl;
	}

	for (int i = 0; i <gvMaps.size(); i++)
	{
		oFile << "vt " <<gvMaps[i].u << " " << 1.0f - gvMaps[i].v << std::endl;
	}

	oFile << "g " << meshName << std::endl;
	oFile << "usemtl mat" << std::endl;

	for (int i = 0; i < faces.size(); i++)
	{
		int temp[3];
		temp[0] = faces[i].face[0];
		temp[1] = faces[i].face[1];
		temp[2] = faces[i].face[2];
		oFile << "f " << temp[0] + 1 << "/" << temp[0] + 1 << "/" << temp[0] + 1
			<< " "
			<< temp[1] + 1 << "/" << temp[1] + 1 << "/" << temp[1] + 1
			<< " "
			<< temp[2] + 1 << "/" << temp[2] + 1 << "/" << temp[2] + 1 << std::endl;

	}

	mtlFile << "newmtl mat" << std::endl;
	mtlFile << "Kd 1.00 1.00 1.00" << std::endl;
	mtlFile << "map_Kd " << textureName << std::endl;
	std::cout << "Model " << obj.c_str() << " saved!" << std::endl;

	return true;
	


}

bool eObj::LoadFile(const char * name)
{
	char szLine[1024];
	FILE* pFile = fopen(name, "rb");
	if (!pFile)
	{
		std::cout << "ERROR: Could not open " << name << "!" << std::endl;
		return false;
	}

	std::cout << "INFO: Loading " << name  << std::endl;
	std::vector<obj_v> vVerts;
	std::vector<obj_uv> vMaps;
	std::vector<obj_vn> vNorm;

	while (fgets(szLine, 1024, pFile))
	{
		if (szLine[0] == '#' || szLine[0] == ';')
			continue;

		if (szLine[0] == 'f' && szLine[1] == ' ')
		{
			obj_face f;
			int fa; 
			int i = 0;
			while (szLine[i] != '\0')
			{
				if (szLine[i] == '/')
					szLine[i] = ' ';
				i++;
			}
			sscanf(szLine + 2, "%d %d %d %d %d %d %d %d %d", &f.face[0], &fa, &fa, &f.face[1], &fa, &fa, &f.face[2], &fa, &fa);

			f.face[0] -= 1;
			f.face[1] -= 1;
			f.face[2] -= 1;
			faces.push_back(f);
		}
		if (szLine[0] == 'g' && szLine[1] == ' ')
		{
			char mesh[512] = {};
			sscanf(szLine + 2, "%s", &mesh);
			std::string str(mesh, strlen(mesh));
			meshName = str;
		}
		if (szLine[0] == 'v' && szLine[1] == ' ')
		{
			obj_v vert;
			sscanf(szLine + 2, "%f %f %f", &vert.x, &vert.y, &vert.z);
			vVerts.push_back(vert);
		}
		if (szLine[0] == 'v' && szLine[1] == 't' && szLine[2] == ' ')
		{
			obj_uv map;
			sscanf(szLine + 2, "%f %f", &map.u, &map.v);
			vMaps.push_back(map);
		}
		if (szLine[0] == 'v' && szLine[1] == 'n' && szLine[2] == ' ')
		{
			obj_vn vn;
			sscanf(szLine + 3, "%f %f %f", &vn.norm[0], &vn.norm[1], &vn.norm[2]);
			vNorm.push_back(vn);
		}

		if (szLine[0] == 'm' && szLine[1] == 't' && szLine[2] == 'l'&& szLine[3] == 'l' && szLine[4] == 'i' && szLine[5] == 'b' && szLine[6] == ' ')
		{
			char file[256] = {};
			sscanf(szLine + 6, "%s", &file);
			if (!folderName.empty())
			std::filesystem::current_path(folderName);

			FILE* mtl = fopen(file, "rb");
			if (!mtl)
				printf("ERROR: Failed to open MTL %s!\n", file);

			char szLine[1024];

			while (fgets(szLine, 1024, mtl))
			{
				if (szLine[0] == 'm' && szLine[1] == 'a' && szLine[2] == 'p'&& szLine[3] == '_' && szLine[4] == 'K' && szLine[5] == 'd' && szLine[6] == ' ')
				{
					char texture[256] = {};
					sscanf(szLine + 6, "%s", &texture);
					std::string tmp(texture, strlen(texture));
					textureName = tmp;
				}
			}
		}
	}

	TheModel.v = vVerts;
	TheModel.uv = vMaps;
	TheModel.vn = vNorm;
	gvMaps = vMaps;
	gvVerts = vVerts;
	gvNorm = vNorm;

	return true;
}

void eObj::SetFolder(const char * name)
{
	std::string tmp(name, strlen(name));
	folderName = tmp;
}

void eObj::Print()
{
	printf("Loaded vertex coords:\n");
	for (int i = 0; i < TheModel.v.size(); i++)
		printf("X:%f Y:%f Z:%f\n", TheModel.v[i].x, TheModel.v[i].y, TheModel.v[i].z);

	printf("Loaded vertex uv maps (%d):\n", TheModel.uv.size());
	for (int i = 0; i < TheModel.uv.size(); i++)
		printf("U:%f V:%f\n", TheModel.uv[i].u, TheModel.uv[i].v);

	printf("Loaded vertex normals (%d):\n", TheModel.vn.size());
	for (int i = 0; i < TheModel.vn.size(); i++)
		printf("%f %f %f\n", TheModel.vn[i].norm[0], TheModel.vn[i].norm[1], TheModel.vn[i].norm[2]);

	printf("Loaded faces:\n");
	for (int i = 0; i < faces.size(); i++)
		printf("%d %d %d\n", faces[i].face[0], faces[i].face[1], faces[i].face[2]);

	printf("Faces :%d | Verts: %d\n", faces.size(), TheModel.v.size());
}

void eObj::SetModelData(std::vector<obj_v> verts, std::vector<obj_uv> uv, std::vector<obj_vn> norm, std::vector<obj_face> face)
{
	gvMaps = uv;
	gvVerts = verts;
	gvNorm= norm;
	faces = face;
	TheModel.v = verts;
	TheModel.uv = uv;
	TheModel.vn = norm;
	
}

std::vector<obj_face> eObj::GetFaces()
{
	return faces;
}

obj_verts eObj::GetVertexes()
{
	return TheModel;
}

std::string eObj::GetTextureName()
{
	return textureName;
}

std::string eObj::GetMeshName()
{
	return meshName;
}

int eObj::CalcVertSize()
{
	return TheModel.v.size() * 4 * 4 * 2;
}

int eObj::CalcFaceSize()
{
	return faces.size() * 3 * 2;
}
