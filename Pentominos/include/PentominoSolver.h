#pragma once
#include <vector>

#include "Pentomino.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	struct PlacedPentomino
	{
		Pentomino pentomino;
		Point position;
		PlacedPentomino(const Pentomino& a_pentomino, Point a_position) : pentomino{ a_pentomino }, position{ a_position } {}
	};

	class PentominoSolver 
	{
	public:
		PentominoSolver(const PentominoBoard& board) : mBoard{ board }, mPlacedPentominoes {} {}
		
		static std::vector<Pentominoes::PentominoSolver> findAllSolutions();
	private:		
		const PentominoBoard& mBoard;
		std::vector<PlacedPentomino> mPlacedPentominoes{};

		
		std::vector<int> findIslandAreas() const;
		bool tryPushPentomino(const Pentomino& piece, const Point& pos);
		PlacedPentomino popPentomino();
	};
}