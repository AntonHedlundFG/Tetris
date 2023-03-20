// Fill out your copyright notice in the Description page of Project Settings.


#include "TetrisBoard.h"

// Sets default values
ATetrisBoard::ATetrisBoard()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup for all Cube MeshComponents
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		//Generate cube at index i
		FString s = FString("Tile_");
		s.AppendInt(i);
		MeshGrid[i] = CreateDefaultSubobject<UStaticMeshComponent>(FName(*s));

		//Set cube position
		pair<int, int> coords = CoordFromIndex(i);
		MeshGrid[i]->SetRelativeLocation(FVector(coords.first, coords.second, 0) * 100.0f);

		//Set parent/root structure, cube 0 is root&parent component.
		if (i == 0) {
			SetRootComponent(MeshGrid[i]);
		} else {
			MeshGrid[i]->SetupAttachment(MeshGrid[0]);
		}
	}

	SetupOutlineGrid();
	
}

void ATetrisBoard::SetupOutlineGrid() {
	int i;
	int x = -2;
	int y = -1;

	//Bottom row
	for (i = 0; i < (TetrisConstants::Width + 2); i++) {
		x++;
		MakeOutlineTile(i, FVector(x, y, 0));
	}

	//Right column
	for (; i < (TetrisConstants::Width + TetrisConstants::Height + 3); i++) {
		y++;
		MakeOutlineTile(i, FVector(x, y, 0));
	}

	//Top row
	for (; i < (TetrisConstants::Width * 2 + TetrisConstants::Height + 4); i++) {
		x--;
		MakeOutlineTile(i, FVector(x, y, 0));
	}

	//Left column
	for (; i < (2 * (TetrisConstants::Width + TetrisConstants::Height) + 4); i++) {
		y--; 
		MakeOutlineTile(i, FVector(x, y, 0));
	}
}

void ATetrisBoard::MakeOutlineTile(int index, FVector pos) {
	FString s = FString("OutlineTile_");
	s.AppendInt(index);
	OutlineMeshGrid[index] = CreateDefaultSubobject<UStaticMeshComponent>(FName(*s));
	OutlineMeshGrid[index]->SetRelativeLocation(pos * 100.0f);
	OutlineMeshGrid[index]->SetupAttachment(MeshGrid[0]);
}

// Called when the game starts or when spawned
void ATetrisBoard::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		MeshGrid[i]->SetStaticMesh(StaticMesh);
	}

	for (int i = 0; i < (2 * (TetrisConstants::Width + TetrisConstants::Height) + 4); i++) {
		if (OutlineMeshGrid[i] == nullptr) { break; }
		OutlineMeshGrid[i]->SetStaticMesh(StaticMesh);
		OutlineMeshGrid[i]->SetMaterial(0, OutlineMaterial);
	}
	
	TrySpawnBlock(TetrisConstants::TileType::J);
	DrawGrid();

	//TimerRepetitions = 0;
	//GetWorldTimerManager().SetTimer(GameplayTimerHandle, this, &ATetrisBoard::TestTimerFunction, 0.25f, true, 0.25f);
}

//TEST FUNCTION DELETE LATER
void ATetrisBoard::TestTimerFunction() {
	bool result;
	switch (TimerRepetitions) {
		case 0:
			result = TryMovingRight();
			TimerRepetitions++;
			break;
		case 1:
			result = TryMovingLeft();
			TimerRepetitions++;
			break;
		case 2:
			result = TryLoweringBlock();
			TimerRepetitions = 0;
			break;
	}
	if (!result) {
		LockHoveringTiles();
		GetWorldTimerManager().ClearTimer(GameplayTimerHandle);
	}
	DrawGrid();
}

// Called every frame
void ATetrisBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATetrisBoard::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);	
}

//These three methods are used to translate between coordinates (int pairs) to grid array index.
int ATetrisBoard::IndexFromCoord(int x, int y) {
	if (x < 0 || x > TetrisConstants::Width || y < 0 || y > TetrisConstants::Height) {
		return 0;
	}
	return x + y * TetrisConstants::Width;
}
int ATetrisBoard::IndexFromCoord(pair<int, int> coordinate) {
	return IndexFromCoord(coordinate.first, coordinate.second);
}
pair<int, int> ATetrisBoard::CoordFromIndex(int index) {
	if (index < 0 || index >= (TetrisConstants::Width * TetrisConstants::Height)) {
		return std::make_pair(-1, -1);
	}
	return std::make_pair(index % TetrisConstants::Width, index / TetrisConstants::Width);
}

//Updates board visibility from current logical grid
void ATetrisBoard::DrawGrid() {
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		switch (StateGrid[i]) {
			case TetrisConstants::TileState::Filled:
				MeshGrid[i]->SetVisibility(true);
				MeshGrid[i]->SetMaterial(0, FilledMaterial);
				break;
			case TetrisConstants::TileState::Empty:
				MeshGrid[i]->SetVisibility(false);
				break;
			case TetrisConstants::TileState::Hovered:
				MeshGrid[i]->SetVisibility(true);
				MeshGrid[i]->SetMaterial(0, HoverMaterial);
				break;
		}
	}
}

