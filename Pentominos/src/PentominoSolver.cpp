#include <cassert>
#include <iostream>
#include <thread>


#include "PentominoSolver.h"
#include "Debug.h"


namespace Pentominoes
{
	std::mutex PentominoSolver::lock{};
	std::vector<PentominoSolver>* PentominoSolver::solutionsFound = new std::vector<PentominoSolver>;
	std::chrono::duration<double> PentominoSolver::durationLastSolution{};


	PentominoSolver::PentominoSolver(const PentominoBoard& board, bool minimizeRepeats) 
		: mBoard{ board }, mMinimizeRepeats{ minimizeRepeats }
	{
		
		if (mMinimizeRepeats)
		{
			mPiecesAvailable = new bool[Pentomino::cTotalOrientations];
			resetAvailable();
		}
	}

	PentominoSolver::PentominoSolver(const PentominoSolver& original) 
		: mBoard{ original.mBoard }, mMinimizeRepeats{ original.mMinimizeRepeats }
	{
		if (mMinimizeRepeats)
		{
			mPiecesAvailable = new bool[Pentomino::cTotalOrientations];
			for (int i = 0; i < Pentomino::cTotalOrientations; i++)
				mPiecesAvailable[i] = original.mPiecesAvailable[i];
		}
	}

	PentominoSolver::~PentominoSolver()
	{		
		if (mMinimizeRepeats)
			delete[] mPiecesAvailable;;
	}

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

			// If minimizing repeats, mark this piece orientation as unavailable
			if (mMinimizeRepeats)
				mPiecesAvailable[static_cast<int>(piece)] = false;

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

		PlacedPentomino piece{ mPlacedPentominoes.back() };
		mPlacedPentominoes.pop_back();
		--mNextSymbol;

		// Clean the piece off the board
		mBoard.replaceChars(mNextSymbol, '0');

		if (mMinimizeRepeats)
			mPiecesAvailable[static_cast<int>(piece.pentomino)] = true;
		
		return piece;
	}

	void PentominoSolver::findAllSolutions(const PentominoBoard& board, bool minimizeRepeats, bool multithreading)
	{
		using std::chrono::steady_clock;
		solutionsFound->clear();
		steady_clock::time_point begin(steady_clock::now());

		PentominoSolver solver(board, minimizeRepeats);
		//solutionsFound.reserve(board.mWidth * board.mHeight)
		int startIndex{ static_cast<int>(board.mBoard.find('0')) };

		std::vector<std::thread> threads {};
		for (int i = 0; i < Pentomino::cTotalOrientations; i++)
		{
			Pentomino startPiece(static_cast<PieceOrientation>(i));
			int x = startIndex % board.mWidth - startPiece.GetXOffset();
			int y = startIndex / board.mWidth;
			if (multithreading)
			{
				threads.emplace_back(std::thread(&PentominoSolver::searchSimple, PentominoSolver(solver), startPiece, Point(x,y), 0));
			}
			else
				solver.searchSimple(startPiece, Point(x, y), 1);
		}

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
		threads.clear();
		std::cout << "\nTotal solutions: " << solutionsFound->size() << "\n";
		steady_clock::time_point end(steady_clock::now());
		durationLastSolution = std::chrono::duration_cast<std::chrono::duration<double >> (end - begin);
		std::cout << "Time elapsed: " << durationLastSolution.count();
		
	}


	// Recursive backtracking function to find and print all solutions
	// Takes an initial piece and position to continue searching from.
	void PentominoSolver::searchSimple(const Pentomino& piece, const Point& pos, int depth)
	{
		if (tryPushPentomino(piece, pos))
		{
			// Find next zero, used to calculate where to place the next piece
			int nextZeroIndex{ static_cast<int>(mBoard.mBoard.find('0')) };
			if (nextZeroIndex == std::string::npos)
			{

				// Board is solved, add the solution
				lock.lock();
				solutionsFound->emplace_back(*this);
				lock.unlock();
#if DEBUG_LEVEL > 1
				std::cout << "Solution found!\n";
#endif
				// Leaf node reached, backtrack
				popPentomino();
				return;
			}
			else if (isPossibleSolution())
			{
				// Next branches consist of all available fitting pieces in the next available spot		
				if (mMinimizeRepeats)
				{
					if (checkNoPiecesAvailable())
						resetAvailable();

					for (int i = 0; i < Pentomino::cTotalOrientations; i++)
					{
						if (mPiecesAvailable[i])
						{
							Pentomino nextPiece(static_cast<PieceOrientation>(i));
							int x{ nextZeroIndex % mBoard.mWidth };
							int y{ nextZeroIndex / mBoard.mWidth };
							Point nextPos(x - nextPiece.GetXOffset(), y); // 
							searchSimple(nextPiece, nextPos, depth + 1);
						}
					}
				}
				
				else
				{
					for (int i = 0; i < Pentomino::cTotalOrientations; i++)
					{
						Pentomino nextPiece(static_cast<PieceOrientation>(i));
						int x{ nextZeroIndex % mBoard.mWidth };
						int y{ nextZeroIndex / mBoard.mWidth };
						Point nextPos(x - nextPiece.GetXOffset(), y);
						searchSimple(nextPiece, nextPos, depth + 1);
					}
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

	// Precondition: mMinimizeRepeats == true
	void PentominoSolver::resetAvailable()
	{
		assert(mMinimizeRepeats == true);
		for (int i = 0; i < Pentomino::cTotalOrientations; i++)
			mPiecesAvailable[i] = true;
	}

	bool PentominoSolver::checkNoPiecesAvailable() const
	{
		for (int i = 0; i < Pentomino::cTotalOrientations; i++)
		{
			if (mPiecesAvailable[i])
			{
				return false;
			}
		}
		return true;
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
