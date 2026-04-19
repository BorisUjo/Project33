#pragma once

#include <vector>

struct Vec3
{
	float x, y, z;
	Vec3(float x, float y, float z) : x(x), y(y), z(z) {}
	Vec3() : x(0), y(0), z(0) {}
};

struct Vec2
{
	float u, v;
	Vec2(float x, float y) : u(x), v(y) {}
	Vec2() : u(0), v(0) {}
};

struct Vertex
{
	Vec3 position;
	Vec3 normal;
	Vec2 texCoord;
};

struct Face
{
	unsigned int vertexIndices[3];
	unsigned int normalIndices[3];
	unsigned int texCoordIndices[3];
};

struct Mesh
{
	std::vector<Vertex> vertices;
	unsigned int indicesCount;
};

void loadMeshFromFile(const char* path, Mesh& mesh);
void loadMeshFromFile(const char* path, bool flipV, Mesh& mesh);
