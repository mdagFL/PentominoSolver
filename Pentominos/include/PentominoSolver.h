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
		bool isPossibleSolution();
		

		PlacedPentomino popPentomino(); 
		void searchSimple(const Pentomino& piece, const Point& pos, int depth);


		static void findAllSolutions(const PentominoBoard& board);
		
	private:		
		PentominoBoard mBoard;
		std::vector<PlacedPentomino> mPlacedPentominoes{};

		std::vector<int> findHoleAreas();
		int findHoleArea(const Point& posHole);
		char mNextSymbol{ 'A' }; // 1-char symbol to represent each instance of a piece in the solution
		static int numSolutions;
		
		
	};
}