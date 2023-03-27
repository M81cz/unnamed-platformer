#include "engine.h"
#include "sprite.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "objects.h"

void Update(float dt);
void RenderFrame(float dt);

//=============================================================================

#define WW 1200
#define WH 900
const int ammount = 10;
int gravity = 15;
Entity player = {0, 0, 50, 50, 0, 0, 50, 50, 0, 0, 25, 350, 5, 500, 10, false};
Terrain terrain[ammount];

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

void TerGen()
{
	int spacing = 150;
	int xoffset = 75;
	int yoffset = 75;
	for (int i = 0; i < ammount; i++)
	{
		if (i % 2 == 0)
		{
			terrain[i].box.w = 450;
			terrain[i].box.h = 200;
			terrain[i].box.x = xoffset;
			terrain[i].box.y = yoffset + (i * spacing);
		}
		else
		{
			terrain[i].box.w = 450;
			terrain[i].box.h = 100;
			terrain[i].box.x = WW - xoffset - terrain[i].box.w;
			terrain[i].box.y = yoffset + (i * spacing);
		}
	}
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
	entity.canjump = false;
	SDL_Point left_top = { entity.box.x, entity.box.y };
	SDL_Point right_top = { entity.box.x + entity.box.w, entity.box.y };
	SDL_Point left_bottom = { entity.box.x, entity.box.y + entity.box.h };
	SDL_Point right_bottom = { entity.box.x + entity.box.w, entity.box.y + entity.box.h };
	for (int i = 0; i < ammount; i++)
	{
		//Keeps the entity above a rectangle
		if ((SDL_PointInRect(&right_bottom, &terrain[i].box) || SDL_PointInRect(&left_bottom, &terrain[i].box)) && entity.past.y + entity.past.h <= terrain[i].box.y)
		{
			entity.box.y = terrain[i].box.y - entity.box.h;
			entity.vely = 0;
			entity.canjump = true;
		}
		//Keeps the entity below a rectangle
		else if ((SDL_PointInRect(&right_top, &terrain[i].box) || SDL_PointInRect(&left_top, &terrain[i].box)) && entity.past.y >= terrain[i].box.y + terrain[i].box.h)
		{
			entity.box.y = terrain[i].box.y + terrain[i].box.h;
			entity.vely = 0;
		}
		//Keeps the entity to the left of a rectangle
		else if (SDL_PointInRect(&right_bottom, &terrain[i].box) || SDL_PointInRect(&right_top, &terrain[i].box))
		{
			entity.box.x = terrain[i].box.x - entity.box.w;
			entity.velx = 0;
			entity.canjump = true;
		}
		//Keeps the entity to the right of a rectangle
		else if (SDL_PointInRect(&left_bottom, &terrain[i].box) || SDL_PointInRect(&left_top, &terrain[i].box))
		{
			entity.box.x = terrain[i].box.x + terrain[i].box.w;
			entity.velx = 0;
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
	TerGen();
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
	for (int i = 0; i < ammount; i++)
	{
		SDL_RenderFillRect(gRenderer, &terrain[i].box);
	}
}
