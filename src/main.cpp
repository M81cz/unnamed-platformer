#include "engine.h"
#include "sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "objects.h"

void Update(float dt);
void RenderFrame(float dt);

//=============================================================================

#define WW 1200
#define WH 900
const int hiboxtolerance = 4;
const int terrainsize = 60;
int terraincount = 0;
int gravity = 15;
Entity player = {100, 100, 50, 50, 0, 0, 50, 50, 0, 0, 25, 350, 5, 500, 10, false};
Terrain terrain[100];

bool generate = true;

//=============================================================================

int main(int argc, char* argv[])
{
	if (!InitSDL())
	{
		return 1;
	}

	if (!CreateWindow("Jet-Climber", WW, WH))
	{
		return 1;
	}
	
	StartLoop(Update, RenderFrame);

	return 0;
}

//=============================================================================

void ElementCreation(char element, char x, char y) {
	//converts .txt file symbols into game elements
	switch (element)
	{
	case 'O':
		//initial spawn point

		break;
	case 'F':
		//level finish

		break;
	case 'S':
		//spawn point

		break;
	case '.':
		//air
		break;
	case 'N':
		//wall
		terrain[terraincount] = {terrainsize * x, terrainsize * y,terrainsize, terrainsize};
		terraincount++;
		break;
	case 'X':
		//spikes

		break;
	case 'E':
		//enemy

		break;
	default:
		//void
		break;
	}
}
void LevelGeneration(char file[])
{
	//generates level based on .txt file symbols (more in README.md and ElementCreation())
	char path[100] = "levels/";
	strcat(path, file);
	strcat(path, ".txt");
	FILE* f;
	f = fopen(path, "r");
	char c = fgetc(f);
	for (int x=0,y=0; c != EOF; x++)
	{
		if (c == '\n')
		{
			x = -1;
			y++;
		}
		else {
			ElementCreation(c, x, y);
		}
		c = fgetc(f);
	}
	fclose(f);
}

void EntityMovement(Entity &entity, bool left, bool right, bool up, float dt)
{
	//Accelerates the entity to the left
	if (left)
	{
		entity.velx -= entity.speed;
	}
	//Accelerates the entity to the right
	if (right)
	{
		entity.velx += entity.speed;
	}
	//Makes the entity jump
	if (up)
	{
		if (entity.canjump)
		{
			entity.vely = -entity.jump;
		}
		else if (entity.vely < 0)
		{
			entity.vely -= entity.jumpjet;
		}
	}
	//Produces friction
	if (entity.velx > 0)
	{
		entity.velx -= entity.friction;
	}
	else
	{
		entity.velx += entity.friction;
	}
	if (fabs(entity.velx) - entity.friction < entity.friction) {
		entity.velx = 0;
	}
	//Limits max velocity
	if (entity.velx > entity.maxspeed)
	{
		entity.velx = entity.maxspeed;
	}
	else if (entity.velx < -entity.maxspeed)
	{
		entity.velx = -entity.maxspeed;
	}
	//Applies accelerated motion
	entity.vely += gravity;
	entity.box.x += (int)(entity.velx * dt);
	entity.box.y += (int)(entity.vely * dt);
}

void PlayerControl(float dt)
{
	bool left = false;
	bool right = false;
	bool up = false;
	if (IsKeyDown(SDL_SCANCODE_A) || IsKeyDown(SDL_SCANCODE_LEFT))
	{
		left = true;
	}
	if (IsKeyDown(SDL_SCANCODE_D) || IsKeyDown(SDL_SCANCODE_RIGHT))
	{
		right = true;
	}
	if (IsKeyDown(SDL_SCANCODE_W) || IsKeyDown(SDL_SCANCODE_UP))
	{
		up = true;
	}
	EntityMovement(player, left, right, up, dt);
}

