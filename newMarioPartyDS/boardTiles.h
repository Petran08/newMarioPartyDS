#pragma once
#include "raylib.h"
#include <fstream>
#include <string>
#include <vector>

void readTileData();

class boardTiles
{
public:
	Vector3 position;
	std::string type;
	int nextTile[4];
	bool jumpOnNext = false;
	float jumpHeight;
	int maxDir = 1;
	bool countDown = true;
};

extern std::vector<boardTiles>;