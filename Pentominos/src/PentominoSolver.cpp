#include <cassert>

#include "PentominoSolver.h"

namespace Pentominoes
{

	// If legal placement, returns true and places the piece on the board.
	bool PentominoSolver::tryPushPentomino(const Pentomino& piece, const Point& pos)
	{

		piece.GetXOffset();
		const std::string& pieceStr = piece.getDataString();

		mPlacedPentominoes.push_back(PlacedPentomino{ piece, pos });
		return false;
	}

	// Precondition: At least 1 placed pentomino on the board
	// Pop the last placed pentomino
	PlacedPentomino PentominoSolver::popPentomino()
	{
		assert(mPlacedPentominoes.size() > 0);
		PlacedPentomino piece = mPlacedPentominoes.back();
		mPlacedPentominoes.pop_back();
		return piece;
	}

	std::vector<PentominoSolver> PentominoSolver::findAllSolutions()
	{
		// TO DO
		return std::vector<PentominoSolver>();
	}

	std::vector<int> PentominoSolver::findIslandAreas() const
	{
		// TO DO
		return std::vector<int>();
	}
}
