// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisConstants.h"

TetrisConstants::TetrisConstants()
{
}

TetrisConstants::~TetrisConstants()
{
}


TetrisConstants::TileType TetrisConstants::RandomTileType() {
	TileType returnType = static_cast<TileType>(std::rand() % 7);
	return returnType;
}
