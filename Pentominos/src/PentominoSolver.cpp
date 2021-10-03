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


	void PentominoSolver::findAllSolutions(const PentominoBoard& board, bool minimizeRepeats, bool multithreading)
	{
		using std::chrono::steady_clock;
		solutionsFound->clear();
		steady_clock::time_point begin(steady_clock::now());

		PentominoSolver solver(board, minimizeRepeats);
		//solutionsFound.reserve(board.mWidth * board.mHeight)
		int startIndex{ static_cast<int>(board.mBoard.find('0')) };

		std::vector<std::thread> threads{};
		int nextOrientation = 0;
		for (int base = 0; base < Pentomino::cTotalBasePieces; base++)
		{
			
			// Some initial branches will be pruned to prevent trivial rotation/reflection solutions
			// based on the board's symmetry.

			// If a board has any reflective symmetry, we will omit reflections.
			// If a board has 180 degree symmetry, we will omit 180 and 270 degree rotations.
			// If a board has 90 degree symmetry, we will omit all rotations.

			bool nextTrivialOmissions[8]{};
			int orientations = Pentomino::getNumberOfOrientations(static_cast<OrientationBase>(base));
			
			if (orientations == 8)
			{
				if (board.mSymmetry & (PentominoBoard::cMaskSymmetryHorizontal | PentominoBoard::cMaskSymmetryVertical))
				{
					// Omit reflections
					nextTrivialOmissions[4] = true;
					nextTrivialOmissions[5] = true;
					nextTrivialOmissions[6] = true;
					nextTrivialOmissions[7] = true;
				}
				if (board.mSymmetry & PentominoBoard::cMaskSymmetry180)
				{
					// Omit 180 and 270 for regular and reflection
					nextTrivialOmissions[2] = true;
					nextTrivialOmissions[3] = true;
					nextTrivialOmissions[6] = true;
					nextTrivialOmissions[7] = true;
				}
				if (board.mSymmetry & PentominoBoard::cMaskSymmetry90)
				{
					// Omit all but the base and its reflection
					memset(nextTrivialOmissions, true, sizeof(bool) * 8); // set all to true
					nextTrivialOmissions[0] = false;
					nextTrivialOmissions[4] = false;
				}
			}
			else if (orientations == 4)
			{
				memset(nextTrivialOmissions + 4, true, sizeof(bool) * 4);
				if (static_cast<OrientationBase>(base) == OrientationBase::Z)
				{
					// Z has 2 reflections and 2 rotations, so must be handled separately
					// The piece has 180 symmetry, so it makes no difference if the board also has 180 symmetry
					if (board.mSymmetry & (PentominoBoard::cMaskSymmetryHorizontal | PentominoBoard::cMaskSymmetryVertical))
					{
						// Omit reflections
						nextTrivialOmissions[2] = true;
						nextTrivialOmissions[3] = true;
					}
					if (board.mSymmetry & PentominoBoard::cMaskSymmetry90)
					{
						// Omit all but the base and its reflection
						nextTrivialOmissions[1] = true;
						nextTrivialOmissions[3] = true;
					}
				}
				else // All other 4 orientation bases have 4 rotations but no reflections
				{
					// For these pieces, a certain reflection results in a particular rotation, which varies for each piece
					// We want to omit the resultant rotations
					if (static_cast<OrientationBase>(base) == OrientationBase::T 
						|| static_cast<OrientationBase>(base) == OrientationBase::U)
					{
						// For T and U, a reflection results in either the same piece, or a 180 rotation.
						nextTrivialOmissions[2] = true;
					}
					else if (static_cast<OrientationBase>(base) == OrientationBase::U)
					{
						
					}
					else if (static_cast<OrientationBase>(base) == OrientationBase::V)
					{

					}
					else if (static_cast<OrientationBase>(base) == OrientationBase::W)
					{
					}
					if (board.mSymmetry & PentominoBoard::cMaskSymmetry180)
					{
						// Omit 180 and 270 for regular and reflection
						nextTrivialOmissions[2] = true;
						nextTrivialOmissions[3] = true;
					}
					if (board.mSymmetry & PentominoBoard::cMaskSymmetry90)
					{
						// Omit all but the base and its reflection
						memset(nextTrivialOmissions+1, true, sizeof(bool) * 3); // set 1-3 to true
					}
				}
			}
			else if (orientations == 2)
			{
				// I piece; only 90 rotations are meaningful
				if (board.mSymmetry & PentominoBoard::cMaskSymmetry90)
				{
					nextTrivialOmissions[1] = true;
				}
			}
			else // X piece, only 1 orientation
			{
				// Set 1-7 to true
				memset(nextTrivialOmissions + 1, true, sizeof(bool) * 7); //
			}


			for (int i = 0; i < orientations; i++)
			{
				if (!nextTrivialOmissions[i])
				{
					Pentomino startPiece(static_cast<PieceOrientation>(nextOrientation));
					int x = startIndex % board.mWidth - startPiece.getXOffset();
					int y = startIndex / board.mWidth;
					if (multithreading)
					{
						if (minimizeRepeats && solver.checkPieceAvailable(startPiece))
							threads.emplace_back(std::thread(&PentominoSolver::searchSimpleMinimizeRepeats, PentominoSolver(solver), startPiece, Point(x, y), 0));
						else
							threads.emplace_back(std::thread(&PentominoSolver::searchSimpleWithRepeats, PentominoSolver(solver), startPiece, Point(x, y), 0));
					}
					else
						if (minimizeRepeats && solver.checkPieceAvailable(startPiece))
							solver.searchSimpleMinimizeRepeats(startPiece, Point(x, y), 1);
						else
							solver.searchSimpleWithRepeats(startPiece, Point(x, y), 1);
				}
				nextOrientation++;
			}
			
		}

		for (int i = 0; i < threads.size(); i++)
		{
			threads[i].join();
		}
		threads.clear();
		std::cout << "\nTotal solutions: " << solutionsFound->size() << "\n";
		steady_clock::time_point end(steady_clock::now());
		durationLastSolution = std::chrono::duration_cast<std::chrono::duration<double >> (end - begin);
		std::cout << "Time elapsed: " << durationLastSolution.count() << "\n";

	}

	void PentominoSolver::printSolutions()
	{
		

		if (solutionsFound->size() > 0)
		{
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
			int consoleColumns{ csbi.srWindow.Right - csbi.srWindow.Left + 1 };

			int height{ solutionsFound->at(0).mBoard.mHeight };
			int width{ solutionsFound->at(0).mBoard.mWidth };
			int solutionsPerRow{ consoleColumns / (width + 1) };
			
			// Iterate through all solutions in batches of size solutionsPerRow
			for (int i = 0; i < solutionsFound->size(); i+= solutionsPerRow)
			{
				// If last row, only access remaining solutions
				int solutionsThisRow = (i + solutionsPerRow > solutionsFound->size())? solutionsFound->size() % solutionsPerRow : solutionsPerRow;

				// Print each batch line height times
				for (int j = 0; j < height; j++)
				{
					// Iterate through the batch
					for (int k = 0; k < solutionsThisRow; k++)
					{
						solutionsFound->at(i + k).mBoard.printLine(j);
						std::cout << " ";
					}
				
				std::cout << "\n";
				}
				
				std::cout << "\n";
			}
		}
		
	}


	PentominoSolver::PentominoSolver(const PentominoBoard& board, bool minimizeRepeats) 
		: mBoard{ board }, mMinimizeRepeats{ minimizeRepeats }, mPiecesAvailable{ nullptr }
	{
		if (mMinimizeRepeats)
		{
			mPiecesAvailable = new bool[Pentomino::cTotalBasePieces];
			resetAvailable();
		}
	}

	PentominoSolver::PentominoSolver(const PentominoSolver& original)
		: mBoard{ original.mBoard }, mMinimizeRepeats{ original.mMinimizeRepeats }, mPiecesAvailable{ nullptr }
	{
		if (mMinimizeRepeats)
		{
			mPiecesAvailable = new bool[Pentomino::cTotalBasePieces];
			for (int i = 0; i < Pentomino::cTotalBasePieces; i++)
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
				setAvailable(piece, false);

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
			setAvailable(piece.pentomino, true);
		
		return piece;
	}

	


	// Recursive backtracking function to find and print all solutions
	// Takes an initial piece and position to continue searching from.
	void PentominoSolver::searchSimpleMinimizeRepeats(const Pentomino& piece, const Point& pos, int depth)
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
				if (checkNoPiecesAvailable())
					resetAvailable();

				for (int i = 0; i < Pentomino::cTotalOrientations; i++)
				{	
					Pentomino nextPiece(static_cast<PieceOrientation>(i));
					if (checkPieceAvailable(nextPiece))
					{
						int x{ nextZeroIndex % mBoard.mWidth };
						int y{ nextZeroIndex / mBoard.mWidth };
						Point nextPos(x - nextPiece.getXOffset(), y); // 
						searchSimpleMinimizeRepeats(nextPiece, nextPos, depth + 1);
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

	void PentominoSolver::searchSimpleWithRepeats(const Pentomino& piece, const Point& pos, int depth)
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
				// Next branches consist of all fitting pieces in the next available spot		
				for (int i = 0; i < Pentomino::cTotalOrientations; i++)
				{
					Pentomino nextPiece(static_cast<PieceOrientation>(i));
					int x{ nextZeroIndex % mBoard.mWidth };
					int y{ nextZeroIndex / mBoard.mWidth };
					Point nextPos(x - nextPiece.getXOffset(), y);
					searchSimpleWithRepeats(nextPiece, nextPos, depth + 1);
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
		assert(mMinimizeRepeats);
		for (int i = 0; i < Pentomino::cTotalBasePieces; i++)
			mPiecesAvailable[i] = true;
	}

	// Precondition: mMinimizeRepeats == true
	void PentominoSolver::setAvailable(const Pentomino& piece, bool available)
	{
		assert(mMinimizeRepeats);
		mPiecesAvailable[static_cast<int>(piece.getBasePiece())] = available;
	}
	bool PentominoSolver::checkNoPiecesAvailable() const
	{
		for (int i = 0; i < Pentomino::cTotalBasePieces; i++)
		{
			if (mPiecesAvailable[i])
			{
				return false;
			}
		}
		return true;
	}

	// Precondition: minimizeRepeats == true
	bool PentominoSolver::checkPieceAvailable(const Pentomino& piece) const
	{
		assert(mMinimizeRepeats == true);
		OrientationBase base{ piece.getBasePiece() };
		return (mPiecesAvailable[static_cast<int>(base)]);
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
