#include <iostream>

#include "Pentomino.h"
#include "PentominoData.h"
#include "Debug.h"
namespace Pentominoes
{
	const std::string Pentomino::cOrientationDataStrings[63] { PENTOMINO_ORIENTATION_STRINGS };
	const std::string Pentomino::cOrientationLabelStrings[63]{ PENTOMINO_LABEL_STRINGS };

	// Print data for all possible pentominoes
	void Pentomino::printAll()
	{
		for (int i = 0; i < cTotalOrientations; i++)
		{
			Pentomino piece{ static_cast<PieceOrientation>(i) };
			piece.print();
		}
	}

	void Pentomino::print() const
	{
		std::cout << getLabelString() << ":\n";
		std::cout << getDataString();
		std::cout << "rectWidth: " << mRectangleWidth << "\n";
		std::cout << "rectHeight: " << mRectangleHeight << "\n";
		std::cout << "X offset: " << mXOffset << "\n";
	}

	Pentomino::Pentomino(PieceOrientation piece)
		: mOrientation{ piece }
	{
		const std::string& pieceString{ getDataString() };

		// First occurrence of '\n' is the width
		mRectangleWidth = pieceString.find('\n');
		mRectangleHeight = pieceString.length() / (mRectangleWidth + 1); // Width + 1 to discount '\n' chars
		
		mXOffset = pieceString.find('1');
	}

	const std::string& Pentomino::getDataString() const
	{
		return cOrientationDataStrings[static_cast<int>(mOrientation)];
	}

	const std::string& Pentomino::getLabelString() const
	{
		return cOrientationLabelStrings[static_cast<int>(mOrientation)];
	}
}