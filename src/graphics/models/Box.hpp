#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "../Shader.h"

#define UPPER_BOUND 100

class Box
{
public:
	std::vector<glm::vec3> offsets;
	std::vector<glm::vec3> sizes;

	void init()
	{
		vertices =
		{
			// position             x   y   z   i
			 0.5f,  0.5f,  0.5f, // +   +   +   0
			-0.5f,  0.5f,  0.5f, // -   +   +   1
			-0.5f, -0.5f,  0.5f, // -   -   +   2
			 0.5f, -0.5f,  0.5f, // +   -   +   3
			 0.5f,  0.5f, -0.5f, // +   +   -   4
			-0.5f,  0.5f, -0.5f, // -   +   -   5
			-0.5f, -0.5f, -0.5f, // -   -   -   6
			 0.5f, -0.5f, -0.5f  // +   -   -   7
		};

		indices = 
		{
			// 12 lines
			// front face (+ve z)
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			// back face (-ve z)
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			// right face (+ve x)
			0, 4,
			3, 7,
			// left face (-ve x)
			1, 5,
			2, 6
		};

		// generate VAO
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// generate vertices VBO
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// generate offset VBO
		glGenBuffers(1, &offsetVBO);
		glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// generate size VBO
		glGenBuffers(1, &sizeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// generate EBO
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set attribute pointer
		// vertex position
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		// offsets
		glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(1);

		// sizes
		glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
		glEnableVertexAttribArray(2);

		glVertexAttribDivisor(1, 1); // reset _1st_ attribute every _1_ instance
		glVertexAttribDivisor(2, 1); // reset _2nd_ attribute every _1_ instance

		glBindVertexArray(0);
	}

	void render(Shader shader)
	{
		shader.SetMat4("model", glm::mat4(1.0f));

		int size = std::min(UPPER_BOUND, (int)offsets.size()); // if more than 100 instances, only render 100

		// update data
		if (size != 0)
		{
			// if instances exists
			glBindBuffer(GL_ARRAY_BUFFER, offsetVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3 * sizeof(float), &offsets[0]);

			glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3 * sizeof(float), &sizes[0]);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// render instanced data
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0, size);
		glBindVertexArray(0);
	}

	void addInstance(BoundingRegion br, glm::vec3 pos, glm::vec3 size)
	{
		offsets.push_back(br.calculateCenter() * size + pos);

		sizes.push_back(br.calculateDimensions() * size);
	}

	void cleanup()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &offsetVBO);
		glDeleteBuffers(1, &sizeVBO);
	}

private:
	unsigned int VAO, VBO, EBO;
	unsigned int offsetVBO, sizeVBO;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};