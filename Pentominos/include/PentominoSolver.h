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
		char symbol;
		PlacedPentomino(const Pentomino& a_pentomino, Point a_position, char a_symbol)
			: pentomino{ a_pentomino }, position{ a_position }, symbol{a_symbol} {}
	};

	class PentominoSolver 
	{
	public:
		PentominoSolver(const PentominoBoard& board) : mBoard{ board }, mPlacedPentominoes {} {}
		bool tryPushPentomino(const Pentomino& piece, const Point& pos);
		PlacedPentomino popPentomino();
		
		static std::vector<Pentominoes::PentominoSolver> findAllSolutions();
	private:		
		PentominoBoard mBoard;
		std::vector<PlacedPentomino> mPlacedPentominoes{};

		
		std::vector<int> findHoleAreas() const;
		char mNextSymbol{ 'A' }; // 1-char symbol to represent each instance of a piece in the solution
		
		
	};
}