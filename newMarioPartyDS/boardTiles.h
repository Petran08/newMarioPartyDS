#pragma once
#include "raylib.h"
#include <fstream>
#include <string>
#include <vector>

void readTileData(std::string boardName);

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


//if tiles size = 0 add first tile, next tile 0
//select tile with p/l
//move to another position with arrow keys, space and shift
//press m, add tiles.size to next tile in the selected tile(at maxDir position) and increase maxDir for selected tile
//push back new tile at player position, type green