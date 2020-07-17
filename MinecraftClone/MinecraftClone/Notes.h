#pragma once

//What I did
//Impact it had
//How I can add value - Value I bring
//How to simplify, integrate with co workers
//Perforce - Industry standard

//Impact statements - emphasise what I did - how I contributed
//What feature would I add to minecraft?
//Internships/Placements 

//Another MC clone
//https://www.youtube.com/watch?v=65_LIdnd6Ek
//Math
//https://www.reddit.com/r/gamedev/comments/hh98gh/what_kind_of_math_do_i_need_to_learn/
//https://www.reddit.com/r/gamedev/comments/gqndy3/what_types_of_math_would_you_say_are_essential_to/

////Example Project
//https://www.youtube.com/watch?v=MfaXf6Gs8oY
//
//https://github.com/rhysboer/VoxitCraft/tree/master/Minecraft
//
//
////Lighting
//https://www.seedofandromeda.com/blogs/29-fast-flood-fill-lighting-in-a-blocky-voxel-game-pt-1
//https://www.seedofandromeda.com/blogs/30-fast-flood-fill-lighting-in-a-blocky-voxel-game-pt-2
//https://github.com/Unarelith/OpenMiner/issues/21
//
////Gui
//well, when you code your UI you'll define a projection matrix with the size of the screen, and render using that matrix (gluOrtho2D for old opengl and glm::ortho for modern opengl)
//so you can pick a small size, (mine is 1920 / 3; 1080 / 3) and upscale it to the actual screen
//for the other stuff, definining UI element position relative to screen sizeand screen scaling is a good idea most of the time
//hmm, I highly suggest that you create an Image class instead of doing things like getToolbarTextCoords()
//https ://github.com/Unarelith/GameKit/blob/master/include/gk/graphics/Image.hpp
//https://github.com/Unarelith/GameKit/blob/master/source/graphics/Image.cpp
//something like this

//Dyanmic Chunk Generation
//https://gamedev.stackexchange.com/questions/173820/how-should-i-store-loaded-world-chunks-in-ram-for-my-game-similar-to-minecraft
//https://gamedev.stackexchange.com/questions/37911/how-to-deal-with-large-open-worlds

//https://www.reddit.com/r/VoxelGameDev/comments/4xsy0d/resources_for_terrain_generation_using/
//http://accidentalnoise.sourceforge.net/minecraftworlds.html
//https://learninggeekblog.wordpress.com/2013/04/25/voxel-engine-part-2/

//https://www.reddit.com/r/VoxelGameDev/comments/btt0hz/octrees_and_chunks/

//https://www.reddit.com/r/proceduralgeneration/comments/2gy05g/creating_biomes_in_an_infinite_world_like_in/
//https://www.seedofandromeda.com/blogs/1-creating-a-region-file-system-for-a-voxel-game
//https://www.reddit.com/r/VoxelGameDev/comments/95hqrb/how_to_get_started_with_terrain_generation/

//https://algs4.cs.princeton.edu/34hash/
//https://www.hackerearth.com/practice/data-structures/hash-tables/basics-of-hash-tables/tutorial

//https://www.reddit.com/r/proceduralgeneration/comments/4eixfr/how_are_randomly_placed_structures_generated_in_a/

//https://github.com/gametutorials/tutorials/tree/master/OpenGL/Frustum%20Culling
//http://www.lighthouse3d.com/tutorials/view-frustum-culling/
//https://github.com/BSVino/MathForGameDevelopers/tree/frustum-culling

//https://github.com/Hopson97/MineCraft-One-Week-Challenge/blob/master/Source/Maths/Frustum.h
//https://github.com/Hopson97/MineCraft-One-Week-Challenge/blob/master/Source/Maths/Frustum.cpp

//http://www.lighthouse3d.com/tutorials/maths/


//Trees
//https://www.reddit.com/r/proceduralgeneration/comments/72waky/how_does_layered_generation_work_when_generating/
//https://www.reddit.com/r/proceduralgeneration/comments/2t2dyy/trees_in_minecraft_like_worlds_help/

//Saving/Storing
//https://www.reddit.com/r/proceduralgeneration/comments/3gwbux/question_how_does_the_world_remember_changes/




//Scale
//Octaves
///Lacunraity
//Persistance
//https://www.reddit.com/r/proceduralgeneration/comments/byju4s/minecraft_style_terrain_gen_question_how_to/
//https://www.reddit.com/r/proceduralgeneration/comments/dkdfq0/different_generation_for_biomes/

//https://rtouti.github.io/graphics/perlin-noise-algorithm
//http://www.6by9.net/simplex-noise-for-c-and-python/
//https://medium.com/@yvanscher/playing-with-perlin-noise-generating-realistic-archipelagos-b59f004d8401

//Lacunarity = Controls the increase in frequency in octaves
//Persistence = Controls decrease in amplitude of octaves

//Amplitude - 'y' Axis
//Frequency - 'x' Axis

//https://www.reddit.com/r/proceduralgeneration/comments/4i9a08/terrain_generation_of_a_game_i_am_working_on/
//http://pcgbook.com/wp-content/uploads/chapter04.pdf
//https://www.reddit.com/r/proceduralgeneration/comments/drc96v/getting_started_in_proceduralgeneration/
//https://notch.tumblr.com/post/3746989361/terrain-generation-part-1

//https://www.reddit.com/r/proceduralgeneration/comments/4i9a08/terrain_generation_of_a_game_i_am_working_on/
//http://pcgbook.com/wp-content/uploads/chapter04.pdf
//https://www.reddit.com/r/proceduralgeneration/comments/drc96v/getting_started_in_proceduralgeneration/
//https://notch.tumblr.com/post/3746989361/terrain-generation-part-1

//https://www.reddit.com/r/VoxelGameDev/comments/c0fcsi/giving_terrain_oomph_ive_been_trying_to_go_for/

//http://accidentalnoise.sourceforge.net/minecraftworlds.html