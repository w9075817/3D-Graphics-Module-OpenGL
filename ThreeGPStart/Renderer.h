#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

struct Mesh
{
	GLuint VAO;
	GLuint m_numElements;
	GLuint Tex;
};

struct Model
{
	std::vector<Mesh> m_meshVector;
};


class Renderer
{
private:
	Model Skymodel;
	Model jeepmodel;
	Model terrainmodel;
	Model cubemodel;
	std::vector<Model> m_modelVector;

	// Program object - to host shaders
	GLuint m_program{ 0 };
	GLuint cube_Program{ 0 };

	bool m_wireframe{ false };

	GLuint CreateProgram(std::string fragmentpath, std::string vertexpath);
public:
	Renderer();
	~Renderer();

	// Draw GUI
	void DefineGUI();

	float Noise(int x, int y);

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();

	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

