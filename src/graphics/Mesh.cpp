#include "Mesh.h"

std::vector<Vertex> Vertex::getList(float* vertices, int noVertices)
{
	std::vector<Vertex> ret(noVertices);

	int stride = sizeof(Vertex) / sizeof(float);

	for (int i = 0; i < noVertices; i++)
	{
		// float s = i * stride; // Improvement for little bit performance
		ret[i].pos = glm::vec3(
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2]
		);

		ret[i].normal = glm::vec3(
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5]
		);

		ret[i].texCoord = glm::vec2(
			vertices[i * stride + 6],
			vertices[i * stride + 7]
		);
	}

	return ret;
}

Mesh::Mesh(BoundingRegion br, std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
	: br(br), vertices(vertices), indices(indices), textures(textures), noTex(false)
{
	setup();
}

Mesh::Mesh(BoundingRegion br, std::vector<Vertex> vertices, std::vector<unsigned int> indices, aiColor4D diffuse, aiColor4D specular)
	: br(br),vertices(vertices), indices(indices), diffuse(diffuse), specular(specular), noTex(true)
{
	setup();
}

void Mesh::render(Shader shader, glm::vec3 pos, glm::vec3 size, Box* box, bool doRender /* = true */)
{
	if (noTex)
	{
		// materials
		shader.Set4Float("material.diffuse", diffuse);
		shader.Set4Float("material.specular", specular);
		shader.SetInt("noTex", 1);
	}
	else
	{
		// textures
		unsigned int diffuseIdx = 0;
		unsigned int specularIdx = 0;

		for (unsigned int i = 0; i < textures.size(); i++)
		{
			// activate texture
			glActiveTexture(GL_TEXTURE0 + i);

			// retrieve texture info
			std::string name;
			switch (textures[i].type)
			{
			case aiTextureType_DIFFUSE:
				name = "diffuse" + std::to_string(diffuseIdx++);
				break;

			case aiTextureType_SPECULAR:
				name = "specular" + std::to_string(specularIdx++);
				break;

			default:
				break;
			}

			// set the shader value
			shader.SetInt(name, i);

			// bind texture
			textures[i].bind();
		}
	}

	if (doRender)
	{
		box->addInstance(br, pos, size);

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		// reset
		glActiveTexture(GL_TEXTURE0);
	}
}

void Mesh::cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::setup()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set vertex attribute pointer
	// vertex.position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0); // alternative for @param pointer use offsetof(Vertex, pos) function

	// vertex.normal
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float))); // alternative for @param pointer use offsetof(Vertex, normal) function

	// vertex.texCoord
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float))); // alternative for @param pointer use offsetof(Vertex, texCoord) function

	glBindVertexArray(0);
}
