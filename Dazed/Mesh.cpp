#include <iostream>
#include <fstream>
#include <sstream>
#include "Mesh.h"

bool ObjParser::ParserMesh(const char* path,Mesh& mesh){
	std::ifstream in;

	// open with read mode.
	in.open(path,std::ifstream::in);

	if(in.fail()){
		std::cout<<"open obj error."<<std::endl;
		return false;
	}

	std::string line;
	while (!in.eof())
	{
		char trash;
		std::getline(in,line);
		std::istringstream ssline(line.c_str());

		if (!line.compare(0,2,"v ")) //vertex
		{
			ssline >> trash; //push "v" in trash.
			vec3f vert;
			for (int i = 0; i < 3; i++)
			{
				float tmp;
				ssline >> tmp;
				vert.raw[i] = tmp;
			}
			mesh.vertexs.push_back(vert);
		}

		if(!line.compare(0,3,"vn ")){
			ssline >> trash >> trash; //push "vn" in trash.
			vec3f normal;
			for (int i = 0; i < 3; i++)
			{
				float tmp;
				ssline >> tmp;
				normal.raw[i] = tmp;
			}
			mesh.vertexsNormal.push_back(normal);
		}

		if(!line.compare(0,3,"vt ")){
			ssline >> trash >> trash; //push "vn" in trash.
			vec3f uv;
			for (int i = 0; i < 2; i++)
			{
				float tmp;
				ssline >> tmp;
				uv.raw[i] = tmp;
			}
			mesh.vertexTextures.push_back(uv);
		}

		if(!line.compare(0,2,"f ")){
			ssline >> trash; //push "f" in trash.

			int tmp[3][3];
			ssline >> tmp[0][0]>>trash>>tmp[1][0]>>trash>>tmp[2][0];
			ssline >> tmp[0][1]>>trash>>tmp[1][1]>>trash>>tmp[2][1];
			ssline >> tmp[0][2]>>trash>>tmp[1][2]>>trash>>tmp[2][2];

			mesh.faceVertexIndex.push_back(vec3i(tmp[0]));
			mesh.faceTextureIndex.push_back(vec3i(tmp[1]));
			mesh.faceNormalIndex.push_back(vec3i(tmp[2]));
		}

	}
	mesh.NumVertex = mesh.vertexs.size();
	mesh.NumFaces = mesh.faceVertexIndex.size();

	std::cout<<"Vert Num: "<<mesh.NumVertex<<". Face Num: "<<mesh.NumFaces<<std::endl;

	in.clear();
	in.seekg(0, in.beg);
	in.close();
	return true;
}