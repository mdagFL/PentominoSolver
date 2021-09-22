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
			: mBoard{ board }, mPlacedPentominoes{ } {  } 
		
		void promptUserInputBoard();
		void printBoard() const;

	private:
		std::string mBoard{};
		
		std::vector<PlacedPentomino> mPlacedPentominoes{};
		int mWidth{};
		int mStrWidth{}; // mWidth+1
		int mHeight{};
		void removeRow(int row);
		void removeColumn(int col);
		void rectangularizeBoard();
		void trimBoard();
		bool pushPentomino(const Pentomino& piece, const Point& pos);
		PlacedPentomino popPentomino();
		

	};


}