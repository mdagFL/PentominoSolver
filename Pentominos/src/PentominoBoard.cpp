#include <vector>
#include <string>
#include <iostream>
#include <cassert>

#include "Debug.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	void PentominoBoard::promptUserInputBoard()
	{
		std::cout << "Enter the board to be solved:\n";
		std::string line;
		std::getline(std::cin, line);
		while (line[0])
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
		std::cout << "Entry after filtering 0s:\n";
		printBoard();
		std::cout << "mWidth: " << mWidth << "\n";
		std::cout << "mHeight: " << mHeight << "\n";
#endif
		rectangularizeBoard();
		trimBoard();
	}

	void PentominoBoard::printBoard() const
	{
		std::cout << mBoard << "\n";
	}

	// Insert padding to the board to keep new lines a fixed width apart
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
			if (newLineCharReached && i % mStrWidth < mWidth)
				mBoard.insert(i, "1");
		}
#if DEBUG_LEVEL > 0
		std::cout << "Entry after rectangularizing:\n" << mBoard << "\n";
#endif
	}

	// Precondition: 0 <= row < mHeight
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

	// Precondition: 0 <= col < mWidth
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

	// Remove any bordering rows or columns that are only 1s
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
			if (i == mWidth-1) // no zero found in the row
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
			foundZero = mBoard[i + mStrWidth * (mHeight-1)] == '0';
			if (foundZero)
				break;
			if (i == mWidth-1) // no zero found in the row
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
			if (i == mHeight-1) // no zero found in the col
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
			foundZero = mBoard[i * mStrWidth + mWidth-1] == '0';
			if (foundZero)
				break;	
			if (i == mHeight - 1) // no zero found in the col
			{
				// Remove the col
				removeColumn(mWidth-1);
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

	
}