//Spawns a block of the designated type in the spawn position. 
//If any spawn position blocks are already occupied, returns false. Otherwise, true.
bool ATetrisBoard::TrySpawnBlock(TetrisConstants::TileType type) {
	pair<int, int> coordinates[4];
	switch (type) {
		case TetrisConstants::TileType::I:
			coordinates[0] = std::make_pair(3, 19);
			coordinates[1] = std::make_pair(4, 19);
			coordinates[2] = std::make_pair(5, 19);
			coordinates[3] = std::make_pair(6, 19);
			break;
		case TetrisConstants::TileType::J:
			coordinates[1] = std::make_pair(3, 18);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(5, 18);
			coordinates[0] = std::make_pair(3, 19);
			break;
		case TetrisConstants::TileType::L:
			coordinates[1] = std::make_pair(3, 18);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(5, 18);
			coordinates[0] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::O:
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(5, 18);
			coordinates[0] = std::make_pair(4, 19);
			coordinates[1] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::S:
			coordinates[1] = std::make_pair(3, 18);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(4, 19);
			coordinates[0] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::T:
			coordinates[1] = std::make_pair(3, 18);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[0] = std::make_pair(5, 18);
			coordinates[3] = std::make_pair(4, 19);
			break;
		case TetrisConstants::TileType::Z:
			coordinates[2] = std::make_pair(4, 18);
			coordinates[0] = std::make_pair(5, 18);
			coordinates[1] = std::make_pair(3, 19);
			coordinates[3] = std::make_pair(4, 19);
			break;
	}
	return TrySpawnBlock(coordinates);
}
bool ATetrisBoard::TrySpawnBlock(pair<int, int> coordinates[]) {
	bool ret = true;
	for (int i = 0; i < 4; i++) {
		int gridIndex = IndexFromCoord(coordinates[i]);
		if (StateGrid[gridIndex] == TetrisConstants::TileState::Filled) {
			ret = false;
		}
		else {
			StateGrid[gridIndex] = TetrisConstants::TileState::Hovered;
		}
	}
	return ret;
}

//Tries lowering hovered blocks 1 step, if failed return false.
bool ATetrisBoard::TryLoweringBlock() {

	//Check if each hovered tile can be lowered without being blocked by floor or other tiles.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		if (i < TetrisConstants::Width) { return false; }
		if (StateGrid[i - TetrisConstants::Width] == TetrisConstants::TileState::Filled) { return false; }
	}

	//Lower all hovered tiles by 1 row. This only happens if the blocking check above passes.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		StateGrid[i] = TetrisConstants::TileState::Empty;
		StateGrid[i - TetrisConstants::Width] = TetrisConstants::TileState::Hovered;
	}

	return true;
}

//IMPLEMENT THESE
bool ATetrisBoard::TryMovingRight() {

	//Check if each hovered tile can be moved without being blocked by wall or other tiles.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		if ((i+1) % TetrisConstants::Width == 0) { return false; }
		if (StateGrid[i+1] == TetrisConstants::TileState::Filled) { return false; }
	}

	//Move all hovered tiles 1 column to the right. Only happens if the blocking check above passes.
	for (int i = TetrisConstants::Width - 1; i >= 0; i--) {
		for (int j = 0; j < TetrisConstants::Height; j++) {
			int index = j * TetrisConstants::Width + i;
			if (StateGrid[index] != TetrisConstants::TileState::Hovered) { continue; }

			StateGrid[index] = TetrisConstants::TileState::Empty;
			StateGrid[index+1] = TetrisConstants::TileState::Hovered;
		}
	}

	return true;
}

bool ATetrisBoard::TryMovingLeft() {
	//Check if each hovered tile can be moved without being blocked by wall or other tiles.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		if (i % TetrisConstants::Width == 0) { return false; }
		if (StateGrid[i - 1] == TetrisConstants::TileState::Filled) { return false; }
	}

	//Move all hovered tiles 1 column to the right. Only happens if the blocking check above passes.
	for (int i = 0; i < TetrisConstants::Width; i++) {
		for (int j = 0; j < TetrisConstants::Height; j++) {
			int index = j * TetrisConstants::Width + i;
			if (StateGrid[index] != TetrisConstants::TileState::Hovered) { continue; }

			StateGrid[index] = TetrisConstants::TileState::Empty;
			StateGrid[index - 1] = TetrisConstants::TileState::Hovered;
		}
	}

	return true;
}

void ATetrisBoard::LockHoveringTiles() {
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] == TetrisConstants::TileState::Hovered) {
			StateGrid[i] = TetrisConstants::TileState::Filled;
		}
	}
}