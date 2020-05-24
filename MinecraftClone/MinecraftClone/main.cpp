
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glad.h"

#include "TextureArray.h"
#include "Camera.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "VertexBuffer.h"
#include "Utilities.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "SkyBox.h"
#include "glm/gtc/noise.hpp"
#include "ShaderHandler.h"
#include "Frustum.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <unordered_map>
#include <thread>

#include <GLFW/glfw3.h>


//https://hacknplan.com/
//https://www.reddit.com/r/Minecraft/comments/2ikiaw/opensimplex_noise_for_terrain_generation_instead/

//Discourage use of VAOs all the time
//https://www.youtube.com/watch?v=Bcs56Mm-FJY&list=PLlrATfBNZ98foTJPJ_Ev03o2oq3-GGOS2&index=12

//Run Tests - benchmarks/profile
//1. One global VAO
//2. VAO for each gameobject

//OpenGL is a state machine
//Select buffer - state - then draw me a triangle
//Based off of which buffer has been selected, it determines what/how will be 

//https://www.reddit.com/r/VoxelGameDev/comments/376vmv/how_do_you_implement_threading_in_your_game/

//View == Frustrum
//The reason why it can get away with that is because of other rendering optimizations.
//Try refraining from rendering any blocks that are not adjacent to air(less bandwidth), 
//buildingand optimizing meshes from chunk data(less vertices to draw), 
//or cutting down on the size of each vertex element(less bandwidth).There are plenty of other ways to increase performance with such an engine.

//https://devtalk.nvidia.com/default/topic/720651/opengl/access-violation-in-nvoglv32-dll-how-do-i-track-down-the-problem-/
//https://community.khronos.org/t/how-do-you-implement-texture-arrays/75315

//http://ogldev.atspace.co.uk/index.html



