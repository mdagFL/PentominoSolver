// Author: Michael D'Agostino

#include <iostream>
#include "Pentomino.h"
#include "PentominoData.h"
#include "Debug.h"

namespace Pentominoes
{
	// Note: Many of these functions use integer literals which are derived from the 
	// UniqueOrientation enum defined in Pentomino.h and how it is organized. See
	// UniqueOrientation in Pentomino.h for more information.


	const std::string Pentomino::cOrientationDataStrings[63]{ PENTOMINO_ORIENTATION_STRINGS };
	const std::string Pentomino::cOrientationLabelStrings[63]{ PENTOMINO_LABEL_STRINGS };

	void Pentomino::printAll()
	{
		for (int i = 0; i < cTotalOrientations; i++)
		{
			Pentomino piece{ static_cast<UniqueOrientation>(i) };
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
		std::cout << "Base UniqueOrientation: " << Pentomino(getBaseUniqueOrientation()).getLabelString() << "\n";
		std::cout << "Rotated 90: " << getRotated90().getLabelString() << "\n";
		std::cout << "Roated 180: " << getRotated180().getLabelString() << "\n";
		if (hasUniqueReflection())
			std::cout << "Reflected: " << getReflection().getLabelString() << "\n";
	}

	UniqueOrientation Pentomino::getBaseOrientation(BasePentominoType base)
	{
		int baseInt{ static_cast<int>(base) };
		if (base <= BasePentominoType::Y)
		{
			baseInt *= 8; // Each base in this section has 8 orientations
			return static_cast<UniqueOrientation>(baseInt);
		}
		else if (base <= BasePentominoType::W)
		{
			baseInt -= 5; // Offset relative to the first base with 4 orientations, T
			baseInt *= 4; // Account for each of these orientations
			baseInt += 40; // Add the 40 from the section with 8 orientations
			return static_cast<UniqueOrientation>(baseInt);
		}
		else if (base == BasePentominoType::Z)
		{
			return UniqueOrientation::Z0;
		}
		else if (base == BasePentominoType::I)
		{
			return UniqueOrientation::I0;
		}
		else // X	
		{
			return UniqueOrientation::X0;
		}
			
	}


	int Pentomino::getNumberOfOrientations(BasePentominoType base)
	{
		if (base <= BasePentominoType::Y)
			return 8;
		else if (base <= BasePentominoType::Z)
			return 4;
		else if (base == BasePentominoType::I)
			return 2;
		else // I
			return 1;
	}

	Pentomino::Pentomino(UniqueOrientation piece)
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
	
	BasePentominoType Pentomino::getBasePiece() const
	{
		// Instead of manually mapping every single UniqueOrientation to an BasePentominoType, we can
		// use bit manipulation to map each of four groups of UniqueOrientations, each group of which
		// having the same number of mappings to an BasePentominoType. See UniqueOrientation in Pentomino.h
		// for more information.
		if (mOrientation == UniqueOrientation::I0 || mOrientation == UniqueOrientation::I1)
			return BasePentominoType::I;
		else if (mOrientation == UniqueOrientation::X0)
			return BasePentominoType::X;
		else if (mOrientation < UniqueOrientation::T0)
		{
			// The first 40 orientations consist of groups of 8 orientations that map to one base piece
			// and are sequenced in the same order as BasePentominoType values.
			int shifted = static_cast<int>(mOrientation) >> 3;
			return static_cast<BasePentominoType>(shifted);
		}
		else if (mOrientation < UniqueOrientation::I0)
		{
			// Similar to the previous shortcut, but the remaining groups with 4 orientations
			// only consist of 4 elements, so subtract 40 and shift by 2 instead of 3.
			// Add 5 at the end, since the first 5 base pieces were covered by the previous block.
			int shifted = (static_cast<int>(mOrientation) - 40) >> 2;
			return static_cast<BasePentominoType>(shifted + 5);
		}
	}

	Pentomino Pentomino::getRotated90() const
	{
		int orientation{ static_cast<int>(mOrientation) };
		if (orientation < 56) // All pieces where that base and any reflections have 4 x 90 rotations
		{
			int base{ (orientation / 4) * 4 }; // Regular/mirrored base piece
			int rotation{ orientation % 4 };
			rotation += 1;
			rotation %= 4; // 360 -> 0
			return Pentomino(static_cast<UniqueOrientation>(base + rotation));
		}
		else if (orientation < 60) // Z piece (2 x 90 rotations for base and any reflections)
		{
			int base{ (orientation / 2) * 2 };
			int rotation{ orientation % 2 };
			rotation += 1;
			rotation %= 2; // Only 2 rotations for Z/ZM
			return Pentomino(static_cast<UniqueOrientation>(base + rotation));
		}
		else if (orientation < 62) // I piece (1 x 90 rotation)
		{
			if (orientation == 60)
				return Pentomino(static_cast<UniqueOrientation>(61));
			else
				return Pentomino(static_cast<UniqueOrientation>(60));
		}
		else // X piece (only 1 orientation)
		{
			return (*this);
		}
	}

	Pentomino Pentomino::getRotated180() const
	{
		return getRotated90().getRotated90();
	}

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
					return Pentomino(static_cast<UniqueOrientation>(orientation-4));
				else
					return Pentomino(static_cast<UniqueOrientation>(orientation+4));
			}
			else // Z piece
			{
				// Change will either be orientation + 2 or orientation - 2
				bool isMirrored = static_cast<bool>(orientation > 57);
				if (isMirrored)
					return Pentomino(static_cast<UniqueOrientation>(orientation - 2));
				else
					return Pentomino(static_cast<UniqueOrientation>(orientation + 2));
			}
		}
		return *this; // no unique reflection
	}

	bool Pentomino::hasUniqueReflection() const
	{
		int orientation = static_cast<int>(mOrientation);

		// Orientations 0-40 and 56-59 have unique reflections
		if (orientation < 40 || (orientation > 55 && orientation < 60))
			return true;
		return false;
	}

}