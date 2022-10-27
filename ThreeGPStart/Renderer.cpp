#include "Renderer.h"
#include "Camera.h"
#include "ImageLoader.h"

Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	glDeleteProgram(cube_Program);
	//for (int i = 0; i < m_modelVector.size(); i++)
	//{
	//	glDeleteBuffers(1, &m_modelVector[i].m_meshVector[i].VAO);
	//}
}

// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImGui::Begin("3GP");						// Create a window called "3GP" and append into it.

	ImGui::Text("Visibility.");					// Display some text (you can use a format strings too)	

	ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
	ImGui::End();
}

// Load, compile and link the shaders and create a program object to host them
GLuint Renderer::CreateProgram(std::string fragmentpath, std::string vertexpath)
{
	// Create a new program (returns a unqiue id)
	GLuint program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, vertexpath) };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, fragmentpath) };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(program, vertex_shader);

	// The attibute location 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(program, fragment_shader);

	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(program))
		return 0;

	return program;
}

float Renderer::Noise(int x, int y)
{
	int n = x + y * 57;
	n = (n >> 13) ^ n;
	int nn = (n * (n * n * 60493 + 19990303) + 1376312589) & 0x7fffffff;
	return 1.0f - ((float)nn / 1073741924.0f);
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	// Load and compile shaders into m_program
	m_program = CreateProgram("Data/Shaders/fragment_shader.frag", "Data/Shaders/vertex_shader.vert");


	//// Load and compile shaders into m_program
	cube_Program = CreateProgram("Data/Shaders/cubeFrag_shader.frag", "Data/Shaders/cubeVert_shader.vert");

	Mesh cubeMesh;
	std::vector<glm::vec3> verts =
	{
		//Front Face
		{-10, -10, 10},
		{10, -10, 10},
		{-10, 10, 10},
		{10, 10, 10},

		//Back Face
		{-10, -10, -10},
		{10, -10, -10},
		{-10, 10, -10},
		{10, 10, -10},

		//Left Face
		{-10, -10, -10},
		{-10, -10, 10},
		{-10, 10, 10},
		{-10, 10, -10},

		//Right Face
		{10, -10, 10},
		{10, 10, 10},
		{10, -10, -10},
		{10, 10, -10},

		//Top Face
		{-10, -10, 10},
		{10, -10, 10},
		{-10, -10, -10},
		{10, -10, -10},

		//Bottom Face
		{-10, 10, 10},
		{10, 10, 10},
		{-10, 10, -10},
		{10, 10, -10}
	};

	std::vector<glm::vec3> colors=
	{
		//Red
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},
		{1.0f, 0.0f, 0.0f},

		//Blue
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f},

		//White
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},

		//Orange
		{1.0f, 0.5f, 0.0f},
		{1.0f, 0.5f, 0.0f},
		{1.0f, 0.5f, 0.0f},
		{1.0f, 0.5f, 0.0f},

		//Yellow
		{1.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},
		{1.0f, 1.0f, 0.0f},

		//Green
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f}
	};

	std::vector<GLuint> Elements = 
	{
		//Front Face
		3,2,1,2,0,1,//0-3

		//Back Face
		6,7,4,7,5,4,//4-7

		//Left Face
		10,11,9,11,8,9,//8-11

		//Right Face
		15,13,14,13,12,14,//12-15

		//Top Face
		17,16,19,16,18,19,//16-19

		//Bottom Face
		23,22,21,22,20,21//20-23
	};

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* verts.size(), verts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint elementsEBO;
	glGenBuffers(1, &elementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* Elements.size(), Elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	cubeMesh.m_numElements = Elements.size();
	
	GLuint ColoursVBO;
	glGenBuffers(1, &ColoursVBO);
	glBindBuffer(GL_ARRAY_BUFFER, ColoursVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* colors.size(), colors.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &cubeMesh.VAO);
	glBindVertexArray(cubeMesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, ColoursVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsEBO);
	glBindVertexArray(0);

	cubemodel.m_meshVector.emplace_back(cubeMesh);



	// Helpers has an object for loading 3D geometry, supports most types
	// Load in the jeep
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))
	return false;

	// Now we can loop through all the mesh in the loaded model:
	for (const Helpers::Mesh& mesh : loader.GetMeshVector())
	{
		Mesh newMesh;

		GLuint meshVBO;
		glGenBuffers(1, &meshVBO);
		glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.vertices.size(), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh.normals.size(), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint meshElementsEBO;
		glGenBuffers(1, &meshElementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshElementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		GLuint texcoordsVBO;
		glGenBuffers(1, &texcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * mesh.uvCoords.size(), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		newMesh.m_numElements = mesh.elements.size();

		glGenVertexArrays(1, &newMesh.VAO);
		glBindVertexArray(newMesh.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, meshVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, texcoordsVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshElementsEBO);
		glBindVertexArray(0);

		Helpers::ImageLoader texture;
		if (texture.Load("Data\\Models\\Jeep\\jeep_rood.jpg"))
		{
			glGenTextures(1, &newMesh.Tex);
			glBindTexture(GL_TEXTURE_2D, newMesh.Tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.Width(), texture.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texture.GetData());
			glGenerateMipmap(GL_TEXTURE_2D);
		}

		jeepmodel.m_meshVector.emplace_back(newMesh);
	}

	// Terrain
	std::vector<glm::vec3> terVerts;
	std::vector<glm::vec3> terNormals;
	std::vector<glm::vec2> terTexture;
	std::vector<GLuint> terElements;

	int mNumVertsX = 50;
	int mNumVertsZ = 50;

	bool noise_on = true;

	for (float i = 0; i < mNumVertsZ; i++)
	{
		for (int j = 0; j < mNumVertsX; j++)
		{
			terVerts.push_back(glm::vec3(i * 100 - 2000, 0, j * 150 - 2000));
			terNormals.push_back({ 0,1,0 });
			terTexture.push_back({ ((float)i / mNumVertsZ) * 40, ((float)j / mNumVertsX) * 40 });
		}
	}

	if (noise_on == true)
	{
		int index = 0;
		for (int i = 0; i < mNumVertsZ; i++)
		{
			for (int j = 0; j < mNumVertsX; j++)
			{
				float noiseValue = Noise(i, j);
				noiseValue = noiseValue + 1.00001 / 2;
				glm::vec3 vec = terVerts[index];

				noiseValue = noiseValue * 50;

				vec.y = vec.y + noiseValue;
				terVerts[index] = vec;
				index++;
			}
		}
	}


	bool toggleDiamond = true;

	for (int cellZ = 0; cellZ < (mNumVertsZ - 1); cellZ++)
	{
		for (int cellX = 0; cellX < (mNumVertsX - 1); cellX++)
		{
			int startVert = (cellZ * mNumVertsX) + cellX;

			if (toggleDiamond == true)
			{
				//First triangle
				terElements.push_back(startVert);
				terElements.push_back(startVert + 1);
				terElements.push_back(startVert + mNumVertsX);

				//Second triangle
				terElements.push_back(startVert + 1);
				terElements.push_back(startVert + mNumVertsX + 1);
				terElements.push_back(startVert + mNumVertsX);
				toggleDiamond = false;
			}
			else 
			{
				terElements.push_back(startVert);
				terElements.push_back(startVert + 1);
				terElements.push_back(startVert + mNumVertsX + 1);

				terElements.push_back(startVert);
				terElements.push_back(startVert + mNumVertsX + 1);
				terElements.push_back(startVert + mNumVertsX);
				toggleDiamond = true;
			}
		}
	}

	Mesh terrainMesh;

	GLuint terVertsVBO;
	glGenBuffers(1, &terVertsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terVertsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* terVerts.size(), terVerts.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terNormalsVBO;
	glGenBuffers(1, &terNormalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terNormalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* terNormals.size(), terNormals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terTextureVBO;
	glGenBuffers(1, &terTextureVBO);
	glBindBuffer(GL_ARRAY_BUFFER, terTextureVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* terTexture.size(), terTexture.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint terElementsEBO;
	glGenBuffers(1, &terElementsEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terElementsEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* terElements.size(), terElements.data(), GL_STATIC_DRAW);//Broken
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	terrainMesh.m_numElements = terElements.size();

	glGenVertexArrays(1, &terrainMesh.VAO);
	glBindVertexArray(terrainMesh.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, terVertsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, terNormalsVBO);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, terTextureVBO);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terElementsEBO);
	glBindVertexArray(0);

	Helpers::ImageLoader terrain_texture;
	if (terrain_texture.Load("Data\\Textures\\grass11.bmp"))
	{
		glGenTextures(1, &terrainMesh.Tex);
		glBindTexture(GL_TEXTURE_2D, terrainMesh.Tex);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, terrain_texture.Width(), terrain_texture.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, terrain_texture.GetData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	terrainmodel.m_meshVector.emplace_back(terrainMesh);

	Helpers::ModelLoader loader2;
	if (!loader2.LoadFromFile("Data\\Models\\Sky\\Hills\\skybox.x"))
		return false;


	for (const Helpers::Mesh& mesh2 : loader2.GetMeshVector())
	{
		Mesh newMesh;
		
		GLuint skyvertsVBO;
		glGenBuffers(1, &skyvertsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyvertsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* mesh2.vertices.size(), mesh2.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint skynormalsVBO;
		glGenBuffers(1, &skynormalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skynormalsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* mesh2.normals.size(), mesh2.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint skyboxElementsEBO;
		glGenBuffers(1, &skyboxElementsEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxElementsEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)* mesh2.elements.size(), mesh2.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		GLuint skyboxtexcoordsVBO;
		glGenBuffers(1, &skyboxtexcoordsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, skyboxtexcoordsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)* mesh2.uvCoords.size(), mesh2.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		glGenVertexArrays(1, &newMesh.VAO);
		glBindVertexArray(newMesh.VAO);

		glBindBuffer(GL_ARRAY_BUFFER, skyvertsVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, skynormalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ARRAY_BUFFER, skyboxtexcoordsVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxElementsEBO);
		glBindVertexArray(0);

		newMesh.m_numElements = mesh2.elements.size();
		Skymodel.m_meshVector.emplace_back(newMesh);
	}

	m_modelVector.emplace_back(Skymodel);

	std::string facesCubemap[6] =
	{
		"Data\\Models\\Sky\\Hills\\skybox_top.jpg",
		"Data\\Models\\Sky\\Hills\\skybox_right.jpg",
		"Data\\Models\\Sky\\Hills\\skybox_left.jpg",
		"Data\\Models\\Sky\\Hills\\skybox_front.jpg",
		"Data\\Models\\Sky\\Hills\\skybox_back.jpg",
		"Data\\Models\\Sky\\Hills\\skybox_bottom.jpg"

	};

	for (int i = 0; i < Skymodel.m_meshVector.size(); i++)
	{
		Helpers::ImageLoader skyBox_texture;
		if (skyBox_texture.Load(facesCubemap[i]))
		{
			glGenTextures(1, &Skymodel.m_meshVector[i].Tex);
			glBindTexture(GL_TEXTURE_2D, Skymodel.m_meshVector[i].Tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, skyBox_texture.Width(), skyBox_texture.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, skyBox_texture.GetData());
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
}

// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{			
	// Configure pipeline settings
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
//	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.1f, 10000.0f);

	glUseProgram(m_program);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glm::mat4 model_xform = glm::mat4(1);
	// Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());


	//Skybox Rendering
	glm::mat4 view_xform2 = glm::mat4(glm::mat3(view_xform));
	glm::mat4 combined_xform2 = projection_xform * view_xform2;
	GLuint combined_xform_id2 = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id2, 1, GL_FALSE, glm::value_ptr(combined_xform2));
	for (int i = 0;i < Skymodel.m_meshVector.size(); i++)
	{		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Skymodel.m_meshVector[i].Tex);
		glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);
		glBindVertexArray(Skymodel.m_meshVector[i].VAO);
		glDrawElements(GL_TRIANGLES, Skymodel.m_meshVector[i].m_numElements, GL_UNSIGNED_INT, (void*)0);
	}
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	//Jeep Rendering
	glm::mat4 combined_xform = projection_xform * view_xform;
	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jeepmodel.m_meshVector[0].Tex);
	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);
	glBindVertexArray(jeepmodel.m_meshVector[0].VAO);
	glDrawElements(GL_TRIANGLES, jeepmodel.m_meshVector[0].m_numElements, GL_UNSIGNED_INT, (void*)0);

	//Terrain Rendering
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainmodel.m_meshVector[0].Tex);
	glBindVertexArray(terrainmodel.m_meshVector[0].VAO);
	glDrawElements(GL_TRIANGLES, terrainmodel.m_meshVector[0].m_numElements, GL_UNSIGNED_INT, (void*)0);

	//Cube Rendering
	glUseProgram(cube_Program);
	glm::mat4 transMatrix = glm::translate(glm::mat4(1), glm::vec3(0, 500, 0));
	glm::mat4 scaleMatrix = glm::scale(transMatrix, glm::vec3(10.0f,10.0f,10.0f));
	glm::mat4 model_xform2 = glm::mat4(1);
	static float angle = 0;
	static bool rotateY = true;
	if (rotateY) // Rotate around y axis		
		model_xform2 = glm::rotate(scaleMatrix, angle, glm::vec3{ 0 ,1,0 });
	else // Rotate around x axis		
		model_xform2 = glm::rotate(scaleMatrix, angle, glm::vec3{ 1 ,0,0 });
	angle+=0.001f;
	if (angle > glm::two_pi<float>())
	{
		angle = 0;
		rotateY = !rotateY;
	}
	glm::mat4 combined_xform3 = projection_xform * view_xform;
	GLuint combined_xform_id3 = glGetUniformLocation(cube_Program, "combined_xform3");
	glUniformMatrix4fv(combined_xform_id3, 1, GL_FALSE, glm::value_ptr(combined_xform3));
	GLuint model_xform_id2 = glGetUniformLocation(cube_Program, "model_xform2");
	glUniformMatrix4fv(model_xform_id2, 1, GL_FALSE, glm::value_ptr(model_xform2));
	glBindVertexArray(cubemodel.m_meshVector[0].VAO);
	glDrawElements(GL_TRIANGLES, cubemodel.m_meshVector[0].m_numElements, GL_UNSIGNED_INT, (void*)0);

}