void EntityTerrainCollision(Entity &entity, float dt)
{
	// PLAYER GETS STUCK ON THE GRID GAPS!!!!
	// solution = diagnose for the exact exeption of touching corners, then probe below to decide if terrain below exists and act accordingly
	// solutionnn = make top-bottom collision points slightly inwards
	entity.canjump = false;
	SDL_Point left_top_side = { entity.box.x, entity.box.y + hiboxtolerance };
	SDL_Point right_top_side = { entity.box.x + entity.box.w, entity.box.y + hiboxtolerance };
	SDL_Point left_bottom_side = { entity.box.x, entity.box.y + entity.box.h - hiboxtolerance };
	SDL_Point right_bottom_side = { entity.box.x + entity.box.w, entity.box.y + entity.box.h - hiboxtolerance };
	SDL_Point left_top_base = { entity.box.x + hiboxtolerance, entity.box.y };
	SDL_Point right_top_base = { entity.box.x + entity.box.w - hiboxtolerance, entity.box.y };
	SDL_Point left_bottom_base = { entity.box.x + hiboxtolerance, entity.box.y + entity.box.h };
	SDL_Point right_bottom_base = { entity.box.x + entity.box.w - hiboxtolerance, entity.box.y + entity.box.h };
	for (int i = 0; i < terraincount; i++)
	{
		//Keeps the entity above a rectangle
		if ((SDL_PointInRect(&right_bottom_base, &terrain[i].box) || SDL_PointInRect(&left_bottom_base, &terrain[i].box)) && entity.past.y + entity.past.h <= terrain[i].box.y)
		{
			entity.box.y = terrain[i].box.y - entity.box.h;
			if (entity.vely > 0) {
				entity.vely = 0;
			}
			entity.canjump = true;
		}
		//Keeps the entity below a rectangle
		else if ((SDL_PointInRect(&right_top_base, &terrain[i].box) || SDL_PointInRect(&left_top_base, &terrain[i].box)) && entity.past.y >= terrain[i].box.y + terrain[i].box.h)
		{
			entity.box.y = terrain[i].box.y + terrain[i].box.h;
			if (entity.vely < 0) {
				entity.vely = 0;
			}
		}
		//Keeps the entity to the left of a rectangle
		if ((SDL_PointInRect(&right_bottom_side, &terrain[i].box) || SDL_PointInRect(&right_top_side, &terrain[i].box)) && entity.past.x + entity.past.w <= terrain[i].box.x)
		{
			entity.box.x = terrain[i].box.x - entity.box.w;
			if (entity.velx > 0) {
				entity.velx = 0;
			}
			entity.canjump = true;
		}
		//Keeps the entity to the right of a rectangle
		else if ((SDL_PointInRect(&left_bottom_side, &terrain[i].box) || SDL_PointInRect(&left_top_side, &terrain[i].box)) && entity.past.x >= terrain[i].box.x + terrain[i].box.w)
		{
			entity.box.x = terrain[i].box.x + terrain[i].box.w;
			if (entity.velx < 0) {
				entity.velx = 0;
			}
			entity.canjump = true;
		}
	}
	entity.past = entity.box;
}
void Collisions(float dt)
{
	EntityTerrainCollision(player, dt);
}

void Update(float dt)
{
	if (generate) 
	{
		LevelGeneration("test");
		generate = false;
	}
	PlayerControl(dt);
	//AiControl()
	Collisions(dt);

	if (IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		ExitGame();
	}
}

void RenderFrame(float interpolation)
{
	
	//Clear screen
	SDL_SetRenderDrawColor(gRenderer, 65, 105, 225, 255);
	SDL_RenderClear(gRenderer);
	//Render player
	SDL_SetRenderDrawColor(gRenderer, 160, 0, 160, 255);
	SDL_RenderFillRect(gRenderer, &player.box);
	//Render terrain
	SDL_SetRenderDrawColor(gRenderer, 120, 120, 120, 255);
	for (int i = 0; i < terraincount; i++)
	{
		SDL_RenderFillRect(gRenderer, &terrain[i].box);
	}
}
