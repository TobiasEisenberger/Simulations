#include <GLAD/glad.h>
#include <GLFW/glfw3.h>

#include <GLM/fwd.hpp>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <stb/stb_image.h>

#include <iostream>
#include <string>

#include "Shader.h"
#include "Camera.h"
#include "fire/ParticleSystem.h";

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);

int g_width{ 800 };
int g_height{ 600 };

float lastX = 400.0f;
float lastY = 300.0f;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

Camera camera(glm::vec3(0.0f, 0.0f, 15.0f));

int main()
{
	std::cout << "Hello World!\n";

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(g_width, g_height, "Simulation", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window!\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD!\n";
		return -1;
	}

	glViewport(0, 0, g_width, g_height);
	glfwSwapInterval(1);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA);
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << "\n";

	unsigned int vertexArrayObjectId;
	glGenVertexArrays(1, &vertexArrayObjectId);
	glBindVertexArray(vertexArrayObjectId);
	
	ParticleSystem particleSystem(1);

	unsigned int vertexBufferObjectId;
	glGenBuffers(1, &vertexBufferObjectId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjectId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Particle) * particleSystem.GetParticles().size(), particleSystem.GetParticles().data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(sizeof(float) * 6));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(sizeof(float) * 7));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (const void*)(sizeof(float) * 8));
	glEnableVertexAttribArray(4);

	Shader shader("src/fire/shader/vertex.vert", "src/fire/shader/geometry.geom", "src/fire/shader/fragment.frag");
	shader.use();
	shader.setMat4f("model", glm::value_ptr(glm::mat4(1.0f)));

	unsigned int fireTexture = loadTexture("src/fire/textures/fire.png");
	shader.setInt("particleTexture", 0);

	unsigned int transparencyFramebuffer;
	glGenFramebuffers(1, &transparencyFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, transparencyFramebuffer);
	glEnable(GL_BLEND);
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);

	unsigned int accumColorBuffer;
	glGenTextures(1, &accumColorBuffer);
	glBindTexture(GL_TEXTURE_2D, accumColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_width, g_height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumColorBuffer, 0);

	unsigned int revealColorBuffer;
	glGenTextures(1, &revealColorBuffer);
	glBindTexture(GL_TEXTURE_2D, revealColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, g_width, g_height, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, revealColorBuffer, 0);

	const unsigned int transparentDrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, transparentDrawBuffers);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Transparent framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, accumColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, revealColorBuffer);

	Shader transparentShader("src/fire/shader/transparent.vert", nullptr, "src/fire/shader/transparent.frag");

	unsigned int vaoPlaneId;
	glGenVertexArrays(1, &vaoPlaneId);
	glBindVertexArray(vaoPlaneId);

	float planeVerts[] = {
		-0.5f, 0.5f, // top left
		-0.5f, -0.5f, // bot left
		0.5f, 0.5f, // top right
		0.5f, 0.5f, // top right
		-0.5f, -0.5f, // bot left
		0.5f, -0.5f, // bot right
	};

	unsigned int vboPlaneId;
	glGenBuffers(1, &vboPlaneId);
	glBindBuffer(GL_ARRAY_BUFFER, vboPlaneId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12, planeVerts, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);

	Shader planeShader("src/fire/shader/plane.vert", nullptr, "src/fire/shader/plane.frag");
	planeShader.use();
	planeShader.setInt("accum", 0);
	planeShader.setInt("reveal", 1);
	
	int pointCount = particleSystem.GetParticles().size();
	float elapsedTime = 0.0f;
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		elapsedTime += deltaTime;
		shader.setFloat("elapsedTime", elapsedTime);

		processInput(glfwGetCurrentContext());

		glm::mat4 viewMatrix = camera.GetViewMatrix();
		glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.Zoom), (float)g_width / g_height, 0.1f, 100.f);

		shader.setMat4f("view", glm::value_ptr(viewMatrix));
		shader.setMat4f("projection", glm::value_ptr(projectionMatrix));

		// Transparency pass
		glBindFramebuffer(GL_FRAMEBUFFER, transparencyFramebuffer);
		glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));
		glClearBufferfv(GL_COLOR, 1, glm::value_ptr(glm::vec4(1.0f)));
		transparentShader.use();
		transparentShader.setMat4f("view", glm::value_ptr(viewMatrix));
		transparentShader.setMat4f("projection", glm::value_ptr(projectionMatrix));

		glm::mat4 model = glm::mat4(1.0f);
		// Red plane
		transparentShader.setMat4f("model", glm::value_ptr(model));
		transparentShader.setFloat4("color", 1.0f, 0.0f, 0.0f, 0.75f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Yellow plane
		transparentShader.setMat4f("model", glm::value_ptr(glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f))));
		transparentShader.setFloat4("color", 1.0f, 1.0f, 0.0f, 0.75f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Blue plane
		transparentShader.setMat4f("model", glm::value_ptr(glm::translate(model, glm::vec3(0.0f, 0.0f, 2.0f))));
		transparentShader.setFloat4("color", 0.0f, 0.0f, 1.0f, 0.75f);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Composite pass
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		planeShader.use();
		model = glm::mat4(1.0f);
		planeShader.setMat4f("model", glm::value_ptr(glm::scale(model, glm::vec3(2.0f, 2.0f, 1.0f))));
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glDeleteVertexArrays(1, &vertexArrayObjectId);
	glDeleteBuffers(1, &vertexBufferObjectId);
	glDeleteProgram(shader.ID);

	glfwTerminate();

	return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	g_width = width;
	g_height = height;
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		int wrappingMethod = format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrappingMethod);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrappingMethod);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}