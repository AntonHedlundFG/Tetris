#include "TetrisConstants.h"

using std::make_pair;

TetrisConstants::TileType TetrisConstants::RandomTileType() {
	TileType returnType = static_cast<TileType>(std::rand() % 7);
	return returnType;
}

pair<int, int>* TetrisConstants::TileCoordinates(TileType type) {
	static pair<int, int> coordinates[4];
	switch (type) {
	case TetrisConstants::TileType::I:
		coordinates[0] = make_pair(-2, 0);
		coordinates[1] = make_pair(-1, 0);
		coordinates[2] = make_pair(0, 0);
		coordinates[3] = make_pair(1, 0);
		break;
	case TetrisConstants::TileType::J:
		coordinates[0] = make_pair(0, -1);
		coordinates[1] = make_pair(-1, -1);
		coordinates[2] = make_pair(-2, -1);
		coordinates[3] = make_pair(-2, 0);
		break;
	case TetrisConstants::TileType::L:
		coordinates[0] = make_pair(-2, -1);
		coordinates[1] = make_pair(-1, -1);
		coordinates[2] = make_pair(0, -1);
		coordinates[3] = make_pair(0, 0);
		break;
	case TetrisConstants::TileType::O:
		coordinates[0] = make_pair(-1, -1);
		coordinates[1] = make_pair(0, -1);
		coordinates[2] = make_pair(-1, 0);
		coordinates[3] = make_pair(0, 0);
		break;
	case TetrisConstants::TileType::S:
		coordinates[0] = make_pair(-2, -1);
		coordinates[1] = make_pair(-1, -1);
		coordinates[2] = make_pair(-1, 0);
		coordinates[3] = make_pair(0, 0);
		break;
	case TetrisConstants::TileType::T:
		coordinates[0] = make_pair(-1, 0);
		coordinates[1] = make_pair(-2, -1);
		coordinates[2] = make_pair(-1, -1);
		coordinates[3] = make_pair(0, -1);
		break;
	case TetrisConstants::TileType::Z:
		coordinates[0] = make_pair(-2, 0);
		coordinates[1] = make_pair(-1, 0);
		coordinates[2] = make_pair(-1, -1);
		coordinates[3] = make_pair(0, -1);
		break;
	}

	for (int i = 0; i < 4; i++) {
		coordinates[i].first += TetrisConstants::Width / 2 + 1; //From 0 to middle of the board
		coordinates[i].second += TetrisConstants::Height - 1; //From 0 to top of the board
	}
	return coordinates;
}

int TetrisConstants::Score(int rows)
{
	switch (rows) {
	case 1:
		return 40;
	case 2:
		return 100;
	case 3:
		return 300;
	case 4:
		return 1200;
	default:
		return 0;
	}
}