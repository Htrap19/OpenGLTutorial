#pragma once

#include "../Model.h"

#include "Box.hpp"

#define UPPER_BOUND 100

template <class T>
class ModelArray
{
public:
	std::vector<RigidBody> instances;

	void init()
	{
		model.init();

		// generate positions VBO
		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// generate size VBO
		glGenBuffers(1, &sizeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// set attribute pointers for each mesh
		for (unsigned int i = 0, size = model.meshes.size(); i < size; i++)
		{
			glBindVertexArray(model.meshes[i].VAO);
			// positions
			glBindBuffer(GL_ARRAY_BUFFER, posVBO);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(3);
			// sizes
			glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
			glEnableVertexAttribArray(4);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glVertexAttribDivisor(3, 1); // reset _3rd_ attribute every _1_ instance
			glVertexAttribDivisor(4, 1); // reset _4th_ attribute every _1_ instance

			glBindVertexArray(0);
		}
	}

	void render(Shader shader, float dt, Box* box, bool setLists = true)
	{
		if (setLists)
		{
			positions.clear();
			sizes.clear();

			for (RigidBody& rb : instances)
			{
				rb.update(dt);
				positions.push_back(rb.pos);
				sizes.push_back(model.size);
			}
		}

		shader.SetMat4("model", glm::mat4(1.0f));

		model.render(shader, dt, nullptr, false, false);

		int s = std::min(UPPER_BOUND, (int)positions.size()); // if more than 100 instances, only render 100

		// update data
		if (s != 0)
		{
			// if instances exists
			glBindBuffer(GL_ARRAY_BUFFER, posVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, s * 3 * sizeof(float), &positions[0]);

			glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, s * 3 * sizeof(float), &sizes[0]);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		// render instanced data
		for (unsigned int i = 0, length = model.meshes.size(); i < length; i++)
		{
			for (unsigned int j = 0; j < s; j++)
				box->addInstance(model.meshes[i].br, positions[j], sizes[j]);

			glBindVertexArray(model.meshes[i].VAO);
			glDrawElementsInstanced(GL_TRIANGLES, model.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, s);
			glBindVertexArray(0);
		}
	}

	void setSize(glm::vec3 size)
	{
		model.size = size;
	}

	void cleanup()
	{
		model.cleanup();
	}

protected:
	T model;

	unsigned int posVBO;
	unsigned int sizeVBO;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> sizes;
};
