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
	SetupUpcomingGrid();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(FName("CameraComponent"));
	CameraComponent->AttachToComponent(MeshGrid[0], FAttachmentTransformRules::KeepRelativeTransform);
	CameraComponent->SetRelativeLocation(FVector(5, 10, -20) * 100.0f);
	CameraComponent->SetRelativeRotation(FRotator(90, -90, 0));
	
}

void ATetrisBoard::SetupUpcomingGrid() {
	
	for (int i = 0; i < 8; i++) {
		MakeUpcomingTile(i, FVector(15 + (i % 4), 10 + (i / 4), 0));
	}
}

void ATetrisBoard::MakeUpcomingTile(int index, FVector pos) {
	FString s = FString("UpcomingTile_");
	s.AppendInt(index);
	UpcomingMeshGrid[index] = CreateDefaultSubobject<UStaticMeshComponent>(FName(*s));
	UpcomingMeshGrid[index]->SetRelativeLocation(pos * 100.0f);
	UpcomingMeshGrid[index]->SetupAttachment(MeshGrid[0]);
}

void ATetrisBoard::UpdateUpcomingGrid() {
	for (int i = 0; i < 8; i++) {
		UpcomingMeshGrid[i]->SetVisibility(false);
	}
	switch (NextTile) {
	case TetrisConstants::TileType::I:
		UpcomingMeshGrid[0]->SetVisibility(true); 
		UpcomingMeshGrid[1]->SetVisibility(true); 
		UpcomingMeshGrid[2]->SetVisibility(true); 
		UpcomingMeshGrid[3]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::J:
		UpcomingMeshGrid[0]->SetVisibility(true);
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[2]->SetVisibility(true);
		UpcomingMeshGrid[4]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::L:
		UpcomingMeshGrid[0]->SetVisibility(true);
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[2]->SetVisibility(true);
		UpcomingMeshGrid[6]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::O:
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[2]->SetVisibility(true);
		UpcomingMeshGrid[5]->SetVisibility(true);
		UpcomingMeshGrid[6]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::S:
		UpcomingMeshGrid[0]->SetVisibility(true);
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[5]->SetVisibility(true);
		UpcomingMeshGrid[6]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::T:
		UpcomingMeshGrid[0]->SetVisibility(true);
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[2]->SetVisibility(true);
		UpcomingMeshGrid[5]->SetVisibility(true);
		break;
	case TetrisConstants::TileType::Z:
		UpcomingMeshGrid[1]->SetVisibility(true);
		UpcomingMeshGrid[2]->SetVisibility(true);
		UpcomingMeshGrid[4]->SetVisibility(true);
		UpcomingMeshGrid[5]->SetVisibility(true);
		break;
	}
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

	for (int i = 0; i < 8; i++) {
		UpcomingMeshGrid[i]->SetStaticMesh(StaticMesh);
		UpcomingMeshGrid[i]->SetMaterial(0, FilledMaterial);
	}

	NextTile = TetrisConstants::RandomTileType();
	TrySpawnBlock(NextTile);
	DrawGrid();
}


void ATetrisBoard::ResetAutoDownTimer() {
	GetWorldTimerManager().ClearTimer(GameplayTimerHandle);
	GetWorldTimerManager().SetTimer(GameplayTimerHandle, this, &ATetrisBoard::DoLoweringBlock, 1.0f, false, 2.0f);
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

	PlayerInputComponent->BindAction("Left", IE_Pressed, this, &ATetrisBoard::LeftInput);
	PlayerInputComponent->BindAction("Right", IE_Pressed, this, &ATetrisBoard::RightInput);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &ATetrisBoard::DownInput);
	PlayerInputComponent->BindAction("FastDrop", IE_Pressed, this, &ATetrisBoard::FastDropInput);
	PlayerInputComponent->BindAction("Rotate", IE_Pressed, this, &ATetrisBoard::RotateInput);
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
			coordinates[0] = std::make_pair(5, 18);
			coordinates[1] = std::make_pair(4, 18);
			coordinates[2] = std::make_pair(3, 18);
			coordinates[3] = std::make_pair(3, 19);
			break;
		case TetrisConstants::TileType::L:
			coordinates[0] = std::make_pair(3, 18);
			coordinates[1] = std::make_pair(4, 18);
			coordinates[2] = std::make_pair(5, 18);
			coordinates[3] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::O:
			coordinates[0] = std::make_pair(4, 18);
			coordinates[1] = std::make_pair(5, 18);
			coordinates[2] = std::make_pair(4, 19);
			coordinates[3] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::S:
			coordinates[0] = std::make_pair(3, 18);
			coordinates[1] = std::make_pair(4, 18);
			coordinates[2] = std::make_pair(4, 19);
			coordinates[3] = std::make_pair(5, 19);
			break;
		case TetrisConstants::TileType::T:
			coordinates[0] = std::make_pair(4, 19);
			coordinates[1] = std::make_pair(3, 18);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(5, 18);
			break;
		case TetrisConstants::TileType::Z:
			coordinates[0] = std::make_pair(3, 19);
			coordinates[1] = std::make_pair(4, 19);
			coordinates[2] = std::make_pair(4, 18);
			coordinates[3] = std::make_pair(5, 18);
			break;
	}
	RotationPointIndex = IndexFromCoord(coordinates[2]);
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
	NextTile = TetrisConstants::RandomTileType();
	UpdateUpcomingGrid();
	return ret;
}

