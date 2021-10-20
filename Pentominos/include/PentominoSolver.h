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
		{
			return durationLastSolution;
		}
		static void printSolutions();
		static void removeTrivialSolutions();


		PentominoSolver(const PentominoBoard& board, bool minimizeRepeats);
		PentominoSolver(const PentominoSolver& original);
		PentominoSolver(PentominoSolver&& original) noexcept;
		~PentominoSolver();
		
		PentominoSolver& operator=(const PentominoSolver& original);
		PentominoSolver& operator=(PentominoSolver&& original) noexcept;

		bool tryPushPentomino(const Pentomino& piece, const Point& pos);
		bool isPossibleSolution();
		PlacedPentomino popPentomino(); 
		void searchSimpleMinimizeRepeats(const Pentomino& piece, const Point& pos, int depth);
		void searchSimpleWithRepeats(const Pentomino& piece, const Point& pos, int depth);
		

		
		
	private:		
		static std::chrono::duration<double> durationLastSolution;
		static std::vector<PentominoSolver>* solutionsFound;
		static std::mutex lock;

		PentominoBoard mBoard{};
		bool* mPiecesAvailable{nullptr}; // heap allocated bool array, only used when mMinimizeRepeats = true
		std::vector<PlacedPentomino> mPlacedPentominoes{};
		bool mMinimizeRepeats{};
		char mNextSymbol{ 'A' }; // 1-char symbol to represent each instance of a piece in the solution

		std::vector<int> findHoleAreas();
		int findHoleArea(const Point& posHole);
		void resetAvailable();
		void setAvailable(const Pentomino& piece, bool available);
			
		bool checkNoPiecesAvailable() const;
		bool checkPieceAvailable(const Pentomino& piece) const;
		
		
		
		
	};
}