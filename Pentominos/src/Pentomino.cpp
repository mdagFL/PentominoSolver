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
		std::cout << "Base PieceOrientation: " << Pentomino(getBasePieceOrientation()).getLabelString() << "\n";
		std::cout << "Rotated 90: " << getRotated90().getLabelString() << "\n";
		std::cout << "Roated 180: " << getRotated180().getLabelString() << "\n";
		std::cout << "Reflected over the vertical: " << getVerticalReflection().getLabelString() << "\n";
		std::cout << "Reflected over the horizontal: " << getHorizontalReflection().getLabelString() << "\n";
	}

	// Gets the first PieceOrientation that corresponds with the given OrientationBase
	PieceOrientation Pentomino::getBaseOrientation(BasePiece base)
	{
		int baseInt{ static_cast<int>(base) };
		if (base <= BasePiece::Y)
		{
			baseInt *= 8; // Each base in this section has 8 orientations
			return static_cast<PieceOrientation>(baseInt);
		}
		else if (base <= BasePiece::W)
		{
			baseInt -= 5; // Offset relative to the first base with 4 orientations, T
			baseInt *= 4; // Account for each of these orientations
			baseInt += 40; // Add the 40 from the section with 8 orientations
			return static_cast<PieceOrientation>(baseInt);
		}
		else if (base == BasePiece::Z)
		{
			return PieceOrientation::Z0;
		}
		else if (base == BasePiece::I)
		{
			return PieceOrientation::I0;
		}
		else // X	
		{
			return PieceOrientation::X0;
		}
			
	}


	int Pentomino::getNumberOfOrientations(BasePiece base)
	{
		if (base <= BasePiece::Y)
			return 8;
		else if (base <= BasePiece::Z)
			return 4;
		else if (base == BasePiece::I)
			return 2;
		else // I
			return 1;
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

	BasePiece Pentomino::getBase() const
	{
		if (mOrientation == PieceOrientation::I0 || mOrientation == PieceOrientation::I1)
			return BasePiece::I;
		else if (mOrientation == PieceOrientation::X0)
			return BasePiece::X;
		else if (mOrientation < PieceOrientation::T0)
		{
			// This is a shortcut that works because of how the PieceOrientation values are organized.
			// The first 40 orientations consist of groups of 8 orientations that map to one base piece
			// and are sequenced in the same order as OrientationBase values.
			int shifted = static_cast<int>(mOrientation) >> 3;
			return static_cast<BasePiece>(shifted);
		}
		else if (mOrientation < PieceOrientation::I0)
		{
			// Similar to the previous shortcut, but the remaining groups with 4 orientations
			// only consist of 4 elements, so subtract 40 and shift by 2 instead of 3.
			// Add 5 at the end, since the first 5 base pieces were covered by the previous block.
			int shifted = (static_cast<int>(mOrientation) - 40) >> 2;
			return static_cast<BasePiece>(shifted + 5);
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
			return Pentomino(base + rotation);
		}
		else if (orientation < 60) // Z piece
		{
			int base{ (orientation / 2) * 2 };
			int rotation{ orientation % 2 };
			rotation += 1;
			rotation %= 2; // Only 2 rotations for Z/ZM
			return Pentomino(base + rotation);
		}
		else if (orientation < 62) // I piece
		{
			if (orientation == 60)
				return Pentomino(61);
			else
				return Pentomino(60);
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

	Pentomino Pentomino::getRotated270() const
	{
		return getRotated180().getRotated90();
	}

	// Precondition: hasUniqueReflection() == true
	Pentomino Pentomino::getVerticalReflection() const
	{
		int orientation{ static_cast<int>(mOrientation) };
		if (hasUniqueReflection())
		{		
			if (orientation < 40)
			{
				// Change will either be orientation + 4 or orientation - 4
				bool isMirrored{ static_cast<bool>((orientation >> 2) & 1) };
				if (isMirrored)
					return Pentomino(orientation-4);
				else
					return Pentomino(orientation+4);
			}
			else // Z piece
			{
				// Change will either be orientation + 2 or orientation - 2
				bool isMirrored{ static_cast<bool>(orientation > 57) };
				if (isMirrored)
					return Pentomino(orientation - 2);
				else
					return Pentomino(orientation + 2);
			}
		}
		else
		{
			BasePiece base = getBase();
			if (mOrientation < PieceOrientation::Z0)
			{
				// Piece with 4 unique rotations but no unique reflections
				if (base == BasePiece::U || base == BasePiece::T)
				{
					if (orientation % 2 == 0)
						return *this; // Vertical reflection doesn't change these
					else
						return getRotated180();
				}
				else
				{
					if (orientation % 2 == 0)
						return getRotated270();
					else
						return getRotated90();
				}
			}
			else if (getBase() == BasePiece::I || getBase() == BasePiece::X)
			{
				// Always the same piece
				return *this;
			}
		}
		
	}

	Pentomino Pentomino::getHorizontalReflection() const
	{
		return getVerticalReflection().getRotated180();
	}

	bool Pentomino::hasUniqueReflection() const
	{
		int orientation = static_cast<int>(mOrientation);
		if (orientation < 40 || (orientation > 55 && orientation < 60))
			return true;
		return false;
	}

}