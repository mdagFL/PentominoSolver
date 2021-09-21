#include <iostream>
#include <string>

#include "Pentomino.h"

int main()
{

	for (int i = 0; i < 63; i++)
	{
		Pentominoes::Pentomino pent{ static_cast<Pentominoes::PieceOrientation>(i) };

		std::cout << pent.getOrientationLabelString() << ":\n";
		std::cout << pent.getOrientationDataString();
		std::cout << "width: " << pent.getRectangleWidth() << ", height: " << pent.getRectangleHeight() << "\n\n";
	}
	

	return 0;
}