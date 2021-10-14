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
	bool PentominoSolver::wasFirstPiecePlaced{ false };

	// Takes a base piece and a bool array of size 8 and populates the bool array to correspond
	// with which piece orientations can be omitted as branches in the initial search tree
	// based on the symmetry of mBoard.
	void PentominoSolver::findTrivialOmissions(const BasePiece& base, int boardSymmetry, bool outTrivialOmissions[8])
	{
		// Some initial branches will be pruned to prevent trivial rotation/reflection solutions
		// based on the board's symmetry.

		// If a board has any reflective symmetry, we will omit reflections.
		// If a board has 180 degree symmetry, we will omit 180 and 270 degree rotations.
		// If a board has 90 degree symmetry, we will omit all rotations.

		int orientations = Pentomino::getNumberOfOrientations(static_cast<BasePiece>(base));

		// Rotational symmetry will only matter here if there is also at least one reflective symmetry.
		// This is because the only way to achieve a differently oriented piece in the top left
		// corner of the board from an initial piece in the top left corner is to combine a 90
		// rotation with a vertical reflection, or a 270 rotation with a horizontal reflection.
		if (boardSymmetry & PentominoBoard::cMaskSymmetry90
			&& (boardSymmetry & (PentominoBoard::cMaskSymmetryVertical | PentominoBoard::cMaskSymmetryHorizontal)))
		{
			int baseInt{ static_cast<int>(Pentomino::getBaseOrientation(base)) };
			for (int i = 0; i < orientations / 2; i++)
			{
				int nextOrientation{ static_cast<int>(Pentomino::getBaseOrientation(base)) + i };
				Pentomino nextPentomino(nextOrientation);
				int omitted{ static_cast<int>(nextPentomino.getRotated90().getVerticalReflection().getOrientation()) };
				// Note that rotating 270 and reflecting horizontal gives the same piece orientation, so it's not considered.
				// If it results in the same piece, nothing should be omitted.
				if (omitted != nextOrientation)
					outTrivialOmissions[omitted - baseInt] = true;
			}
		}
		/*
		if (orientations == 8)
		{
			if (boardSymmetry & (PentominoBoard::cMaskSymmetryHorizontal | PentominoBoard::cMaskSymmetryVertical))
			{
				// Omit reflections
				memset(outTrivialOmissions + 4, true, sizeof(bool) * 4);
			}
			if (boardSymmetry & (PentominoBoard::cMaskSymmetryHorizontal & PentominoBoard::cMaskSymmetryVertical))
			{
				// If both symmetries, omit all but 2
				memset(outTrivialOmissions + 2, true, sizeof(bool) * 6);
			}
		}
		else if (orientations == 4)
		{
			if (static_cast<BasePiece>(base) == BasePiece::Z)
			{
				// Z has 2 reflections and 2 rotations, so must be handled separately
				// The piece has 180 symmetry, so it makes no difference if the mBoard also has 180 symmetry
				if (boardSymmetry & (PentominoBoard::cMaskSymmetryHorizontal | PentominoBoard::cMaskSymmetryVertical))
				{
					// Omit reflections
					outTrivialOmissions[2] = true;
					outTrivialOmissions[3] = true;
				}
			}
			else // All other 4 orientation bases have 4 rotations but no reflections
			{
				// For all of these, 180 and 90 symmetry have the same effect:
				if (boardSymmetry & PentominoBoard::cMaskSymmetry90)
					memset(outTrivialOmissions + 2, true, sizeof(bool) * 2);
				if (boardSymmetry & PentominoBoard::cMaskSymmetry90)
					memset(outTrivialOmissions + 1, true, sizeof(bool) * 3);

				// However, a certain reflection results in a particular rotation, which varies somewhat
				// We want to omit the resultant rotations
				if (static_cast<BasePiece>(base) == BasePiece::T
					|| static_cast<BasePiece>(base) == BasePiece::U)
				{
					// For T and U, a reflection results in either the same piece, or a 180 rotation.
					if (boardSymmetry & PentominoBoard::cMaskSymmetryVertical)
						outTrivialOmissions[3] = true;
					if (boardSymmetry & PentominoBoard::cMaskSymmetryHorizontal)
						outTrivialOmissions[2] = true;
				}
				else if (static_cast<BasePiece>(base) == BasePiece::V
					|| static_cast<BasePiece>(base) == BasePiece::W)
				{
					// For V and W, a reflection results in either a 90 or 270 rotation.
					// With both, all orientations can be made.
					if (boardSymmetry & PentominoBoard::cMaskSymmetryHorizontal)
					{
						outTrivialOmissions[1] = true;
						outTrivialOmissions[3] = true;
					}
					if (boardSymmetry & PentominoBoard::cMaskSymmetryVertical)
					{
						outTrivialOmissions[2] = true;
						outTrivialOmissions[3] = true;
					}
				}
			}
		}
		*/
	}

	void PentominoSolver::findAllSolutions(const PentominoBoard& board, bool minimizeRepeats, bool multithreading)
	{
		using std::chrono::steady_clock;
		solutionsFound->clear();
		steady_clock::time_point begin(steady_clock::now());

		PentominoSolver solver(board, minimizeRepeats);
		std::cout << "0,0: " << solver.checkPointInFirstQuadrant(Point(0, 0)) << "\n";
		std::cout << "1,1: " << solver.checkPointInFirstQuadrant(Point(1, 1)) << "\n";
		std::cout << "2,2: " << solver.checkPointInFirstQuadrant(Point(2, 2)) << "\n";
		std::cout << "3,3: " << solver.checkPointInFirstQuadrant(Point(3, 3)) << "\n";
		std::cout << "7,1: " << solver.checkPointInFirstQuadrant(Point(7, 1)) << "\n";
		std::cout << "8,1: " << solver.checkPointInFirstQuadrant(Point(8, 1)) << "\n";
		std::cout << "9,2: " << solver.checkPointInFirstQuadrant(Point(9, 2)) << "\n";
		std::cout << "10,3: " << solver.checkPointInFirstQuadrant(Point(10, 3)) << "\n";

		//solutionsFound.reserve(board.mWidth * board.mHeight)
		int startIndex{ static_cast<int>(board.mBoard.find('0')) };

		std::vector<std::thread> threads{};
		int nextOrientation = 0;
		for (int base = 0; base < Pentomino::cTotalBasePieces; base++)
		{
			// For the first placement, place only pieces that do not result in trivial solutions.
			bool outTrivialOmissions[8]{};
			int orientations = Pentomino::getNumberOfOrientations(static_cast<BasePiece>(base));
			
			solver.findTrivialOmissions(static_cast<BasePiece>(base), solver.mBoard.mSymmetry, outTrivialOmissions);

			for (int i = 0; i < orientations; i++)
			{
				if (!outTrivialOmissions[i])
				{

					Pentomino startPiece(static_cast<PieceOrientation>(nextOrientation));
					int x = startIndex % board.mWidth - startPiece.getXOffset();
					int y = startIndex / board.mWidth;
					if (multithreading)
					{
						if (minimizeRepeats && solver.checkPieceAvailable(startPiece))
							threads.emplace_back(std::thread(&PentominoSolver::searchSimpleMinimizeRepeats,
								PentominoSolver(solver), startPiece, Point(x, y), solver.mBoard.mSymmetry, 0));
						else
							threads.emplace_back(std::thread(&PentominoSolver::searchSimpleWithRepeats,
								PentominoSolver(solver), startPiece, Point(x, y), 0));
					}
					else
						if (minimizeRepeats && solver.checkPieceAvailable(startPiece))
							solver.searchSimpleMinimizeRepeats(startPiece, Point(x, y), solver.mBoard.mSymmetry, 1);
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

	void PentominoSolver::checkForTrivialSolutions()
	{

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
	void PentominoSolver::searchSimpleMinimizeRepeats(const Pentomino& piece, const Point& pos, int symmetry, int depth)
	{
		bool placed = tryPushPentomino(piece, pos);
		if (placed)
		{
			lock.lock();
			if (!wasFirstPiecePlaced)
			{
				// The first piece to be successfully placed from any branch will be used as an "anchor"
				// for boards with rotational symmetries. The anchor must be placed in the
				// top half / first quadrant for all branches to prevent trivial solutions.
				wasFirstPiecePlaced = true;
				//firstBase = piece.getBase();
				firstBase = BasePiece::X;
			}
			lock.unlock();

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

				// Cases where a trivial reflection/rotation of a board could result
				// in pieces that are the same orientation in a trivial solution
				// require some additional pruning at higher depth until a different
				// piece is reached.
				int nextSymmetry{ symmetry };
				PieceOrientation lastOrientation = mPlacedPentominoes.back().pentomino.getOrientation();
				nextSymmetry = PentominoBoard::cMaskSymmetryNone;
				
				if (nextSymmetry != PentominoBoard::cMaskSymmetryNone)
				{
					if (lastOrientation == PieceOrientation::U0
						|| lastOrientation == PieceOrientation::U2
						|| lastOrientation == PieceOrientation::T0
						|| lastOrientation == PieceOrientation::T2)
						nextSymmetry &= PentominoBoard::cMaskSymmetryVertical;
					else if (lastOrientation == PieceOrientation::U1
						|| lastOrientation == PieceOrientation::U3
						|| lastOrientation == PieceOrientation::T1
						|| lastOrientation == PieceOrientation::T3)
						nextSymmetry &= PentominoBoard::cMaskSymmetryHorizontal;
					else if (lastOrientation != PieceOrientation::X0)
						nextSymmetry &= PentominoBoard::cMaskSymmetryNone;
					else
						nextSymmetry = PentominoBoard::cMaskSymmetryNone;
				}
				if (mBoard.mSymmetry & PentominoBoard::cMaskSymmetry90
					&& mBoard.mSymmetry & (PentominoBoard::cMaskSymmetryHorizontal | PentominoBoard::cMaskSymmetryVertical))
				{
					if (Pentomino(lastOrientation).getRotated90().getVerticalReflection().getOrientation() == lastOrientation)
					{
						// Restrict the next placed piece to avoid trivial solutions
						nextSymmetry = mBoard.mSymmetry;
					}
				}
				//nextSymmetry |= (mBoard.mSymmetry & (PentominoBoard::cMaskSymmetry180));

					

				int nextOrientation = 0;

				int nextX{ nextZeroIndex % mBoard.mWidth };
				int nextY{ nextZeroIndex / mBoard.mWidth };

				
				// Check if the anchor can no longer be placed where it must be placed
				if (mBoard.mSymmetry & (PentominoBoard::cMaskSymmetry180 | PentominoBoard::cMaskSymmetry90)
					&& checkPieceAvailable(Pentomino(Pentomino::getBaseOrientation(firstBase))) && nextY > mBoard.mHeight / 2)
				{
					// Bad branch: cut it and backtrack
#if DEBUG_LEVEL > 1
					std::cout << "Bad branch cut!\n";
#endif
					popPentomino();
					return;
				}
				

				for (int base = 0; base < Pentomino::cTotalBasePieces; base++)
				{
					int orientations = Pentomino::getNumberOfOrientations(static_cast<BasePiece>(base));
					bool nextOmissions[8]{};
					
					if (nextSymmetry)
						PentominoSolver::findTrivialOmissions(static_cast<BasePiece>(base), nextSymmetry, nextOmissions);
					
					for (int i = 0; i < orientations; i++)
					{
						if (!nextOmissions[i])
						{
							Pentomino nextPiece(static_cast<PieceOrientation>(nextOrientation));
							if (checkPieceAvailable(nextPiece))
							{
								
								Point nextPos(nextX - nextPiece.getXOffset(), nextY);
								// If the board has rotational symmetries, need to check the anchor piece
								
								if (mBoard.mSymmetry && nextPiece.getBase() == firstBase)
								{
									// If trying to place the anchor piece outside of its respective area, skip it
									Point centerOnBoard = Point(nextPiece.getCenter().x + nextPos.x, nextPiece.getCenter().y + nextPos.y);
									if ((mBoard.mSymmetry == mBoard.cMaskSymmetry180)
										&& !checkPointInTopHalf(centerOnBoard))
										continue; 
									else if (mBoard.mSymmetry != PentominoBoard::cMaskSymmetryHorizontal
										&& mBoard.mSymmetry != PentominoBoard::cMaskSymmetryVertical
										&& !checkPointInFirstQuadrant(centerOnBoard))
									{
										continue;
									}
										
								}
								
								searchSimpleMinimizeRepeats(nextPiece, nextPos, nextSymmetry, depth + 1);
							}
							nextOrientation++;
						}
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
		lock.lock();
		bool placed = tryPushPentomino(piece, pos);
		lock.unlock();
		if (placed)
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
		mPiecesAvailable[static_cast<int>(piece.getBase())] = available;
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
		BasePiece base{ piece.getBase() };
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

	bool PentominoSolver::checkPointInFirstQuadrant(const Point& pos) const
	{
		return (pos.x < (mBoard.mWidth / 2) + (mBoard.mWidth % 2) 
			&& pos.y < (mBoard.mHeight / 2) + (mBoard.mHeight % 2));
	}

	bool PentominoSolver::checkPointInTopHalf(const Point& pos) const
	{
		// If the board has an odd number of rows, want to include the left half of 
		// the center row in the top
		return (pos.y * mBoard.mWidth + pos.x < mBoard.mWidth* mBoard.mHeight / 2 + (mBoard.mHeight % 2));
	}
}
