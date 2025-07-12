#pragma once
#include "raylib.h"
#include <string>
#include <vector>

class boardTiles
{
public:
	Vector3 position;
	std::string type;
	int nextTile[4];
	bool jumpOnNext = false;
	float jumpHeight;
};

extern std::vector<boardTiles>;