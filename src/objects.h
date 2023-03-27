#include "sprite.h"

struct Entity
{
	SDL_Rect box;
	SDL_Rect past;
	double velx;
	double vely;
	double speed;
	double maxspeed;
	double friction;
	double jump;
	double jumpjet;
	bool canjump;
};
struct Terrain
{
	SDL_Rect box;
};