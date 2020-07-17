#include "TextureArray.h"
#include "Player.h"
#include "Chunk.h"
#include "ChunkManager.h"
#include "VertexBuffer.h"
#include "Globals.h"
#include "VertexArray.h"
#include "Rectangle.h"
#include "SkyBox.h"
#include "glm/gtc/noise.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderHandler.h"
#include "Frustum.h"
#include "Gui.h"
#include "DestroyBlockVisual.h"
#include "SelectedVoxelVisual.h"
#include "FrameBuffer.h"
#include "PickupManager.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <memory>
#include <unordered_map>
#include <thread>

#include "BoundingBox.h"

#ifdef DEBUG_GL_ERRORS
#define glCheck(expr) expr; if(!GLAD_GL_KHR_debug){ glCheckError(__FILE__, __LINE__, #expr) };
#else
#define glCheck(call) call
#endif
#define DEBUG_GL_ERRORS

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

//Good OpenGL Tutorials
//https://ahbejarano.gitbook.io/lwjglgamedev/chapter12

//x + (y * width)
int main()
{
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 8;
	settings.antialiasingLevel = 4;
	settings.majorVersion = 3;
	settings.minorVersion = 3;
	settings.attributeFlags = sf::ContextSettings::Core;
	glm::uvec2 windowSize(1920, 1080);
	sf::Window window(sf::VideoMode::getDesktopMode(), "Minecraft Clone", sf::Style::Fullscreen, settings);
	window.setFramerateLimit(60);
	window.setMouseCursorVisible(false);
	window.setMouseCursorGrabbed(true);
	gladLoadGL();

	glCheck(glViewport(0, 0, windowSize.x, windowSize.y));
	glEnable(GL_DEPTH_TEST);

	std::unique_ptr<ShaderHandler> shaderHandler = ShaderHandler::create(windowSize);
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

	std::unique_ptr<Texture> widjetsTexture = Texture::create("widgets.png");
	assert(widjetsTexture);
	if (!widjetsTexture)
	{
		std::cout << "Couldn't load widjets texture\n";
		return -1;
	}

	std::unique_ptr<Texture> fontTexture = Texture::create("ExportedFont.bmp");
	assert(fontTexture);
	if (!fontTexture)
	{
		std::cout << "Couldn't load font texture\n";
		return -1;
	}

	std::unique_ptr<Texture> destroyBlockTexture = Texture::create("blockDestroy.png");
	assert(destroyBlockTexture);
	if (!destroyBlockTexture)
	{
		std::cout << "Couldn't load blockDestroy texture\n";
		return -1;
	}

	std::unique_ptr<Texture> voxelSelectionTexture = Texture::create("voxelSelection.png");
	assert(voxelSelectionTexture);
	if (!voxelSelectionTexture)
	{
		std::cout << "Couldn't load voxel selection texture\n"; 
		return -1;
	}

	std::unique_ptr<ChunkManager> chunkManager = std::make_unique<ChunkManager>();
	PickupManager pickupManager;
	FrameBuffer frameBuffer(windowSize);
	SelectedVoxelVisual selectedVoxelVisual;
	DestroyBlockVisual destroyBlockVisual;
	Gui gui(windowSize);
	std::vector<Pickup> pickUps;
	Frustum frustum;
	Player player;
	std::atomic<bool> resetGame = false;
	std::mutex renderingMutex;
	std::mutex playerMutex;

	std::thread chunkGenerationThread([&](std::unique_ptr<ChunkManager>* chunkGenerator)
		{chunkGenerator->get()->update(std::ref(player), std::ref(window), std::ref(resetGame), 
			std::ref(playerMutex), std::ref(renderingMutex)); }, &chunkManager );

	player.spawn(*chunkManager, playerMutex);

	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";
	std::cout << glGetError() << "\n";

	std::cout << "\n\n";

	float deltaTime = 0.0f;
	sf::Clock deltaClock;
	deltaClock.restart();

	while (window.isOpen())
	{
		deltaTime = deltaClock.restart().asSeconds();

		//Handle Inputs
		sf::Event currentSFMLEvent;
		while (window.pollEvent(currentSFMLEvent))
		{
			if (currentSFMLEvent.type == sf::Event::Closed)
			{
				window.close();
			}
			if (currentSFMLEvent.type == sf::Event::KeyPressed)
			{
				switch (currentSFMLEvent.key.code)
				{
				case sf::Keyboard::R:
					resetGame = true;
					chunkGenerationThread.join();
					resetGame = false;
					chunkManager.reset();
					chunkManager = std::make_unique<ChunkManager>();

					chunkGenerationThread = std::thread{ [&](std::unique_ptr<ChunkManager>* chunkManager)
						{chunkManager->get()->update(std::ref(player), std::ref(window), std::ref(resetGame),
							std::ref(playerMutex), std::ref(renderingMutex)); }, &chunkManager };

					player.spawn(*chunkManager, playerMutex);
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
			}
			if (currentSFMLEvent.type == sf::Event::MouseButtonReleased)
			{
				player.resetDestroyCubeTimer();
				destroyBlockVisual.reset();
			}

			player.handleInputEvents(pickUps, currentSFMLEvent, *chunkManager, playerMutex, window, gui, selectedVoxelVisual);
		}

		//Update
		player.update(deltaTime, playerMutex, *chunkManager.get(), destroyBlockVisual, pickUps, gui);
		destroyBlockVisual.update(deltaTime);
		pickupManager.update(deltaTime, player, playerMutex, *chunkManager);

		glm::mat4 view = glm::lookAt(player.getPosition(), player.getPosition() + player.getCamera().front, player.getCamera().up);
		glm::mat4 projection = glm::perspective(glm::radians(player.getCamera().FOV),
			static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y), player.getCamera().nearPlaneDistance, player.getCamera().farPlaneDistance);

		frustum.update(projection * view);
		
		if (player.isUnderWater(*chunkManager, playerMutex))
		{
			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);

			std::lock_guard<std::mutex> renderingLock(renderingMutex);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer.getID());
			glEnable(GL_DEPTH_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			textureArray->bind();
			chunkManager->renderOpaque(frustum);

			pickupManager.render(frustum, *shaderHandler, view, projection);

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
			chunkManager->renderTransparent(frustum);

			destroyBlockTexture->bind();
			shaderHandler->switchToShader(eShaderType::DestroyBlock);
			shaderHandler->setUniformMat4f(eShaderType::DestroyBlock, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::DestroyBlock, "uProjection", projection);
			destroyBlockVisual.render();

			if (!player.getDestroyCubeTimer().isActive())
			{
				voxelSelectionTexture->bind();
				shaderHandler->switchToShader(eShaderType::SelectedVoxel);
				shaderHandler->setUniformMat4f(eShaderType::SelectedVoxel, "uView", view);
				shaderHandler->setUniformMat4f(eShaderType::SelectedVoxel, "uProjection", projection);
				selectedVoxelVisual.render();
			}


			glDisable(GL_BLEND);

			//Draw Skybox
			glDepthFunc(GL_LEQUAL);
			shaderHandler->switchToShader(eShaderType::Skybox);
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uView", glm::mat4(glm::mat3(view)));
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uProjection", projection);

			skybox->render();
			glDepthFunc(GL_LESS);

			// now bind back to default framebuffer and draw a quad plane with the attached framebuffer color texture
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
			// clear all relevant buffers
			glClear(GL_COLOR_BUFFER_BIT);

			shaderHandler->switchToShader(eShaderType::PostProcessing);

			glBindVertexArray(frameBuffer.getVAOID());
			glBindTexture(GL_TEXTURE_2D, frameBuffer.getTextureID());	// use the color attachment texture as the texture of the quad plane
			glDrawArrays(GL_TRIANGLES, 0, 6);

			//Draw GUI
			textureArray->bind();
			gui.render(*shaderHandler, *widjetsTexture, *fontTexture);
		}
		else
		{
			//Render
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);

			std::lock_guard<std::mutex> renderingLock(renderingMutex);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

			textureArray->bind();
			chunkManager->renderOpaque(frustum);

			pickupManager.render(frustum, *shaderHandler, view, projection);

			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shaderHandler->switchToShader(eShaderType::Chunk);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::Chunk, "uProjection", projection);
			chunkManager->renderTransparent(frustum);

			destroyBlockTexture->bind();
			shaderHandler->switchToShader(eShaderType::DestroyBlock);
			shaderHandler->setUniformMat4f(eShaderType::DestroyBlock, "uView", view);
			shaderHandler->setUniformMat4f(eShaderType::DestroyBlock, "uProjection", projection);
			destroyBlockVisual.render();

			if (!player.getDestroyCubeTimer().isActive())
			{
				voxelSelectionTexture->bind();
				shaderHandler->switchToShader(eShaderType::SelectedVoxel);
				shaderHandler->setUniformMat4f(eShaderType::SelectedVoxel, "uView", view);
				shaderHandler->setUniformMat4f(eShaderType::SelectedVoxel, "uProjection", projection);
				selectedVoxelVisual.render();
			}

			glDisable(GL_BLEND);

			//Draw Skybox
			glDepthFunc(GL_LEQUAL);
			shaderHandler->switchToShader(eShaderType::Skybox);
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uView", glm::mat4(glm::mat3(view)));
			shaderHandler->setUniformMat4f(eShaderType::Skybox, "uProjection", projection);

			skybox->render();
			glDepthFunc(GL_LESS);

			//Draw GUI
			textureArray->bind();
			gui.render(*shaderHandler, *widjetsTexture, *fontTexture);
		}

		window.display();
	}

	chunkGenerationThread.join();
	return 0;
}