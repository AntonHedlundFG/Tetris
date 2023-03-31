#include "TetrisBoard.h"

using std::make_pair;

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

	//Set up the static border grid and the grid that shows upcoming tiles.
	SetupOutlineGrid();
	SetupUpcomingGrid();

	//Camera & hardcoded position & rotation
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

	IsPlaying = false;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC) {
		PC->bShowMouseCursor = true;
	}

	//Set up all meshes to match the assigned StaticMesh
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
		UpcomingMeshGrid[i]->SetVisibility(false);
	}
	DrawGrid();
}

//Called by the UI button to start the game
void ATetrisBoard::StartNewGame() {
	
	IsPlaying = true;
	OnGameStateChange.Broadcast(true);

	NextTile = TetrisConstants::RandomTileType();
	ClearBoard();
	TrySpawnBlock(NextTile);
	DrawGrid();

	CurrentScore = 0;
	OnPointsChanged.Broadcast();
}

void ATetrisBoard::ClearBoard() {
	for (int i = 0; i < (TetrisConstants::Height * TetrisConstants::Width); i++) {
		StateGrid[i] = TetrisConstants::TileState::Empty;
	}
}

void ATetrisBoard::ResetAutoDownTimer() {
	GetWorldTimerManager().ClearTimer(GameplayTimerHandle);
	GetWorldTimerManager().SetTimer(GameplayTimerHandle, this, &ATetrisBoard::DoLoweringBlock, 1.0f, false, SecondsPerAutoDown);
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
		return make_pair(-1, -1);
	}
	return make_pair(index % TetrisConstants::Width, index / TetrisConstants::Width);
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
	return TrySpawnBlock(TetrisConstants::TileCoordinates(type));
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
		HoveringTileCoordinates[i] = coordinates[i];
	}
	NextTile = TetrisConstants::RandomTileType();
	UpdateUpcomingGrid();
	return ret;
}

//Tries lowering hovered blocks 1 step, if failed return false.
bool ATetrisBoard::TryLoweringBlock() {
	ResetAutoDownTimer();

	//Check if each hovered tile can be lowered without being blocked by floor or other tiles.
	//If they cannot, lock them in and spawn next tile.
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		if (coordinate < TetrisConstants::Width || StateGrid[coordinate - TetrisConstants::Width] == TetrisConstants::TileState::Filled)
		{
			LockHoveringTiles();
			if (!TrySpawnBlock(NextTile)) {
				LoseGame();
			}
			DrawGrid();
			return false;
		}
	}

	//Lower all hovered tiles by 1 row. This only happens if the blocking check above passes.
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		StateGrid[coordinate] = TetrisConstants::TileState::Empty;
		HoveringTileCoordinates[i].second--;
	}
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		StateGrid[coordinate] = TetrisConstants::TileState::Hovered;
	}

	DrawGrid();
	return true;
}
void ATetrisBoard::DoLoweringBlock() { TryLoweringBlock(); }

bool ATetrisBoard::TryMovingSideways(bool right) 
{
	//Check if each hovered tile can be moved without being blocked by wall or other tiles.
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		if (right) {
			if ((coordinate + 1) % TetrisConstants::Width == 0) { return false; }
			if (StateGrid[coordinate + 1] == TetrisConstants::TileState::Filled) { return false; }
		}
		else {
			if (coordinate % TetrisConstants::Width == 0) { return false; }
			if (StateGrid[coordinate - 1] == TetrisConstants::TileState::Filled) { return false; }
		}
	}

	//Move all hovered tiles 1 column. Only happens if the blocking check above passes.
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		StateGrid[coordinate] = TetrisConstants::TileState::Empty;
		HoveringTileCoordinates[i].first += (right ? 1 : -1);
	}
	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);
		StateGrid[coordinate] = TetrisConstants::TileState::Hovered;
	}

	DrawGrid();
	return true;
}

void ATetrisBoard::LockHoveringTiles() {
	for (int i = 0; i < 4; i++) {
		StateGrid[IndexFromCoord(HoveringTileCoordinates[i])] = TetrisConstants::TileState::Filled;
	}
	CheckRowRemoval();
}

