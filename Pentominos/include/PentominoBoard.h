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

	

	class PentominoBoard
	{
	public:
		PentominoBoard() {}; // prompt user to input
		PentominoBoard(const std::string& board) 
			: mBoard{ board } {  } 
		
		void promptUserInputBoard();
		void printBoard() const;

	private:
		std::string mBoard{};
		
		int mWidth{};
		int mStrWidth{}; // mWidth+1
		int mHeight{};

		void removeRow(int row);
		void removeColumn(int col);
		void rectangularizeBoard();
		void trimBoard();
		

	};


}