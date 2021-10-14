#include <iostream>
#include <string>

#include "Pentomino.h"
#include "PentominoBoard.h"
#include "PentominoSolver.h"



int main()
{

	Pentominoes::Pentomino::printAll();
	std::vector<Pentominoes::Pentomino> vec;
	std::cout << vec.max_size() << "\n";
	Pentominoes::PentominoBoard board;
	board.promptUserInputBoard();


	Pentominoes::PentominoSolver::findAllSolutions(board, true, false);

	

	/*
	Pentominoes::PentominoSolver solver(board);
	Pentominoes::Pentomino v0(Pentominoes::PieceOrientation::V0);
	solver.tryPushPentomino(Pentominoes::PieceOrientation::V0, Pentominoes::Point(-v0.GetXOffset(), 0));
	*/
	//Pentominoes::PentominoSolver::findAllSolutions(board);
	//Pentominoes::PentominoSolver solver(board);
	Pentominoes::PentominoSolver::printSolutions();


	
}

