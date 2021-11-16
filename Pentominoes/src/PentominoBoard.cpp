// Author: Michael D'Agostino

#include <vector>
#include <string>
#include <iostream>
#include <cassert>
#include <map>

#include "Debug.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	HANDLE PentominoBoard::consoleHandle{ GetStdHandle(STD_OUTPUT_HANDLE) };

	void PentominoBoard::promptUserInputBoard()
	{
		std::cout << "Enter 1s (walls) and 0s (holes) across multiple lines to create \n"
			"the board to be solved, and enter a blank line when finished:\n";

		std::string line;
		std::getline(std::cin, line);
		mWidth = 0;
		mHeight = 0;
		mBoard.clear();

		while (line.length())
		{
			int len = line.length();

			// mWidth = longest line
			if (line.length() > mWidth)
			{
				mWidth = line.length();
			}

			// replace nonzeroes with 1
			for (char& c : line)
			{
				if (c != '0')
					c = '1';
			}

			// add the line to mBoard and continue
			mBoard += line + '\n';
			std::getline(std::cin, line);
			++mHeight;
		}
		mStrWidth = mWidth + 1;
#if DEBUG_LEVEL > 0
		std::cout << "mWidth: " << mWidth << "\n";
		std::cout << "mHeight: " << mHeight << "\n";
#endif
		rectangularizeBoard();
		trimBoard();
		removeNewLines();
		findSymmetry();     

		std::cout << "Board entered!\n";
	}

	void PentominoBoard::printLine(int row) const
	{
		int newLinesOffset{};
		if (mBoard.find('\n') != std::string::npos)
			newLinesOffset = 1;
		if (row < mHeight)
		{
			for (int i = row * (mWidth + newLinesOffset); i < row * (mWidth + newLinesOffset) + mWidth; i++)
			{
				if (mBoard[i] >= 'A')
				{
					// Change colors
					int textColor = ((mBoard[i] - 'A') % 14) + 1; // Skip 0 (black)
					if (textColor > 7) // Skip 8 (dark gray), reserved for 1s
						textColor++;
					int finalColor = (textColor << 4) | textColor;
					SetConsoleTextAttribute(consoleHandle, finalColor);


				}
				else if (mBoard[i] == '1')
				{
					SetConsoleTextAttribute(consoleHandle, 8 << 4);
				}
				else
				{
					// Restore b/w
					SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
				}

				// Print the char
				std::cout << mBoard[i];
			}
			// Restore b/w
			SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		}


	}

	
	void PentominoBoard::printBoard() const
	{

		for (int row = 0; row < mHeight; row++)
		{
			printLine(row);
			std::cout << "\n";
		}
		std::cout << "\n";
	}

	void PentominoBoard::reLetter()
	{
		std::map<char, char> charMap{}; // Will populate with chars on the board mapping to respective chars 
		char next = 'A'; // Next char to map to

		for (int i = 0; i < mBoard.size(); i++)
		{
			// 1s and 0s don't change
			if (mBoard[i] != '1' && mBoard[i] != '0')
			{
				auto key{ charMap.find(mBoard[i]) };
				if (key == charMap.end())
				{
					charMap[mBoard[i]] = next;
					mBoard[i] = next;
					next++;
				}
				else
				{
					mBoard[i] = charMap[mBoard[i]];
				}
			}			
		}
	}

	bool PentominoBoard::compareBoards(PentominoBoard board) const
	{
		if (board.mWidth != mWidth || board.mHeight != mHeight)
			return false;
		for (int row = 0; row < mHeight; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				char myVal = mBoard[row * mWidth + col];
				char otherVal = board[row * mWidth + col];
				if (myVal == '1' && otherVal != '1')
				{
					return false;
				}
			}
		}
		return true;
	}

	void PentominoBoard::rectangularizeBoard()
	{
		bool newLineCharReached{};
		int newLineNum{};
		for (int i = 0; i < mStrWidth * mHeight; i++)
		{
			int currentLineNum = i / mStrWidth; // current line # of the resulting rectangle
			if (!newLineCharReached && mBoard[i] == '\n')
			{
				newLineCharReached = true;
				newLineNum = currentLineNum;
			}
			if (currentLineNum != newLineNum)
			{
				// current line# has changed, reset flag
				newLineCharReached = false;
			}
			// insert padding if the new line character was reached 
			// and we're not at the end of the line
			if (newLineCharReached&& i% mStrWidth < mWidth)
				mBoard.insert(i, "1");
		}
#if DEBUG_LEVEL > 0
		std::cout << "Entry after rectangularizing:\n" << mBoard << "\n";
#endif
	}

	void PentominoBoard::removeRow(int row)
	{
		assert(row < mHeight);
		for (int i = 0; i < mStrWidth; i++)
		{
			mBoard.erase(row * mStrWidth, 1);
		}
		--mHeight;
#if DEBUG_LEVEL > 1
		std::cout << "Removed row " << row << "\n";
#endif
	}

	void PentominoBoard::removeColumn(int col)
	{
		assert(col < mWidth);
		--mWidth;
		--mStrWidth;

		for (int i = 0; i < mHeight; i++)
		{
			mBoard.erase(i * mStrWidth + col, 1);
		}


#if DEBUG_LEVEL > 1
		std::cout << "Removed column " << col << "\n";
#endif
	}

	void PentominoBoard::trimBoard()
	{

		// Trim top rows
		int i{ 0 };
		bool foundZero{};
		while (!foundZero && !mBoard.empty())
		{
			foundZero = mBoard[i] == '0';
			if (foundZero)
				break;
			if (i == mWidth - 1) // no zero found in the row
			{
				// Remove the row
				removeRow(0);
				i = 0;
				continue;
			}
			++i;

		}

		// Trim bottom rows
		i = 0;
		foundZero = false;
		while (!foundZero && !mBoard.empty())
		{
			foundZero = mBoard[i + mStrWidth * (mHeight - 1)] == '0';
			if (foundZero)
				break;
			if (i == mWidth - 1) // no zero found in the row
			{
				// Remove the row
				removeRow(mHeight - 1);
				i = 0;
				continue;
			}
			++i;
		}

		// Trim left columns
		i = 0;
		foundZero = false;
		while (!foundZero && !mBoard.empty())
		{
			foundZero = mBoard[mStrWidth * i] == '0';
			if (foundZero)
				break;
			if (i == mHeight - 1) // no zero found in the col
			{
				// Remove the col
				removeColumn(0);
				i = 0;
				continue;
			}
			++i;
		}

		// Trim right columns
		i = 0;
		foundZero = false;
		while (!foundZero && !mBoard.empty())
		{
			foundZero = mBoard[i * mStrWidth + mWidth - 1] == '0';
			if (foundZero)
				break;
			if (i == mHeight - 1) // no zero found in the col
			{
				// Remove the col
				removeColumn(mWidth - 1);
				i = 0;
				continue;
			}
			++i;
		}

#if DEBUG_LEVEL > 1
		std::cout << "Board after trimming:\n";
		printBoard();
#endif
	}

	void PentominoBoard::removeNewLines()
	{
		for (int row = 0; row < mHeight; row++)
		{
			mBoard.erase(row * mWidth + mWidth, 1);
		}
	}

	void PentominoBoard::insertNewLines()
	{
		for (int row = 0; row < mHeight; row++)
		{
			mBoard.insert(mStrWidth * row + mWidth, "\n");
		}
	}

	void PentominoBoard::replaceChars(char oldChar, char newChar)
	{
		int nextCharIndex = static_cast<int>(mBoard.find(oldChar));
		while (nextCharIndex != std::string::npos)
		{
			mBoard[nextCharIndex] = newChar;
			nextCharIndex = static_cast<int>(mBoard.find(oldChar));
		}
	}

	void PentominoBoard::findSymmetry()
	{
		// Set symmetry bits
		if (checkHorizontalSymmetry())
		{
			mSymmetry |= cMaskSymmetryHorizontal;
		}
		if (checkVerticalSymmetry())
		{
			mSymmetry |= cMaskSymmetryVertical;
		}
		if (check90Symmetry())
		{		
			mSymmetry |= cMaskSymmetry90;
		}
		if (check180Symmetry())
		{
			mSymmetry |= cMaskSymmetry180;
		}

#if DEBUG_LEVEL >0

		std::cout << "Horizontal symmetry: " << static_cast<bool>((mSymmetry & cMaskSymmetryHorizontal)) << "\n";
		std::cout << "Vertical symmetry: " << static_cast<bool>((mSymmetry & cMaskSymmetryVertical)) << "\n";
		std::cout << "90 symmetry: " << static_cast<bool>((mSymmetry & cMaskSymmetry90)) << "\n";
		std::cout << "180 symmetry: " << static_cast<bool>((mSymmetry & cMaskSymmetry180)) << "\n";
#endif
	}

	bool PentominoBoard::checkHorizontalSymmetry() const
	{
		if (mHeight == 1) // always horizontally symmetrical
			return true;
		for (int row = 0; row < mHeight / 2; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				// Check cell in upper half with corresponding lower half
				int topIndex = row * mWidth + col;
				int bottomIndex = mWidth * mHeight - ((row + 1) * mWidth) + col;
				if (mBoard[topIndex] != mBoard[bottomIndex])
					return false;
			}
		}
		return true;
	}

	bool PentominoBoard::checkVerticalSymmetry() const
	{
		if (mWidth == 1) // always horizontally symmetrical
			return true;
		for (int col = 0; col < mWidth / 2; col++)
		{
			for (int row = 0; row < mHeight; row++)
			{
				// Check cell in left half with corresponding right half
				int leftIndex{ row * mWidth + col };
				int rightIndex{ row * mWidth + (mWidth - (col + 1)) };
				if (mBoard[leftIndex] != mBoard[rightIndex])
					return false;
			}
		}
		return true;
	}

	bool PentominoBoard::check90Symmetry() const
	{
		if (mWidth != mHeight) // Must be a square board
			return false;
		for (int row = 0; row < mHeight; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				// Check each cell with its corresponding rotated counterpart
				int index0{ row * mWidth + col };
				int index90{ mWidth - (row + 1) + col * mWidth }; // swap rows and columns, start with the right column
				if (mBoard[index0] != mBoard[index90])
					return false;
			}
		}
		return true;
	}

	bool PentominoBoard::check180Symmetry() const
	{
		for (int i = 0; i < (mWidth * mHeight) / 2; i++)
		{
			// Check each cell with the corresponding rotated counterpart
			int index180 = (mWidth * mHeight - 1) - i; // 180 rotation is just scanning the board backwards
			if (mBoard[i] != mBoard[index180])
				return false;
		}
		return true;
	}

	PentominoBoard PentominoBoard::getRotated90() const
	{
		PentominoBoard rotated{ *this };
		rotated.mBoard.clear();
		for (int row = 0; row < mHeight; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				int index90{ mWidth - (row + 1) + col * mWidth };
				rotated.mBoard += mBoard[index90];
			}
		}
		return rotated;
	}

	PentominoBoard PentominoBoard::getRotated180() const
	{
		PentominoBoard rotated{ *this };
		rotated.mBoard.clear();

		for (int i = 0; i < (mWidth * mHeight) ; i++)
		{
			int index180 = (mWidth * mHeight - 1) - i; // 180 rotation is just scanning the board backwards
			rotated.mBoard += mBoard[index180];
		}
		return rotated;
	}

	PentominoBoard PentominoBoard::getHorizontalReflection() const
	{
		if (mHeight == 1) // Horizontal reflection is equivalent
			return *this;

		PentominoBoard reflected{ *this };
		reflected.mBoard.clear();

		for (int row = 0; row < mHeight; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				int bottomIndex = mWidth * mHeight - ((row + 1) * mWidth) + col;
				reflected.mBoard += mBoard[bottomIndex];
			}
		}
		return reflected;
	}

	PentominoBoard PentominoBoard::getVerticalReflection() const
	{
		if (mWidth == 1) // Vertical reflection is equivalent
			return *this;

		PentominoBoard reflected{ *this };
		reflected.mBoard.clear();

		
		for (int row = 0; row < mHeight; row++)
		{
			for (int col = 0; col < mWidth; col++)
			{
				int rightIndex{ row * mWidth + (mWidth - (col + 1)) };
				reflected.mBoard += mBoard[rightIndex];
			}
		}
		
		return reflected;
	}

	char PentominoBoard::operator[](int i) const
	{
		return mBoard[i];
	}

	char& PentominoBoard::operator[](int i)
	{
		return mBoard[i];
	}
}
