#include "boardTiles.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

std::vector<boardTiles> tiles;

void readTileData(std::string boardName)
{
	std::ifstream readTile(boardName + ".txt");
	std::string useless;
	boardTiles init;
	readTile >> useless;
	int nrTiles;
	readTile >> nrTiles;
	for (int i = 0; i < nrTiles; i++)
	{
		//tile id(just for readability in tiles.txt)
		readTile >> useless;

		readTile >> useless;
		float x, y, z;
		readTile >> x >> y >> z;
		init.position = Vector3{ x, y, z };
		
		readTile >> useless;
		readTile >> init.type;
		
		readTile >> useless;
		readTile >> init.maxDir;
		
		readTile >> useless;
		for (int j = 0; j < init.maxDir; j++)
		{
			readTile >> init.nextTile[j];
		}

		readTile >> useless;
		readTile >> init.jumpOnNext;

		readTile >> useless;
		readTile >> init.jumpHeight;

		readTile >> useless;
		readTile >> init.countDown;
	
		tiles.push_back(init);
	}
}
