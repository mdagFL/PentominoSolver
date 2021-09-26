#include <cassert>
#include <iostream>

#include "PentominoSolver.h"
#include "Debug.h"


namespace Pentominoes
{
	int PentominoSolver::numSolutions{ 0 };

	// If legal placement, returns true and places the piece on the board.
	bool PentominoSolver::tryPushPentomino(const Pentomino& piece, const Point& pos)
	{
		// Check if the piece is in the bounds of the board
		if (pos.x >= 0
			&& pos.y >= 0
			&& pos.x + piece.getRectangleWidth() <= mBoard.mWidth
			&& pos.y + piece.getRectangleHeight() <= mBoard.mHeight)
		{
			
			// Check if the piece overlaps with anything on the board
			std::string pieceStr = piece.getDataString();
			Pentomino::removeNewLines(pieceStr);

			std::string boardStringCopy = mBoard.mBoard;
			for (int i = 0; i < piece.getRectangleHeight(); i++)
			{
				for (int j = 0; j < piece.getRectangleWidth(); j++)
				{
					int piecePosIndex = piece.getRectangleWidth() * i + j;
					int boardPosIndex = pos.x + pos.y * mBoard.mWidth +
						i * mBoard.mWidth + j;

					if (pieceStr[piecePosIndex] == '1')
					{
						if (mBoard[boardPosIndex] != '0')
						{
#if DEBUG_LEVEL > 1
							std::cout << "Failed to place piece " + piece.getLabelString() << " at (" << pos.x << ", " << pos.y << ")\n";
#endif
							return false;
						}
						else
						{
							// 'place' the cell on the copy board
							boardStringCopy[boardPosIndex] = mNextSymbol;
						}
					}
					
					

				}
			}

			// Piece fits, push it
			mPlacedPentominoes.push_back(PlacedPentomino{ piece, pos, mNextSymbol++ });

			// Update the board to the copy
			mBoard.mBoard = boardStringCopy;

#if DEBUG_LEVEL > 1
			std::cout << "Successfully placed piece " + piece.getLabelString() << " at (" << pos.x << ", " << pos.y << ")\n";
			std::cout << "New board:\n";
			mBoard.printBoard();
#endif
			return true;
		}
		else
		{
#if DEBUG_LEVEL > 1
			std::cout << "Failed to place piece " + piece.getLabelString() << " at (" << pos.x << ", " << pos.y << ")\n";
#endif
			return false;
		}
		
		
	}

	// Precondition: At least 1 placed pentomino on the board
	// Pop the last placed pentomino
	PlacedPentomino PentominoSolver::popPentomino()
	{
		assert(mPlacedPentominoes.size() > 0);
		PlacedPentomino piece = mPlacedPentominoes.back();
		mPlacedPentominoes.pop_back();
		--mNextSymbol;

		// Clean the piece off the board
		mBoard.replaceChars(mNextSymbol, '0');

		
		return piece;
	}

	void PentominoSolver::findAllSolutions(const PentominoBoard& board)
	{
		// TO DO: return a vector of solutions
		numSolutions = 0;
		PentominoSolver solver(board);

		int startIndex{ static_cast<int>(board.mBoard.find('0')) };
		for (int i = Pentomino::cTotalOrientations-1; i >= 0; i--)
		{
			Pentomino startPiece(static_cast<PieceOrientation>(i));
			int x = startIndex % board.mWidth - startPiece.GetXOffset();
			int y = startIndex / board.mWidth;
			solver.searchSimple(startPiece, Point(x, y), 1);
		}

		std::cout << "\nTotal solutions: " << numSolutions << "\n";
	}


	// Recursive backtracking function to find and print all solutions
	// Takes an initial piece and position to continue searching from.
	void PentominoSolver::searchSimple(const Pentomino& piece, const Point& pos, int depth)
	{
		if (tryPushPentomino(piece, pos))
		{
			int nextZeroIndex{ static_cast<int>(mBoard.mBoard.find('0')) };
			if (nextZeroIndex == std::string::npos)
			{
#if DEBUG_LEVEL > 1
				std::cout << "Solution found!\n";
#endif
				// Board is solved
				mBoard.printBoard();
				numSolutions++;
				// Store solution?
				// Leaf node reached, backtrack
				popPentomino();
				return;
			}
			else if (isPossibleSolution())
			{
				// Next branches consist of all fitting pieces in the next available spot
				for (int i = Pentomino::cTotalOrientations-1; i >= 0; i--)
				{
					Pentomino nextPiece(static_cast<PieceOrientation>(i));
					int x{ nextZeroIndex % mBoard.mWidth };
					int y{ nextZeroIndex / mBoard.mWidth };
					Point nextPos(x - nextPiece.GetXOffset(), y);
					searchSimple(nextPiece, nextPos, depth+1);
				}
				// All branches at this level explored, backtrack
#if DEBUG_LEVEL > 1
				std::cout << "All branches explored at depth = " << depth << "!\n";
#endif
				popPentomino();
				return;
			}
			else
			{
				// Bad branch: cut it and backtrack
#if DEBUG_LEVEL > 1
				std::cout << "Bad branch cut!\n";
#endif
				popPentomino();
				return;
			}
		}
		else // Not a branch; piece doesn't fit in this spot
			return;
	}

	bool PentominoSolver::isPossibleSolution()
	{
		std::vector<int> areas(findHoleAreas());

		for (int area : areas)
		{
			if (area % 5 != 0)
				return false;
		}
		return true;
	}

	std::vector<int> PentominoSolver::findHoleAreas()
	{
		std::vector<int> areas;
		for (int i = 0; i < mBoard.mHeight; i++)
		{
			for (int j = 0; j < mBoard.mWidth; j++)
			{
				if (mBoard[i * mBoard.mWidth + j] == '0')
					areas.push_back(findHoleArea(Point(j, i)));
			}
		}

		// Unmark the holes on the board
		mBoard.replaceChars('m', '0');
			
		return areas;
	}

	// Recursive function to sum all the adjacent holes in a given hole on the board.
	int PentominoSolver::findHoleArea(const Point& posHole)
	{
		int boardIndex = posHole.x + posHole.y * mBoard.mWidth;
		// Check if the point is in bounds of the board, and is actually a hole
		if (posHole.x >= 0 && posHole.x < mBoard.mWidth
			&& posHole.y >= 0 && posHole.y < mBoard.mHeight
			&& mBoard[boardIndex] == '0')
		{
			mBoard[boardIndex] = 'm';
			return 1 +
				findHoleArea(Point(posHole.x, posHole.y - 1)) +
				findHoleArea(Point(posHole.x + 1, posHole.y)) +
				findHoleArea(Point(posHole.x, posHole.y + 1)) +
				findHoleArea(Point(posHole.x - 1, posHole.y));
		}
		else
			return 0;
	}

}
