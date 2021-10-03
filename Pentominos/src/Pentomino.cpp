#include <iostream>

#include "Pentomino.h"
#include "PentominoData.h"
#include "Debug.h"
namespace Pentominoes
{
	// Note: Many of these functions use various 'magic numbers' which are derived from the 
	// PieceOrientation enum defined in Pentomino.h and how it is organized. See Pentomino.h
	// for more information.


	const std::string Pentomino::cOrientationDataStrings[63]{ PENTOMINO_ORIENTATION_STRINGS };
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
		std::cout << "Rotated 90: " << getRotated90().getLabelString() << "\n";
		std::cout << "Roated 180: " << getRotated180().getLabelString() << "\n";
		if (hasUniqueReflection())
			std::cout << "Reflected: " << getReflection().getLabelString() << "\n";
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

	void Pentomino::removeNewLines(std::string& str)
	{
		int next = str.find('\n');
		while (next != std::string::npos)
		{
			str.erase(next, 1);
			next = str.find('\n');
		}
	}

	OrientationBase Pentomino::getBasePiece() const
	{
		if (mOrientation == PieceOrientation::I0 || mOrientation == PieceOrientation::I1)
			return OrientationBase::I;
		else if (mOrientation == PieceOrientation::X0)
			return OrientationBase::X;
		else if (mOrientation < PieceOrientation::T0)
		{
			// This is a shortcut that works because of how the PieceOrientation values are organized.
			// The first 40 orientations consist of groups of 8 orientations that map to one base piece
			// and are sequenced in the same order as OrientationBase values.
			int shifted = static_cast<int>(mOrientation) >> 3;
			return static_cast<OrientationBase>(shifted);
		}
		else if (mOrientation < PieceOrientation::I0)
		{
			// Similar to the previous shortcut, but the remaining groups with 4 orientations
			// only consist of 4 elements, so subtract 40 and shift by 2 instead of 3.
			// Add 5 at the end, since the first 5 base pieces were covered by the previous block.
			int shifted = (static_cast<int>(mOrientation) - 40) >> 2;
			return static_cast<OrientationBase>(shifted + 5);
		}
	}

	Pentomino Pentomino::getRotated90() const
	{
		int orientation{ static_cast<int>(mOrientation) };
		if (orientation < 56)
		{
			int base{ (orientation / 4) * 4 }; // regular or mirrored base piece
			int rotation{ orientation % 4 };
			rotation += 1;
			rotation %= 4; // 270 -> 0
			return Pentomino(static_cast<PieceOrientation>(base + rotation));
		}
		else if (orientation < 60) // Z piece
		{
			int base{ (orientation / 2) * 2 };
			int rotation{ orientation % 2 };
			rotation += 1;
			rotation %= 2; // Only 2 rotations for Z/ZM
			return Pentomino(static_cast<PieceOrientation>(base + rotation));
		}
		else if (orientation < 62) // I piece
		{
			if (orientation == 60)
				return Pentomino(static_cast<PieceOrientation>(61));
			else
				return Pentomino(static_cast<PieceOrientation>(60));
		}
		else // X piece
		{
			return (*this);
		}
	}

	Pentomino Pentomino::getRotated180() const
	{
		return getRotated90().getRotated90();
	}

	// Precondition: hasUniqueReflection() == true
	Pentomino Pentomino::getReflection() const
	{
		if (hasUniqueReflection())
		{
			int orientation = static_cast<int>(mOrientation);
			if (orientation < 40)
			{
				// Change will either be orientation + 4 or orientation - 4
				bool isMirrored = static_cast<bool>((orientation >> 2) & 1);
				if (isMirrored)
					return Pentomino(static_cast<PieceOrientation>(orientation-4));
				else
					return Pentomino(static_cast<PieceOrientation>(orientation+4));
			}
			else // Z piece
			{
				// Change will either be orientation + 2 or orientation - 2
				bool isMirrored = static_cast<bool>(orientation > 57);
				if (isMirrored)
					return Pentomino(static_cast<PieceOrientation>(orientation - 2));
				else
					return Pentomino(static_cast<PieceOrientation>(orientation + 2));
			}
		}
		return *this; // shouldn't happen
	}

	bool Pentomino::hasUniqueReflection() const
	{
		int orientation = static_cast<int>(mOrientation);
		if (orientation < 40 || (orientation > 55 && orientation < 60))
			return true;
		return false;
	}

}