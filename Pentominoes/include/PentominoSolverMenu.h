// Author: Michael D'Agostino

#pragma once
#include <iostream>
#include <iomanip>
#include <string>

#include "PentominoBoard.h";
#include "PentominoSolver.h";
#include "Pentomino.h";

namespace Pentominoes
{
	// Static class providing the main menu loop and interfacing with
	// PentominoSolver classes. 
	class PentominoSolverMenu
	{
	public:
		// Begin main application loop, call from entry point
		static void beginMainLoop();
	private:
		static Pentominoes::PentominoBoard currentBoard;

		// Display list of commands and options
		static void promptOptions();
		// Prompt user to enter a board and get the board
		static void enterBoard();
		// Find solutions to previously entered board based on user options
		static void solve(const std::vector<char>& options);
		// View solutions or the board entered based on options
		static void view(const std::vector<char>& options);
		// Close the program
		static void exit();
		
	};
}
