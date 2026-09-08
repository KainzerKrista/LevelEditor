#include "Assets/ObjectLoader.h"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

bool ObjectLoader::Load(const std::string& filePath, MeshData& meshData)
{
	std::ifstream file(filePath);

	if (!file.is_open())
	{
		std::cerr
			<< "ERROR: Failed to open .obj file: "
			<< filePath
			<< '\n';

		return false;
	}

	meshData.vertices.clear();
	meshData.indices.clear();
	
	std::vector<glm::vec3> positions;

	std::string line;

	while (std::getline(file, line))
	{
		std::istringstream lineStream(line);

		std::string type;

		lineStream >> type;

		// Vertex Position
		if (type == "v")
		{
			glm::vec3 position;

			lineStream
				>> position.x
				>> position.y
				>> position.z;

				positions.push_back(position);
		}

		// Faces
		else if (type == "f")
		{
			std::vector<std::uint32_t> faceIndices;
			std::string vertexToken;

			while (lineStream >> vertexToken)
			{
				const std::size_t slashPosition = vertexToken.find('/');
				const std::string positionIndexText = vertexToken.substr(0, slashPosition);
				const int objIndex = std::stoi(positionIndexText);

				// .obj indicies start at 1
				const std::uint32_t positionIndex = static_cast<std::uint32_t>(objIndex - 1);
				faceIndices.push_back(positionIndex);

				for (std::size_t i = 1; i + 1 < faceIndices.size(); ++i)
				{
					meshData.indices.push_back(faceIndices[0]);
					meshData.indices.push_back(faceIndices[i]);
					meshData.indices.push_back(faceIndices[i + 1]);
				}
			}
		}
	}

	// Convert .obj positions to vertex format

	meshData.vertices.reserve(positions.size());

	for (const glm::vec3& position : positions)
	{
		Vertex vertex{};

		vertex.position[0] = position.x;
		vertex.position[1] = position.y;
		vertex.position[2] = position.z;
		
		// Temp neutral color
		vertex.color[0] = 0.8f;
		vertex.color[1] = 0.8f;
		vertex.color[2] = 0.8f;
		vertex.color[3] = 1.0f;

		meshData.vertices.push_back(vertex);
	}

	if (meshData.vertices.empty() || meshData.indices.empty())
	{
		std::cerr
			<< "ERROR: .obj file contains no usable geometry: "
			<< filePath
			<< '\n';

		return false;
	}

	return true;
}