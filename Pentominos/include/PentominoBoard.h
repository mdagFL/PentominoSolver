#pragma once
#include <string>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include <wincon.h>

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
		friend class PentominoSolver;
		PentominoBoard() {}; // prompt user to input
		PentominoBoard(const std::string& board)
			: mBoard{ board } {  }

		void promptUserInputBoard();
		void printBoard(); // Would be const, but new lines may be inserted and then removed..
		int getWidth() const { return mWidth; }
		int getHeight() const { return mHeight; }

	private:
		static HANDLE consoleHandle;

		std::string mBoard{};
		
		int mWidth{};
		int mStrWidth{}; // mWidth+1
		int mHeight{};

		void removeRow(int row);
		void removeColumn(int col);
		void rectangularizeBoard();
		void trimBoard();
		void removeNewLines();
		void insertNewLines();
		void replaceChars(char oldChar, char newChar);

		char operator[](int i) const;
		char& operator[](int i);
	};


}