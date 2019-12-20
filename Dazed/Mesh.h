#pragma once
#include "Maths.h"


struct Mesh
{
	int NumVertex;
	int NumFaces;
	std::vector<vec3f> vertexs;
	std::vector<vec3f> vertexTextures;
	std::vector<vec3f> vertexsNormal;

	std::vector<vec3i> faceVertexIndex;
	std::vector<vec3i> faceTextureIndex;
	std::vector<vec3i> faceNormalIndex;
};



class ObjParser
{  
public:
	static bool ParserMesh(const char*,Mesh&);
};

