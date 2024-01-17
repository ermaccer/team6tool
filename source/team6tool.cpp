// team6tool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>
#include "eObj.h"
#include "t6/DCHeader.h"
#include "t6/DCModel.h"
#include "dds.h"

enum eModes {
	MODE_EXPORT = 1,
	MODE_IMPORT

};


std::streampos getSizeToEnd(std::ifstream& is)
{
	auto currentPosition = is.tellg();
	is.seekg(0, is.end);
	auto length = is.tellg() - currentPosition;
	is.seekg(currentPosition, is.beg);
	return length;
}

int main(int argc, char* argv[])
{
	if (argc == 1) {
		std::cout << "Team6 Tool - work with Team6 files by ermaccer\n"
			<< "Usage: team6tool <params> <file>\n"
			<< "    -e              Extracts Team6 File formats\n"
			<< "    -i              Ignore group data\n"
			<< "    -c              Input file is character model\n"
			<< "    -f              Fix black materials\n"
			<< "    -o              Specifies a folder for extraction\n";
		return 1;
	}


	int mode = 0;
	bool ignoreGroups = false;
	bool character = false;
	bool matfix = false;
	std::string o_param;
	// params
	for (int i = 1; i < argc - 1; i++)
	{
		if (argv[i][0] != '-' || strlen(argv[i]) != 2) {
			return 1;
		}
		switch (argv[i][1])
		{
		case 'e': mode = MODE_EXPORT;
			break;
		case 'i': ignoreGroups = true;
			break;
		case 'c': character = true;
			break;
		case 'f': matfix = true;
			break;
		case 'o':
			i++;
			o_param = argv[i];
			break;
		default:
			std::cout << "ERROR: Param does not exist: " << argv[i] << std::endl;
			break;
		}
	}

	if (mode == MODE_EXPORT)
	{
		std::ifstream pFile(argv[argc - 1], std::ifstream::binary);

		if (!pFile)
		{
			std::cout << "ERROR: Could not open: " << argv[argc - 1] << "!" << std::endl;
			return 1;
		}

		char dcHeader[7] = {};
		pFile.read((char*)&dcHeader, sizeof(dcHeader));

		if (!((dcHeader[0] == 'D' && dcHeader[1] == 'C' && dcHeader[2] == '2') || (dcHeader[0] == 'D' && dcHeader[1] == 'C' && dcHeader[2] == '2' && dcHeader[2] == 'P')))
		{
			std::cout << "ERROR: " << argv[argc - 1] << " is not a valid Team6 file!" << std::endl;
			return 1;
		}

		if (!o_param.empty())
		{
			if (!std::filesystem::exists(o_param))
				std::filesystem::create_directory(o_param);
			std::filesystem::current_path(o_param);
		}

		if ((dcHeader[0] == 'D' && dcHeader[1] == 'C' && dcHeader[2] == '2'))
		{
			std::cout << "INFO: Gen2 File" << std::endl;
			int resourceType;
			pFile.read((char*)&resourceType, sizeof(int));

	

			if (resourceType > RESOURCE_MODEL)
				resourceType = RESOURCE_TEXTURE;

			if (dcHeader[3] == 1)
				resourceType = RESOURCE_FLATOUT_3_TEXTURE;

			std::cout << "INFO: Type: " << GetResourceType(resourceType) << std::endl;

			if (resourceType == RESOURCE_MODEL)
			{
				model_info_new mdl;
			    pFile.read((char*)&mdl, sizeof(model_info_new));

				if (character)
				{
					std::cout << "char\n";
					pFile.seekg(sizeof(dc_header_new) - 1, pFile.beg);
					model_info_new_char chr;
					pFile.read((char*)&chr, sizeof(model_info_new_char));
					mdl.vertices = chr.vertices;
					pFile.seekg(0x4C, pFile.beg);
				}

				std::cout << "INFO: Vertices: " << mdl.vertices << std::endl;

				std::vector<obj_v> vVerts;
				std::vector<obj_uv> vMaps;
				std::vector<obj_vn> vNorm;
				std::vector<obj_face> vFaces;
				std::vector<std::string> vTextures;
				std::vector<group_data> vGroups;

				for (int i = 0; i < mdl.vertices; i++)
				{
					// x, y, z
					obj_v v;
					pFile.read((char*)&v, sizeof(obj_v));
					vVerts.push_back(v);

					obj_vn vn;
					pFile.read((char*)&vn, sizeof(obj_vn));
					vNorm.push_back(vn);

					if (character)
					{
						int unk;
						pFile.read((char*)&unk, sizeof(int));
					}

					// u, v
					obj_uv uv;
					pFile.read((char*)&uv, sizeof(obj_uv));
					vMaps.push_back(uv);

	
				}

		

				int faces;
				std::cout << pFile.tellg() << std::endl;
				pFile.read((char*)&faces, sizeof(int));
				

				std::cout << "INFO: Faces: " << faces << std::endl;

				for (int i = 0; i < faces / 3; i++)
				{
					obj_face face;
					pFile.read((char*)&face, sizeof(obj_face));
					vFaces.push_back(face);
				}
				int materials;
				pFile.read((char*)&materials, sizeof(int));
				std::cout << "INFO: Materials: " << materials << std::endl;
				std::cout << "------------------------------------------" << std::endl;
				std::vector<material_entry> vMaterials;

				for (int i = 0; i < materials; i++)
				{
				
					char strtmp[128] = {};
					char tstrtmp[128] = {};
			
					material_entry newMat;
					std::vector<mat_property> vTempProperty;
					int matLen;
					pFile.read((char*)&matLen, sizeof(int));
					std::unique_ptr<char[]> matName = std::make_unique<char[]>(matLen);
					pFile.read(matName.get(), matLen);
					strcpy(strtmp, (char*)matName.get());
					std::string strName(strtmp, matLen);
					newMat.matName = strName;
	
					int properties;
					pFile.read((char*)&properties,sizeof(int));

					vTempProperty.clear();


					for (int a = 0; a < properties; a++)
					{
						mat_property prop;
						
					
						int propLen = 0;
					    pFile.read((char*)&propLen, sizeof(int));
						std::unique_ptr<char[]> propName = std::make_unique<char[]>(propLen);
						pFile.read(propName.get(), propLen);
						strcpy(strtmp, (char*)propName.get());
						std::string strName(strtmp, propLen);
						prop.propertyName = strName;

						int isTexture = 0;
						pFile.read((char*)&isTexture, sizeof(int));
						prop.isTexture = isTexture;
						int dataLen;
						pFile.read((char*)&dataLen, sizeof(int));

						if (isTexture)
						{
							std::unique_ptr<char[]> textName = std::make_unique<char[]>(dataLen);
							pFile.read(textName.get(), dataLen);
							strcpy(tstrtmp, (char*)textName.get());
							std::string newText(tstrtmp, dataLen);
							prop.textureName = newText;
						}
						else
						{
							std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(dataLen);
							pFile.read(dataBuff.get(), dataLen);
							if (dataLen == 16)
							{
								float values[4];
								prop.values[0] = (float)dataBuff[0];
								prop.values[1] = (float)dataBuff[4];
								prop.values[2] = (float)dataBuff[8];
								prop.values[3] = (float)dataBuff[12];
							}

						}
						vTempProperty.push_back(prop);
					}


					newMat.vProp = vTempProperty;
					vMaterials.push_back(newMat);
				}

				for (int i = 0; i < vMaterials.size(); i++)
				{
					std::cout << "Material [" << i << "]" << std::endl;
					std::cout << "Type: " << vMaterials[i].matName.c_str() << std::endl;
					std::cout << "Properties: " << vMaterials[i].vProp.size() << std::endl;

					for (int a = 0; a < vMaterials[i].vProp.size(); a++)
					{
						std::cout << "Property [" << a << "] - " << vMaterials[i].vProp[a].propertyName.c_str() << std::endl;
						if (vMaterials[i].vProp[a].isTexture)
						{
							std::cout << "Texture Name: " << vMaterials[i].vProp[a].textureName.c_str() << std::endl;
						}
						else
						{
							std::cout << "Values [";
							for (int x = 0; x < 4; x++)
							{
								std::cout << " " << vMaterials[i].vProp[a].values[x];
							}
							std::cout << " ]" << std::endl;
						}
					}

					std::cout << "------------------------------------------" << std::endl;
				}
				
				if (!ignoreGroups)
				{
					int groups;
					pFile.read((char*)&groups, sizeof(int));
					std::cout << "INFO: Groups: " << groups << std::endl;
					std::cout << "------------------------------------------" << std::endl;
					for (int i = 0; i < groups; i++)
					{
						// skip 2 vectors & unk
						group_data group;
						std::vector<group_entry> vTempGroups;
						float vec[6];

						pFile.read((char*)&vec, sizeof(vec));
						int extraParamLen;

						pFile.read((char*)&extraParamLen, sizeof(int));

						if (extraParamLen > 0)
						{
							std::unique_ptr<char[]> extraData = std::make_unique<char[]>(extraParamLen);
							pFile.read(extraData.get(), extraParamLen);
						}
						int groupInfos;
						pFile.read((char*)&groupInfos, sizeof(int));
						std::cout << "INFO: Group [" << i << "] Infos: " << groupInfos << std::endl;
						for (int a = 0; a < groupInfos; a++)
						{
							group_entry grp;


							pFile.read((char*)&grp, sizeof(group_entry));
							vTempGroups.push_back(grp);
						}
						group.vGroups = vTempGroups;
						vTempGroups.clear();
						vGroups.push_back(group);

					}

					for (int i = 0; i < vGroups.size(); i++)
					{
						for (int a = 0; a < vGroups[i].vGroups.size(); a++)
						{
							std::cout << "Group [" << a << "] ";
							std::cout << "Mat: " << vGroups[i].vGroups[a].materialID << " ";
							std::cout << "Faces: " << vGroups[i].vGroups[a].facesSize << " ";
							std::cout << "Verts: " << vGroups[i].vGroups[a].vertSize << std::endl;
						}
					}

					int baseFaceSize = 0;
					int baseVertSize = 0;


					// generate mat
					std::string matoutput = argv[argc - 1];
					matoutput.replace(matoutput.length() - 4, 4, ".mtl");
					matoutput.insert(0, "material_");
					std::ofstream mtl(matoutput, std::ofstream::binary);

					mtl << "# created using team6tool\n";
					for (int i = 0; i < vMaterials.size(); i++)
					{
						mtl << "newmtl mat" << std::to_string(i) << std::endl;
						mat_property ambient = FindProperty(vMaterials[i].vProp, "MatAmbient");
						mat_property diffuse = FindProperty(vMaterials[i].vProp, "MatDiffuse");
						mat_property spec = FindProperty(vMaterials[i].vProp, "MatSpecular");

						if (matfix)
						{
							ambient.values[0] = 1.0f;
							ambient.values[1] = 1.0f;
							ambient.values[2] = 1.0f;
							ambient.values[3] = 1.0f;
						}

						mtl << "Ka " << ambient.values[0] << " " << ambient.values[1] << " " << ambient.values[2] << std::endl;

						if (!matfix)
						{
							mtl << "Kd " << diffuse.values[0] << " " << diffuse.values[1] << " " << diffuse.values[2] << std::endl;
							mtl << "Ks " << spec.values[0] << " " << spec.values[1] << " " << spec.values[2] << std::endl;
						}
	

						std::string texture = "default.dds";

						if (AreTexturesPresent(vMaterials[i].vProp))
						{

							mat_property tex = FindFirstTexture(vMaterials[i].vProp);

							if (tex.textureName.length() > 0)
							{
								texture = tex.textureName;
								texture.replace(texture.length() - 4, 4, ".dds");
							}


							mtl << "map_Kd " << texture.c_str() << std::endl;
						}

					}


					std::string output = argv[argc - 1];
					output.replace(output.length() - 4, 4, ".obj");
					output.insert(0, "model_");

					std::ofstream obj(output, std::ofstream::binary);

					obj << "# created using team6tool\n";
					obj << "mtllib " << matoutput.c_str() << std::endl;
					int baseGroup = 0;
					for (int i = 0; i < vGroups.size(); i++)
					{

						for (int a = 0; a < vGroups[i].vGroups.size(); a++)
						{


							for (int z = 0; z < vGroups[i].vGroups[a].vertSize; z++)
							{
								obj << "v " << vVerts[baseVertSize + z].x << " " << vVerts[baseVertSize + z].y << " " << vVerts[baseVertSize + z].z << std::endl;
							}
							for (int z = 0; z < vGroups[i].vGroups[a].vertSize; z++)
							{
								obj << "vn " << vNorm[baseVertSize + z].norm[0] << " " << vNorm[baseVertSize + z].norm[1] << " " << vNorm[baseVertSize + z].norm[2] << std::endl;
							}
							for (int z = 0; z < vGroups[i].vGroups[a].vertSize; z++)
							{
								obj << "vt " << vMaps[baseVertSize + z].u << " " << 1.0f - vMaps[baseVertSize + z].v << std::endl;
							}

							obj << "g " << "object" << std::to_string(baseGroup) << std::endl;
							obj << "usemtl " << "mat" << std::to_string(vGroups[i].vGroups[a].materialID) << std::endl;
							for (int x = 0; x < vGroups[i].vGroups[a].facesSize / 3; x++)
							{
								int temp[3];
								temp[0] = vFaces[x + baseFaceSize].face[0];
								temp[1] = vFaces[x + baseFaceSize].face[1];
								temp[2] = vFaces[x + baseFaceSize].face[2];
								obj << "f " << temp[0] + 1 << "/" << temp[0] + 1 << "/" << temp[0] + 1
									<< " "
									<< temp[1] + 1 << "/" << temp[1] + 1 << "/" << temp[1] + 1
									<< " "
									<< temp[2] + 1 << "/" << temp[2] + 1 << "/" << temp[2] + 1 << std::endl;
							}


							baseFaceSize += vGroups[i].vGroups[a].facesSize / 3;
							baseVertSize += vGroups[i].vGroups[a].vertSize;

							std::cout << "Saved Group " << baseGroup << " in " << output.c_str() << std::endl;
							baseGroup++;
						}

					}
					int objectInfo;
					pFile.read((char*)&objectInfo, sizeof(int));
					std::cout << "INFO: Object Infos: " << objectInfo << std::endl;
					std::cout << "------------------------------------------" << std::endl;
					for (int i = 0; i < objectInfo; i++)
					{
						int nameLen = 0;
						std::string nameInfo, fxInfo;
						pFile.read((char*)&nameLen, sizeof(int));

						if (nameLen > 0)
						{
							std::unique_ptr<char[]> nameData = std::make_unique<char[]>(nameLen);
							pFile.read(nameData.get(), nameLen);

							std::string str(nameData.get(), nameLen);
							nameInfo = str;
						}

						float pos[3];
						pFile.read((char*)&pos, sizeof(pos));

						float unk[7];
						pFile.read((char*)&unk, sizeof(unk));

						int field28;
						pFile.read((char*)&field28, sizeof(int));

						int fxLen = 0;
						pFile.read((char*)&fxLen, sizeof(int));

						if (fxLen > 0)
						{
							std::unique_ptr<char[]> fxData = std::make_unique<char[]>(fxLen);
							pFile.read(fxData.get(), fxLen);

							std::string str(fxData.get(), fxLen);
							fxInfo = str;
						}

						int infoGroupID;
						pFile.read((char*)&infoGroupID, sizeof(int));

						int lastField;
						pFile.read((char*)&lastField, sizeof(int));

						std::cout << "INFO: Group [ " << infoGroupID << " ] Name: " << nameInfo << " | POS : " << pos[0] << " " << pos[1] << " " << pos[2] << std::endl;
					}

					// would use eObj but no support for multiple objects yet, todo
		
				}
				else
				{
				eObj model;
				std::string output = argv[argc - 1];
				output.replace(output.length() - 4, 4, ".obj");
				output.insert(0, "model_");
		        model.SetModelData(vVerts, vMaps, vNorm, vFaces);
		        model.SaveFile(output.c_str(), "group", "default");
				}

				
			}
			if (resourceType == RESOURCE_TEXTURE || resourceType == RESOURCE_FLATOUT_3_TEXTURE)
			{
				int width, height;
				if (resourceType == RESOURCE_FLATOUT_3_TEXTURE)
				{
					pFile.seekg(27, pFile.beg);
					pFile.read((char*)&width, sizeof(int));
					pFile.seekg(31, pFile.beg);
					pFile.read((char*)&height, sizeof(int));
					pFile.seekg(0x3E, pFile.beg);
				}
				else
				{
					texture_info_new texture;
					pFile.read((char*)&texture, sizeof(texture_info_new));


			
					width = texture.width;
					height = texture.height;

					if ((int)texture._pad[4] == 4)
					{
						width = height;
						height = texture.flags[1];
						pFile.seekg(0x3A, pFile.beg);
					}
				}

				std::cout << "INFO: Size: " << width << "x" << height << std::endl;
				int dataSize = getSizeToEnd(pFile);
				std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(dataSize);
				pFile.read(dataBuff.get(), dataSize);

				std::string output = argv[argc - 1];
				output.replace(output.length() - 4, 4, ".dds");

				std::ofstream oFile(output, std::ofstream::binary);
				
				dds_header dds;
				dds.magic = ' SDD';

				dds.dwSize = (sizeof(dds_header) - sizeof(dds.magic));
				dds.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT | DDSD_MIPMAPCOUNT | DDSD_LINEARSIZE;
				dds.dwHeight = height;
				dds.dwWidth = width;
				dds.dwPitchOrLinearSize = 0;
				dds.dwDepth = 0;
				dds.dwMipMapCount = 0;
				dds.ddspf.dwSize = sizeof(DDS_PIXELFORMAT);
				dds.ddspf.dwFlags = DDPF_FOURCC;
				dds.ddspf.dwFourCC[0] = 'D';
				dds.ddspf.dwFourCC[1] = 'X';
				dds.ddspf.dwFourCC[2] = 'T';
				dds.ddspf.dwFourCC[3] = '1';
				dds.ddspf.dwRGBBitCount = 256;
				dds.ddspf.dwRBitMask = 0x00000000;
				dds.ddspf.dwGBitMask = 0x00000000;
				dds.ddspf.dwBBitMask = 0x00000000;
				dds.ddspf.dwABitMask = 0x00000000;
				dds.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
				// no need
				dds.dwCaps2 = 0;
				dds.dwCaps3 = 0;
				dds.dwCaps4 = 0;
				dds.dwReserved2 = 0;

				oFile.write((char*)&dds, sizeof(dds_header));
				oFile.write(dataBuff.get(), dataSize);

				std::cout << "INFO: Saved as " << output << std::endl;

			}

		}

	}
}

