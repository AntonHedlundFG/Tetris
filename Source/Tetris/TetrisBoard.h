// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "TetrisConstants.h"
#include "Materials/Material.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Camera/CameraComponent.h"
#include "Delegates/Delegate.h"
#include "TetrisBoard.generated.h"

using std::pair;
class UStaticMeshComponent;
class UMaterial;
class FTimerManager;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPointsChangedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameStateChange, bool, NewState);

UCLASS(BlueprintType, Blueprintable)
class TETRIS_API ATetrisBoard : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere)
	UMaterial* FilledMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* HoverMaterial;

	UPROPERTY(EditAnywhere)
	UMaterial* OutlineMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintAssignable)
	FOnPointsChangedDelegate OnPointsChanged;

	UPROPERTY(BlueprintAssignable)
	FOnGameStateChange OnGameStateChange;

	UFUNCTION(BlueprintCallable)
	void StartNewGame();

	UPROPERTY(EditAnywhere)
	float SecondsPerAutoDown = 1.0f;

	UPROPERTY(EditAnywhere)
	float SecondsPerFastDrop = 0.05f;

protected:
	//The grid of pointers that make up the physical game board.
	UStaticMeshComponent* MeshGrid[TetrisConstants::Height * TetrisConstants::Width];

	//The grid of TileStates that make up the logical game board
	TetrisConstants::TileState StateGrid[TetrisConstants::Height * TetrisConstants::Width];

	//The static border grid.
	UStaticMeshComponent* OutlineMeshGrid[2 * (TetrisConstants::Width + TetrisConstants::Height) + 4];
	void SetupOutlineGrid();
	void MakeOutlineTile(int index, FVector pos);

	//The grid that shows the upcoming piece.
	UStaticMeshComponent* UpcomingMeshGrid[8];
	void SetupUpcomingGrid();
	void MakeUpcomingTile(int index, FVector pos);
	void UpdateUpcomingGrid();

	//GRID COORDINATE METHODS
	// Order: Left-To-Right -> Bottom-To-Top
	//  Y
	// [2] 6   7   8
	// [1] 3   4   5
	// [0] 0   1   2
	//    [0] [1] [2] X

	//Index <-> Coordinate translation for the grid array
	int IndexFromCoord(int x, int y);
	int IndexFromCoord(pair<int, int> coordinate);
	pair<int, int> CoordFromIndex(int index);

	//Updates board visibility from current logical grid
	void DrawGrid();

	//Spawns a block of the designated type in the spawn position. 
	//If any spawn position blocks are already occupied, returns false. Otherwise, true.
	bool TrySpawnBlock(TetrisConstants::TileType type);
	bool TrySpawnBlock(pair<int, int> coordinates[]);

	//Tries lowering hovered blocks 1 step, if failed return false.
	bool TryLoweringBlock();
	void DoLoweringBlock();
	bool TryMovingSideways(bool right);

	pair<int, int> HoveringTileCoordinates[4];

	//Turns hovering tiles to filled tiles
	void LockHoveringTiles();

	//Checks which rows should be cleared after a hovering tile lands.
	//ClearRow actually removes the row and lowers the ones above.
	void CheckRowRemoval();
	void ClearRow(int row);

	//Timer Handle that handles automatically lowering blocks after a few seconds
	FTimerHandle GameplayTimerHandle;
	void ResetAutoDownTimer();

	FTimerHandle FastDropTimerHandle;
	void FastDropTimerFunction();

	//Input handlers
	void LeftInput();
	void RightInput();
	void DownInput();
	void FastDropInput();
	void RotateInput();

	//Rotation checks
	bool CheckRotation(pair<int, int> oldCoords[4], pair<int, int> newCoords[4], int SideMod);

	//Quit game when you lose
	void LoseGame();
	void ClearBoard();

	//Update score, invokes event
	void AddScore(int rows);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 CurrentScore;

	//Indicates which TileType will be spawned next
	TetrisConstants::TileType NextTile;

	//Is a game active?
	bool IsPlaying;

	//Index of tile that current TileType should be rotated around
	int RotationPointIndex = -1;

public:
	// Sets default values for this pawn's properties
	ATetrisBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