void ATetrisBoard::CheckRowRemoval() {
	int rowsRemoved = 0;
	for (int j = TetrisConstants::Height; j >= 0; j--) {
		bool rowFilled = true;
		for (int i = 0; i < TetrisConstants::Width; i++) {
			if (StateGrid[j * TetrisConstants::Width + i] != TetrisConstants::TileState::Filled) {
				rowFilled = false;
			}
		}
		if (rowFilled) {
			ClearRow(j);
			rowsRemoved++;
		}
	}
	AddScore(rowsRemoved);
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
	if (IsPlaying) { TryMovingSideways(false); }
}
void ATetrisBoard::RightInput() {
	if (IsPlaying) { TryMovingSideways(true); }
}
void ATetrisBoard::DownInput() {
	if (IsPlaying) { TryLoweringBlock(); }
}
void ATetrisBoard::FastDropInput() {
	if (!IsPlaying) { return; }
	IsPlaying = false;
	GetWorldTimerManager().SetTimer(FastDropTimerHandle, this, &ATetrisBoard::FastDropTimerFunction, 1.0f, false, SecondsPerFastDrop);
	
}
void ATetrisBoard::FastDropTimerFunction() {
	if (TryLoweringBlock()) {
		GetWorldTimerManager().SetTimer(FastDropTimerHandle, this, &ATetrisBoard::FastDropTimerFunction, 1.0f, false, SecondsPerFastDrop);
	} else {
		IsPlaying = true;
	}
}

void ATetrisBoard::RotateInput() {
	if (!IsPlaying) { return; }

	//arrays to store coordinates
	pair<int, int> oldCoords[4];
	pair<int, int> newCoords[4];
	
	if (!CheckRotation(oldCoords, newCoords, 0) 
		&& !CheckRotation(oldCoords, newCoords, 1)
		&& !CheckRotation(oldCoords, newCoords, -1))
		{ return; }

	//If we pass the rotation check above, use the stored coordinates to actually rotate the tile.
	for (int i = 0; i < 4; i++) {
		StateGrid[IndexFromCoord(oldCoords[i])] = TetrisConstants::TileState::Empty;
	}
	for (int i = 0; i < 4; i++) {
		StateGrid[IndexFromCoord(newCoords[i])] = TetrisConstants::TileState::Hovered;
		HoveringTileCoordinates[i] = newCoords[i];
	}

	DrawGrid();
}

bool ATetrisBoard::CheckRotation(pair<int, int> oldCoords[4], pair<int, int> newCoords[4], int SideMod) {
	if (SideMod != 0) {
		SideMod = SideMod > 0 ? 1 : -1;
	}

	pair<int, int> rotationCoord = HoveringTileCoordinates[2];
	int coordIndex = 0;

	for (int i = 0; i < 4; i++) {
		int coordinate = IndexFromCoord(HoveringTileCoordinates[i]);

		//90 degree coordinate rotation around rotation point.
		int x = rotationCoord.first + (HoveringTileCoordinates[i].second - rotationCoord.second) + SideMod;
		int y = rotationCoord.second - (HoveringTileCoordinates[i].first - rotationCoord.first);

		if (x < 0 || y < 0 || x >= TetrisConstants::Width || y >= TetrisConstants::Height) { return false; }
		if (StateGrid[x + y * TetrisConstants::Width] == TetrisConstants::TileState::Filled) { return false; }

		oldCoords[coordIndex] = make_pair(HoveringTileCoordinates[i].first, HoveringTileCoordinates[i].second);
		newCoords[coordIndex] = make_pair(x, y);
		coordIndex++;
	}

	return true;
}

void ATetrisBoard::LoseGame() {
	
	OnGameStateChange.Broadcast(false);
	GetWorldTimerManager().ClearTimer(GameplayTimerHandle);
	IsPlaying = false;
}

void ATetrisBoard::AddScore(int rows) {
	int addScore = TetrisConstants::Score(rows);
	if (addScore == 0) { return; }
	CurrentScore += addScore;
	OnPointsChanged.Broadcast();
}