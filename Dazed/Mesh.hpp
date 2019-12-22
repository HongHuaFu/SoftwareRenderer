#pragma once
#include "Maths.hpp"


struct Mesh
{
	int NumVertex;
	int NumFaces;
	std::vector<vec3f> vertexs;
	std::vector<vec3f> vertexTextures;
	std::vector<vec3f> vertexsNormal;

	// 序号从1开始，记得减1
	std::vector<vec3i> faceVertexIndex;

	// 序号从1开始，记得减1
	std::vector<vec3i> faceTextureIndex;

	// 序号从1开始，记得减1
	std::vector<vec3i> faceNormalIndex;
};



class ObjParser
{  
public:
	static bool ParserMesh(const char*,Mesh&);
};

