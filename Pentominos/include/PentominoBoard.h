#pragma once
#include <string>
#include <vector>

#include "Pentomino.h"

namespace Pentominoes
{
	struct Point
	{
		int x{ 0 };
		int y{ 0 };
		Point(int a_x, int a_y) : x{ a_x }, y{ a_y }
		{}
	};

	struct PlacedPentomino
	{
		Pentomino pentomino;
		Point position;
		PlacedPentomino(const Pentomino& a_pentomino, Point a_position) : pentomino{ a_pentomino }, position{ a_position } {}
	};

	class PentominoBoard
	{
	public:
		PentominoBoard() {}; // prompt user to input
		PentominoBoard(const std::string& board) 
			: mBoard{ board }, mPentominoes{ std::vector<PlacedPentomino>()} {  } // shouldn't work?
		
		void promptUserInputBoard();
		
		bool hasNoSolution() const;
	private:
		std::string mBoard{};
		std::vector<PlacedPentomino> mPentominoes{};

		void placePentomino(const Pentomino& piece, Point pos);
		

	};


}