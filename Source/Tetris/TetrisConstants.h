// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"

using std::pair;

/**
 * 
 */
class TETRIS_API TetrisConstants
{
public:
	TetrisConstants();
	~TetrisConstants();

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
	// Y-coordinate 0 is top row, Y <= 0
	// X-coordinate 0 should be at width/2 + 1
	static pair<int, int>* TileCoordinates(TileType type);
};
