#include "Pentomino.h"
#include "PentominoData.h"
namespace Pentominoes
{
	const std::string Pentomino::cOrientationDataStrings[63] { PENTOMINO_ORIENTATION_STRINGS };
	const std::string Pentomino::cOrientationLabelStrings[63]{ PENTOMINO_LABEL_STRINGS };

	Pentomino::Pentomino(PieceOrientation piece)
		: mOrientation{ piece }
	{
		const std::string& pieceString{ getOrientationDataString() };

		// First occurrence of '\n' is the width
		mRectangleWidth = pieceString.find('\n');
		mRectangleHeight = pieceString.length() / (mRectangleWidth + 1); // Width + 1 to discount '\n' chars
		
		
	}

	const std::string& Pentomino::getOrientationDataString() const
	{
		return cOrientationDataStrings[static_cast<int>(mOrientation)];
	}

	std::string Pentomino::getOrientationLabelString() const
	{
		return cOrientationLabelStrings[static_cast<int>(mOrientation)];
	}
}