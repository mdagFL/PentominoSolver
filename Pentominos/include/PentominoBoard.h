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
		// Bitmasks for symmetry
		static constexpr int cMaskSymmetryNone		 { 0x00 };
		static constexpr int cMaskSymmetryHorizontal { 0x01 };
		static constexpr int cMaskSymmetryVertical	 { 0x02 };
		static constexpr int cMaskSymmetry90		 { 0x04 };
		static constexpr int cMaskSymmetry180		 { 0x08 };

		friend class PentominoSolver;
		PentominoBoard() {}; // prompt user to input
		PentominoBoard(const std::string& board)
			: mBoard{ board } {  }

		void promptUserInputBoard();
		void reLetter(); // Set lettering of the board to be as it would be if it was a generated solution
		void printBoard() const;
		void printLine(int row) const;	
		int getWidth() const { return mWidth; }
		int getHeight() const { return mHeight; }
		PentominoBoard getRotated90() const;
		PentominoBoard getRotated180() const;
		PentominoBoard getVerticalReflection() const;
		PentominoBoard getHorizontalReflection() const;

	private:
		static HANDLE consoleHandle;

		std::string mBoard{};
		
		int mWidth{};
		int mStrWidth{}; // mWidth+1
		int mHeight{};
		int mSymmetry{};


		void removeRow(int row);
		void removeColumn(int col);
		void rectangularizeBoard();
		void trimBoard();
		void removeNewLines();
		void insertNewLines();
		void findSymmetry();
		bool checkHorizontalSymmetry() const;
		bool checkVerticalSymmetry() const;
		bool check90Symmetry() const;
		bool check180Symmetry() const;
		void replaceChars(char oldChar, char newChar);

		char operator[](int i) const;
		char& operator[](int i);
	};


}