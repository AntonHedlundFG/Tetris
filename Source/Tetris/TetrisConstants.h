#pragma once

#include "CoreMinimal.h"

using std::pair;

class TETRIS_API TetrisConstants
{
public:
	
	static const int Height = 20;
	static const int Width = 10;

	enum TileState {
		Empty,
		Filled,
		Hovered
	};

	enum TileType {
		I,
		J,
		L,
		O,
		S,
		T,
		Z
	};

	//Randomly selects a TileType, no weighting
	static TileType RandomTileType();

	// Gives a pair<int, int>[4] of coordinates 
	// with rotation point at index [2]
	static pair<int, int>* TileCoordinates(TileType type);

	static int Score(int rows);
};
