// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

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

	static TileType RandomTileType();
};
