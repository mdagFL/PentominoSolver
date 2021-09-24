#include <iostream>
#include <string>

#include "Pentomino.h"
#include "PentominoBoard.h"
#include "PentominoSolver.h"


int main()
{
	Pentominoes::Pentomino::printAll();

	Pentominoes::PentominoBoard board;
	board.promptUserInputBoard();

	/*

	for (int i = 0; i < 63; i++)
	{
		Pentominoes::Pentomino pent{ static_cast<Pentominoes::PieceOrientation>(i) };

		std::cout << pent.getOrientationLabelString() << ":\n";
		std::cout << pent.getOrientationDataString();
		std::cout << "width: " << pent.getRectangleWidth() << ", height: " << pent.getRectangleHeight() << "\n\n";
	}

	*/

	Pentominoes::PentominoSolver solver(board);
	solver.tryPushPentomino(Pentominoes::Pentomino(Pentominoes::PieceOrientation::W1gj), Pentominoes::Point(0, 2));
	solver.tryPushPentomino(Pentominoes::Pentomino(Pentominoes::PieceOrientation::PM3), Pentominoes::Point(1, 0));
	solver.tryPushPentomino(Pentominoes::Pentomino(Pentominoes::PieceOrientation::NM0), Pentominoes::Point(3, 1));
	solver.tryPushPentomino(Pentominoes::Pentomino(Pentominoes::PieceOrientation::I1), Pentominoes::Point(0, 5));
	
	return 0;
}