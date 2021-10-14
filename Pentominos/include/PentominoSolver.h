#pragma once
#include <vector>
#include <chrono>
#include <mutex>
#include <Windows.h>

#include "Pentomino.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	struct PlacedPentomino
	{
		Pentomino pentomino;
		Point position;
		char symbol;
		PlacedPentomino(const Pentomino& a_pentomino, Point a_position, char a_symbol)
			: pentomino{ a_pentomino }, position{ a_position }, symbol{ a_symbol } {}
	};

	class PentominoSolver 
	{
	public:
		static void findAllSolutions(const PentominoBoard& board, bool minimizeRepeats, bool multithreading = false);
		static std::chrono::duration<double> getDurationLastSolution()
		{ return durationLastSolution; }
		static void printSolutions();
		static void findTrivialOmissions(const BasePiece& base, int boardSymmetry, bool outTrivialOmissions[8]);
		static void checkForTrivialSolutions();

		PentominoSolver(const PentominoBoard& board, bool minimizeRepeats);
		PentominoSolver(const PentominoSolver& original);
		~PentominoSolver();

		bool tryPushPentomino(const Pentomino& piece, const Point& pos);
		bool isPossibleSolution();
		PlacedPentomino popPentomino(); 
		void searchSimpleMinimizeRepeats(const Pentomino& piece, const Point& pos, int symmetry, int depth);
		void searchSimpleWithRepeats(const Pentomino& piece, const Point& pos, int depth);
		

		
		
	private:		
		static std::chrono::duration<double> durationLastSolution;
		static std::vector<PentominoSolver>* solutionsFound;
		static std::mutex lock;
		static bool wasFirstPiecePlaced;

		PentominoBoard mBoard{};
		bool* mPiecesAvailable{}; // heap allocated bool array, only used when mMinimizeRepeats = true
		std::vector<PlacedPentomino> mPlacedPentominoes{};
		bool mMinimizeRepeats{};
		BasePiece firstBase{BasePiece::X};
		
		char mNextSymbol{ 'A' }; // 1-char symbol to represent each instance of a piece in the solution

		std::vector<int> findHoleAreas();
		int findHoleArea(const Point& posHole);
		void resetAvailable();
		void setAvailable(const Pentomino& piece, bool available);
		bool checkPointInFirstQuadrant(const Point& pos) const;
		bool checkPointInTopHalf(const Point& pos) const;
			
		bool checkNoPiecesAvailable() const;
		bool checkPieceAvailable(const Pentomino& piece) const;
		
		
		
		
	};
}