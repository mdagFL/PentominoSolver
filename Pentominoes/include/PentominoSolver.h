// Author: Michael D'Agostino

#pragma once
#include <vector>
#include <chrono>
#include <mutex>
#include <Windows.h>

#include "Pentomino.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	// Struct to represent a Pentomino that has been placed on a board, including
	// the Pentomino object, location, and symbol used for displaying the piece
	struct PlacedPentomino
	{
		Pentomino pentomino;
		Point position;
		char symbol;
		PlacedPentomino(const Pentomino& a_pentomino, Point a_position, char a_symbol)
			: pentomino{ a_pentomino }, position{ a_position }, symbol{ a_symbol } {}
	};

	// Class used for solving a PentominoBoard
	class PentominoSolver
	{
	public:
		// Makes initial call to beginSearch and times its execution
		// PostCondition: solutionsFound is populated with solution boards
		static void beginSearch(const PentominoBoard& board, bool minimizeRepeats, bool multithreading = false);

		// Getter method for for chronos object
		static std::chrono::duration<double> getDurationLastSolution() { return durationLastSolution; }
		
		// Print any solutions found from last call to beginSearch
		static void printSolutions();
		
		// Remove any solutions from last solution set that are created by simple transformations of other solutions
		static void removeTrivialSolutions();
		
		// Get the total number of solutions found from last call to beginSearch.
		// If removeTrivialSolutions was called, result does not include trivial solutions.
		static int getNumSolutionsFound() {return (solutionsFound )? solutionsFound->size() : 0; }


		PentominoSolver(const PentominoBoard& board, bool minimizeRepeats, bool multithreading);
		PentominoSolver(const PentominoSolver& original);
		PentominoSolver(PentominoSolver&& original) noexcept;
		~PentominoSolver();

		PentominoSolver& operator=(const PentominoSolver& original);
		PentominoSolver& operator=(PentominoSolver&& original) noexcept;
		
		
	private:		
		static std::chrono::duration<double> durationLastSolution;
		static std::vector<PentominoSolver>* solutionsFound;
		static std::mutex lock;
		static std::vector<std::thread> threads;

		PentominoBoard mBoard{};
		bool mMultithreading{};
		bool* mPiecesAvailable{nullptr}; // heap allocated bool array, only used when mMinimizeRepeats = true
		std::vector<PlacedPentomino> mPlacedPentominoes{};
		bool mMinimizeRepeats{};
		char mNextSymbol{ 'A' }; // 1-char symbol to represent each instance of a piece in the solution

		// Returns vector of areas of each island of available cells on the board
		std::vector<int> findHoleAreas();

		// Recursive function to sum all the adjacent available cells from a given point on the board
		int findHoleArea(const Point& posHole);

		// Set all pieces to available when minimizing repeats
		// Precondition: mMinimizeRepeats == true
		void resetAvailable();

		// Set an individual piece to available when minimizing repeats
		// Precondition: mMinimizeRepeats == true
		void setAvailable(const Pentomino& piece, bool available);
			
		// Check if every piece has been placed at least once when minimizing repeats
		// Precondition: mMinimizeRepeats == true
		bool checkNoPiecesAvailable() const;

		// Check if piece may still be placed when minimizing repeats
		// Precondition: mMinimizeRepeats == true
		bool checkPieceAvailable(const Pentomino& piece) const;

		// If piece can legally be placed on mBoard at pos, returns true and places piece.
		bool tryPushPentomino(const Pentomino& piece, const Point& pos);

		// Returns false if there exists any island of available cells on the board with
		// an area not divisible by 5
		bool isPossibleSolution();

		// Pop the last placed pentomino
		// Precondition: At least 1 placed pentomino on the board
		PlacedPentomino popPentomino();

		// Recursive backtracking function to find all solutions
		// Must pass depth of 0 for the initial call
		// Postcondition: solutionsFound is populated with all solutions to mBoard
		void solveBacktracking(int depth = 0);
		
		
		
	};
}