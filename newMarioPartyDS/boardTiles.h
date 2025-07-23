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
	std::string type = "green";
	int nextTile[4];
	bool jumpOnNext = false;
	float jumpHeight;
	int maxDir = 1;
	short int countDown = 1;
};

extern std::vector<boardTiles> tiles;