//Tries lowering hovered blocks 1 step, if failed return false.
bool ATetrisBoard::TryLoweringBlock() {
	ResetAutoDownTimer();
	//Check if each hovered tile can be lowered without being blocked by floor or other tiles.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		if (i < TetrisConstants::Width || StateGrid[i - TetrisConstants::Width] == TetrisConstants::TileState::Filled) {
			LockHoveringTiles();
			if (!TrySpawnBlock(NextTile)) {
				LoseGame();
			}
			DrawGrid();
			return false; 
		}
	}

	//Lower all hovered tiles by 1 row. This only happens if the blocking check above passes.
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] != TetrisConstants::TileState::Hovered) { continue; }

		StateGrid[i] = TetrisConstants::TileState::Empty;
		StateGrid[i - TetrisConstants::Width] = TetrisConstants::TileState::Hovered;
	}
	RotationPointIndex -= TetrisConstants::Width;
	DrawGrid();
	return true;
}
void ATetrisBoard::DoLoweringBlock() { TryLoweringBlock(); }

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
	RotationPointIndex++;

	DrawGrid();
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
	RotationPointIndex--;

	DrawGrid();
	return true;
}

void ATetrisBoard::LockHoveringTiles() {
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		if (StateGrid[i] == TetrisConstants::TileState::Hovered) {
			StateGrid[i] = TetrisConstants::TileState::Filled;
		}
	}
	CheckRowRemoval();
}

void ATetrisBoard::CheckRowRemoval() {
	int totalRows = 0;
	for (int j = TetrisConstants::Height; j >= 0; j--) {
		bool rowFilled = true;
		for (int i = 0; i < TetrisConstants::Width; i++) {
			if (StateGrid[j * TetrisConstants::Width + i] != TetrisConstants::TileState::Filled) {
				rowFilled = false;
			}
		}
		if (rowFilled) {
			ClearRow(j);
			totalRows++;
		}
	}
	AddScore(totalRows);
}

void ATetrisBoard::ClearRow(int row) {
	for (int j = row; j < TetrisConstants::Height - 1; j++) {
		for (int i = 0; i < TetrisConstants::Width; i++) {
			StateGrid[j * TetrisConstants::Width + i] = StateGrid[(j + 1) * TetrisConstants::Width + i];
		}
	}
	for (int i = 0; i < TetrisConstants::Width; i++) {
		StateGrid[(TetrisConstants::Height - 1) * TetrisConstants::Width + i] = TetrisConstants::TileState::Empty;
	}
}

void ATetrisBoard::LeftInput() {
	TryMovingLeft();
}
void ATetrisBoard::RightInput() {
	TryMovingRight();
}
void ATetrisBoard::DownInput() {
	TryLoweringBlock();
}
void ATetrisBoard::FastDropInput() {
	while(TryLoweringBlock()) {}
}
void ATetrisBoard::RotateInput() {
	//Get coordinate for the rotation point.
	pair<int, int> rotationCoord = CoordFromIndex(RotationPointIndex);

	//arrays to store coordinates
	pair<int, int> oldCoords[4];
	pair<int, int> newCoords[4];

	//iterator over coordinate arrays
	int coordIndex = 0;
	
	//This checks if a rotation is actually OK (not out of bounds or collide)
	//It also stores coordinates for hovering tiles, both old and new
	for (int i = 0; i < TetrisConstants::Width; i++) {
		for (int j = 0; j < TetrisConstants::Height; j++) {
			if (StateGrid[j * TetrisConstants::Width + i] != TetrisConstants::TileState::Hovered) { continue; }

			//90 degree coordinate rotation around rotation point.
			int x = rotationCoord.first + (j - rotationCoord.second);
			int y = rotationCoord.second - (i - rotationCoord.first);
			
			if (x < 0 || y < 0 || x >= TetrisConstants::Width || y >= TetrisConstants::Height) { return; }
			if (StateGrid[x + y * TetrisConstants::Width] == TetrisConstants::TileState::Filled) { return; }

			oldCoords[coordIndex] = std::make_pair(i, j);
			newCoords[coordIndex] = std::make_pair(x, y);
			coordIndex++;
		}
	}

	//If we pass the rotation check above, use the stored coordinates to actually rotate the tile.
	for (int i = 0; i < 4; i++) {
		StateGrid[IndexFromCoord(oldCoords[i])] = TetrisConstants::TileState::Empty;
	}
	for (int i = 0; i < 4; i++) {
		StateGrid[IndexFromCoord(newCoords[i])] = TetrisConstants::TileState::Hovered;
	}

	DrawGrid();
}

void ATetrisBoard::LoseGame() {
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController) {
		UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, true);
	}
}

void ATetrisBoard::AddScore(int amount) {
	switch (amount) {
		case 0:
			return;
		case 1:
			CurrentScore += 40;
			break;
		case 2:
			CurrentScore += 100;
			break;
		case 3:
			CurrentScore += 300;
			break;
		case 4:
			CurrentScore += 1200;
			break;
	}
	OnPointsChanged.Broadcast();
	//UKismetSystemLibrary::PrintString(this, FString::FromInt(CurrentScore));
}