//x + (y * width)
int main()
{
	if (!glfwInit())
	{
		std::cout << "GLFW Initialization Failed\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glm::ivec2 windowSize{ 1980, 1080 };
	GLFWwindow* window = glfwCreateWindow(windowSize.x, windowSize.y, "MINECRAFT CLONE", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to load window\n";
		glfwTerminate();
		return -1;
	}
	glfwWindowHint(GLFW_REFRESH_RATE, 60);
	glfwSwapInterval(1);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//glfwSetCursorPosCallback(window, mouse_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	

	glViewport(0, 0, windowSize.x, windowSize.y);
	glEnable(GL_DEPTH_TEST);

	std::unique_ptr<ShaderHandler> shaderHandler = ShaderHandler::create();
	assert(shaderHandler);
	if (!shaderHandler)
	{
		std::cout << "Unable to load shader handler\n";
		return -1;
	}

	std::unique_ptr<TextureArray> textureArray = TextureArray::create();
	assert(textureArray);
	if (!textureArray)
	{
		std::cout << "Failed to load textures\n";
		return -1;
	}
	textureArray->bind();

	std::unique_ptr<SkyBox> skybox = SkyBox::create();
	assert(skybox);
	if (!skybox)
	{
		std::cout << "Failed to load Skybox\n";
		return -1;
	}

	shaderHandler->setUniform1i(eShaderType::Skybox, "uSkyboxTexture", 0);
	shaderHandler->setUniform1i(eShaderType::Chunk, "uTexture", 0);

	std::atomic<bool> running = true;
	Frustum frustum;
	Camera camera(Utilities::PLAYER_STARTING_POSITION);
	glm::vec3 cameraPosition;
	cameraPosition = camera.m_position;
	bool movePlayer = false;
	std::atomic<bool> resetGame = false;
	std::mutex renderingMutex;
	std::mutex cameraMutex;
	//std::unique_ptr<ChunkManager> chunkManager = std::make_unique<ChunkManager>(camera.m_position);

	//std::thread chunkGenerationThread([&](std::unique_ptr<ChunkManager>* chunkGenerator)
	//	{chunkGenerator->get()->update(std::ref(cameraPosition), std::ref(running), std::ref(resetGame), 
	//		std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkManager );

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	//float deltaTime = 0.0f;
	//int frames = 0;
	//sf::Clock deltaClock;
	//sf::Clock gameClock;
	//float lastTime = gameClock.getElapsedTime().asSeconds();
	//deltaClock.restart();
	//float messageExpiredTime = 1.0f;
	//float elaspedTime = 0.0f;

	float previousFrameTime = glfwGetTime();
	
	//while (!glfwWindowShouldClose(window))
	//{

	//}
	while (!glfwWindowShouldClose(window))
	{

	}
	while (!glfwWindowShouldClose(window))
	{
		//float currentFrameTime = glfwGetTime();
		//float deltaTime = currentFrameTime - previousFrameTime;

		////deltaTime = deltaClock.restart().asSeconds();
		////++frames;

		////sf::Vector2i mousePosition = sf::Mouse::getPosition();

		////camera.mouse_callback(mousePosition.x, mousePosition.y);

		////sf::Event currentSFMLEvent;
		////while (window.pollEvent(currentSFMLEvent))
		////{
		////	if (currentSFMLEvent.type == sf::Event::Closed)
		////	{
		////		window.close();
		////	}
		////	else if (currentSFMLEvent.KeyPressed)
		////	{
		////		if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
		////		{
		////			resetGame = true;
		////		}

		////	}
		////}

		//if (resetGame)
		//{
		//	chunkGenerationThread.join();
		//	resetGame = false;
		//	camera.m_position = Utilities::PLAYER_STARTING_POSITION;
		//	cameraPosition = camera.m_position;
		//	chunkManager.reset();
		//	chunkManager = std::make_unique<ChunkManager>(cameraPosition);

		//	chunkGenerationThread = std::thread{ [&](std::unique_ptr<ChunkManager>* chunkManager)
		//		{chunkManager->get()->update(std::ref(cameraPosition), std::ref(running), std::ref(resetGame),
		//			std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkManager };
		//}

		//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
		//	glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
		//	glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
		//	glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//{
		//	camera.move(window, deltaTime);

		//	if (cameraMutex.try_lock())
		//	{
		//		cameraPosition = camera.m_position;
		//		cameraMutex.unlock();
		//	}
		//}

		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glm::mat4 view = glm::mat4(1.0f);
		//view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
		//glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		//	static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 1750.0f);

		//frustum.update(projection * view);

		//if (chunkManager)
		//{
		//	shaderHandler->switchToShader(eShaderType::Chunk);
		//	shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
		//	shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
		//	//Draw Scene
		//	std::lock_guard<std::mutex> renderingLock(renderingMutex);
		//	glEnable(GL_CULL_FACE);
		//	glCullFace(GL_BACK);
		//	chunkManager->renderOpaque(frustum);
		//	glDisable(GL_CULL_FACE);
		//	glEnable(GL_BLEND);
		//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//	chunkManager->renderTransparent(frustum);
		//	glDisable(GL_BLEND);

		//	//Draw Skybox
		//	glDepthFunc(GL_LEQUAL);
		//	shaderHandler->switchToShader(eShaderType::Skybox);
		//	shaderHandler->setUniformMat4f(eShaderType::Skybox, "uView", glm::mat4(glm::mat3(view)));
		//	shaderHandler->setUniformMat4f(eShaderType::Skybox, "uProjection", projection);

		//	skybox->render();
		//	glDepthFunc(GL_LESS);
		//}

		//previousFrameTime = currentFrameTime;
		//glfwSwapBuffers(window);
		//glfwPollEvents();

	}

	//Old
	//while (window-isOpen())
	//{
	//	deltaTime = deltaClock.restart().asSeconds();
	//	++frames;
	//	if (gameClock.getElapsedTime().asSeconds() - lastTime >= 1.0f)
	//	{
	//		//std::cout << 1000.0f / frames << "\n";
	//		frames = 0;
	//		lastTime += 1.0f;
	//	}

	//	sf::Vector2i mousePosition = sf::Mouse::getPosition();

	//	camera.mouse_callback(mousePosition.x, mousePosition.y);

	//	sf::Event currentSFMLEvent;
	//	while (window.pollEvent(currentSFMLEvent))
	//	{
	//		if (currentSFMLEvent.type == sf::Event::Closed)
	//		{
	//			window.close();
	//		}
	//		else if (currentSFMLEvent.KeyPressed)
	//		{
	//			if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
	//			{
	//				resetGame = true;
	//			}
	//		}
	//	}

	//	if (resetGame)
	//	{
	//		chunkGenerationThread.join();
	//		resetGame = false;
	//		camera.m_position = Utilities::PLAYER_STARTING_POSITION;
	//		cameraPosition = camera.m_position;
	//		chunkManager.reset();
	//		chunkManager = std::make_unique<ChunkManager>(cameraPosition);

	//		chunkGenerationThread = std::thread{ [&](std::unique_ptr<ChunkManager>* chunkManager)
	//			{chunkManager->get()->update(std::ref(cameraPosition), std::ref(window), std::ref(resetGame), 
	//				std::ref(cameraMutex), std::ref(renderingMutex)); }, &chunkManager };
	//	}

	//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
	//		glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
	//		glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
	//		glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	//	{
	//		camera.move(deltaTime);

	//		if (cameraMutex.try_lock())
	//		{
	//			cameraPosition = camera.m_position;
	//			cameraMutex.unlock();
	//		}
	//	}

	//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	glm::mat4 view = glm::mat4(1.0f);
	//	view = glm::lookAt(camera.m_position, camera.m_position + camera.m_front, camera.m_up);
	//	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
	//		static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), 0.1f, 1750.0f);

	//	frustum.update(projection * view);
	//	
	//	if (chunkManager)
	//	{
	//		shaderHandler->switchToShader(eShaderType::Chunk);
	//		shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
	//		shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
	//		//Draw Scene
	//		std::lock_guard<std::mutex> renderingLock(renderingMutex);
	//		glEnable(GL_CULL_FACE);
	//		glCullFace(GL_BACK);
	//		chunkManager->renderOpaque(frustum);
	//		glDisable(GL_CULL_FACE);
	//		glEnable(GL_BLEND);
	//		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//		chunkManager->renderTransparent(frustum);
	//		glDisable(GL_BLEND);

	//		//Draw Skybox
	//		glDepthFunc(GL_LEQUAL);
	//		shaderHandler->switchToShader(eShaderType::Skybox);
	//		shaderHandler->setUniformMat4f(eShaderType::Skybox, "uView", glm::mat4(glm::mat3(view)));
	//		shaderHandler->setUniformMat4f(eShaderType::Skybox, "uProjection", projection);

	//		skybox->render();
	//		glDepthFunc(GL_LESS);
	//	}

	//	window.display();
	//}

	//chunkGenerationThread.join();
	
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}