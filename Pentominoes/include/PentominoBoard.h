// Author: Michael D'Agostino

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
	
	// Class for representing a Pentomino puzzle board with any number of PlacedPentominoes.
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
		PentominoBoard() {};
		PentominoBoard(const std::string& board)
			: mBoard{ board } {  }
		
		// Prompt user to input the board and parse the user input to this board.
		void promptUserInputBoard();

		// Set lettering of mBoard as it would be if it was a naturally generated solution from PentominoSolver
		void reLetter(); 

		// Print mBoard to the console
		// Precondition: rectangularizeBoard() has been called
		void printBoard() const;
		
		// Compare this board with another board, returns true if the other board is part of the same problem/solution set.
		bool compareBoards(PentominoBoard board) const;

		// Get width of the board
		int getWidth() const { return mWidth; }

		// Get height of the board
		int getHeight() const { return mHeight; }

		// Returns a PentominoBoard that's a 90 degree rotation of this board
		// Precondition: New lines have been removed
		// Precondition: Must be a square board
		PentominoBoard getRotated90() const;

		// Returns a PentominoBoard that's a 180 degree rotation of this board
		PentominoBoard getRotated180() const;

		// Returns a PentominoBoard that's a reflection over the central vertical axis
		PentominoBoard getVerticalReflection() const;

		// Returns a PentominoBoard that's a reflection over the central horizontal axis
		PentominoBoard getHorizontalReflection() const;

	private:
		static HANDLE consoleHandle; // winAPI object used for printing different colors to the console

		std::string mBoard{};
		
		int mWidth{};
		int mStrWidth{}; // mWidth+1
		int mHeight{};
		int mSymmetry{};


		// Print a specified row of the board to the console
		void printLine(int row) const;

		// Remove a specified row from the board
		// Precondition: 0 <= row < mHeight
		void removeRow(int row);

		// Remove a specified column from the board
		// Precondition: 0 <= col < mWidth
		void removeColumn(int col);

		// Pad the board with 1s to new lines a fixed width apart
		// Precondition: mBoard contains new lines at the end of each row
		void rectangularizeBoard();

		// Remove bordering rows or columns that only contain 1s
		void trimBoard();

		// Remove new line characters from the board
		void removeNewLines();

		// Insert new line characters at the end of each row
		void insertNewLines();

		// Find and set board symmetries
		void findSymmetry();

		// Returns true if the board has horizontal symmetry
		bool checkHorizontalSymmetry() const;

		// Returns true if the board has vertical symmetry
		bool checkVerticalSymmetry() const;

		// Returns true if the board has 90 degree rotational symmetry
		bool check90Symmetry() const;

		// Returns true if the board has 180 degree rotational symmetry
		bool check180Symmetry() const;

		// Replace all instances of oldChar on mBoard with newChar 
		void replaceChars(char oldChar, char newChar);

		char operator[](int i) const;
		char& operator[](int i);
	};


}