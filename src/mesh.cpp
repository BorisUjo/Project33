#include <mesh.h>
#include <fstream>
#include <sstream>
#include <iostream>


void loadMeshFromFile(const char* path, Mesh& mesh)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "Failed to open mesh file: " << path << std::endl;
		return;
	}

	const unsigned int BUFFER_SIZE = 16384;
	//char buffer[BUFFER_SIZE];

	char* buffer = new char[BUFFER_SIZE];


	std::string leftover;

	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> texCoords;

	std::vector<Face> faces;



	while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0)
	{
		std::streamsize bytesRead = file.gcount();
		std::string chunk = leftover + std::string(buffer, bytesRead);

		size_t lineStart = 0;
		size_t newlinePos = 0;

		while ((newlinePos = chunk.find('\n', lineStart)) != std::string::npos)
		{
			std::string line = chunk.substr(lineStart, newlinePos - lineStart);

			if (line.rfind("v ", 0) == 0)
			{
				std::istringstream iss(line.substr(2));
				float x, y, z;
				iss >> x >> y >> z;
				vertices.push_back(Vec3(x,y,z));
			}
			else if (line.rfind("vn ", 0) == 0)
			{
				std::istringstream iss(line.substr(3));
				float x, y, z;
				iss >> x >> y >> z;
				normals.push_back(Vec3(x, y, z));
			}
			else if (line.rfind("vt ", 0) == 0)
			{
				std::istringstream iss(line.substr(3));
				float u, v;
				iss >> u >> v;
				texCoords.push_back(Vec2(u, v));
			}
			else if (line.rfind("f ", 0) == 0) {
				Face face;
				std::istringstream lineStream(line.substr(2));
				std::string token;
				int idx = 0;
				while (lineStream >> token && idx < 3) {  
					std::istringstream tokenStream(token);
					std::string vertex, texture, normal;
					std::getline(tokenStream, vertex, '/');
					std::getline(tokenStream, texture, '/');
					std::getline(tokenStream, normal, '/');
					face.vertexIndices[idx] = vertex.empty() ? 0 : std::stoi(vertex);
					face.texCoordIndices[idx] = texture.empty() ? 0 : std::stoi(texture);
					face.normalIndices[idx] = normal.empty() ? 0 : std::stoi(normal);
					idx++;
				}
				faces.emplace_back(face);
			}
				 

			lineStart = newlinePos + 1;

		}

		leftover = chunk.substr(lineStart);

	}
	

	for (size_t fc = 0; fc < faces.size(); fc++)
	{
		for (size_t idx = 0; idx < 3; idx++)
		{
			size_t vi = faces[fc].vertexIndices[idx];
			size_t ni = faces[fc].normalIndices[idx];
			size_t ti = faces[fc].texCoordIndices[idx];

			if (vi == 0 || vi > vertices.size()) continue;
			if (ni == 0 || ni > normals.size()) continue;
			if (ti == 0 || ti > texCoords.size()) continue;

			Vertex vert;
			vert.position = vertices[vi - 1];
			vert.normal = normals[ni - 1];
			vert.texCoord = texCoords[ti - 1];
			mesh.vertices.emplace_back(vert);
		}
	}

	mesh.indicesCount = mesh.vertices.size();


	delete[] buffer;

	file.close();
	
}

void loadMeshFromFile(const char* path, bool flipV, Mesh& mesh)
{
	std::ifstream file(path);

	if (!file.is_open())
	{
		std::cerr << "Failed to open mesh file: " << path << std::endl;
		return;
	}

	const unsigned int BUFFER_SIZE = 16384;
	//char buffer[BUFFER_SIZE];

	char* buffer = new char[BUFFER_SIZE];


	std::string leftover;

	std::vector<Vec3> vertices;
	std::vector<Vec3> normals;
	std::vector<Vec2> texCoords;

	std::vector<Face> faces;



	while (file.read(buffer, BUFFER_SIZE) || file.gcount() > 0)
	{
		std::streamsize bytesRead = file.gcount();
		std::string chunk = leftover + std::string(buffer, bytesRead);

		size_t lineStart = 0;
		size_t newlinePos = 0;

		while ((newlinePos = chunk.find('\n', lineStart)) != std::string::npos)
		{
			std::string line = chunk.substr(lineStart, newlinePos - lineStart);

			if (line.rfind("v ", 0) == 0)
			{
				std::istringstream iss(line.substr(2));
				float x, y, z;
				iss >> x >> y >> z;
				vertices.push_back(Vec3(x, y, z));
			}
			else if (line.rfind("vn ", 0) == 0)
			{
				std::istringstream iss(line.substr(3));
				float x, y, z;
				iss >> x >> y >> z;
				normals.push_back(Vec3(x, y, z));
			}
			else if (line.rfind("vt ", 0) == 0 && !flipV)
			{
				std::istringstream iss(line.substr(3));
				float u, v;
				iss >> u >> v;
				texCoords.push_back(Vec2(u, v));
			}
			else if (line.rfind("vt ", 0) == 0 && flipV)
			{
				std::istringstream iss(line.substr(3));
				float u, v;
				iss >> u >> v;
				texCoords.push_back(Vec2(u, 1.0f - v));
			}
			else if (line.rfind("f ", 0) == 0) {
				Face face;
				std::istringstream lineStream(line.substr(2));
				std::string token;
				int idx = 0;
				while (lineStream >> token && idx < 3) {
					std::istringstream tokenStream(token);
					std::string vertex, texture, normal;
					std::getline(tokenStream, vertex, '/');
					std::getline(tokenStream, texture, '/');
					std::getline(tokenStream, normal, '/');
					face.vertexIndices[idx] = vertex.empty() ? 0 : std::stoi(vertex);
					face.texCoordIndices[idx] = texture.empty() ? 0 : std::stoi(texture);
					face.normalIndices[idx] = normal.empty() ? 0 : std::stoi(normal);
					idx++;
				}
				faces.emplace_back(face);
			}


			lineStart = newlinePos + 1;

		}

		leftover = chunk.substr(lineStart);

	}


	for (size_t fc = 0; fc < faces.size(); fc++)
	{
		for (size_t idx = 0; idx < 3; idx++)
		{
			size_t vi = faces[fc].vertexIndices[idx];
			size_t ni = faces[fc].normalIndices[idx];
			size_t ti = faces[fc].texCoordIndices[idx];

			if (vi == 0 || vi > vertices.size()) continue;
			if (ni == 0 || ni > normals.size()) continue;
			if (ti == 0 || ti > texCoords.size()) continue;

			Vertex vert;
			vert.position = vertices[vi - 1];
			vert.normal = normals[ni - 1];
			vert.texCoord = texCoords[ti - 1];
			mesh.vertices.emplace_back(vert);
		}
	}

	mesh.indicesCount = mesh.vertices.size();


	delete[] buffer;

	file.close();

}
