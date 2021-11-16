// Author: Michael D'Agostino

#include <array>
#include <string>

#pragma once
namespace Pentominoes
{
	// Enum for identifying all 63 unique pentomino orientations
	// Numeric suffix denotes the number of 90 degree clockwise rotations applied
	// M indicates that this is the reflection of the named piece
	enum class UniqueOrientation
	{
		F0,  F1,  F2,  F3, // 0-39: 8x unique orientations (4 rotations x 2 reflections)
		FM0, FM1, FM2, FM3,
		L0,  L1,  L2,  L3, 
		LM0, LM1, LM2, LM3,
		N0,  N1,  N2,  N3,
		NM0, NM1, NM2, NM3,
		P0,  P1,  P2,  P3,
		PM0, PM1, PM2, PM3,
		Y0,  Y1,  Y2,  Y3,
		YM0, YM1, YM2, YM3,
		T0,  T1,  T2,  T3,  // 40-55: 4x unique orientations (4 rotations, no reflections)
		U0,  U1,  U2,  U3,	
		V0,  V1,  V2,  V3,	
		W0,  W1,  W2,  W3,
		Z0,  Z1,			// 56-59: 4x unique orientations (2 rotations x 2 reflections)
		ZM0, ZM1,
		I0,  I1,			// 60-61: 2x unique orientations (2 rotations, no reflections)
		X0					// 62: 1 unique orientation
	};

	// Enum used to represent the 12 unique pentomino pieces
	enum class BasePentominoType
	{
		F,
		L,
		N,
		P,
		Y,
		T,
		U,
		V,
		W,
		Z,
		I,
		X
	};
	
	// Reprsents a uniquely oriented Pentomino
	class Pentomino
	{
	public:
		static constexpr int cTotalOrientations = 63;
		static constexpr int cTotalBasePieces = 12;


		Pentomino() = delete;
		Pentomino(UniqueOrientation piece);

		// Print data for every unique pentomino
		static void printAll();

		// Remove new lines from a string
		static void removeNewLines(std::string& str); // questionable placement

		// Returns the first UniqueOrientation from a given BasePentominoType
		static UniqueOrientation getBaseOrientation(BasePentominoType base);

		// Returns the number of unique orientations in a given BasePentominoType
		static int getNumberOfOrientations(BasePentominoType base);

		// Returns the X offset of the first filled cell in the top row of the piece data string
		int getXOffset() const { return mXOffset; }

		// Return the width of the smallest rectangle in which the Pentomino fits
		int getRectangleWidth() const { return mRectangleWidth; }

		// Returns the height of the smallest rectangle in which the Pentomino fits
		int getRectangleHeight() const { return mRectangleHeight; }

		// Returns true if the piece has a unique chiral counterpart
		bool hasUniqueReflection() const;

		// Returns the data string used to represent this Pentomino
		const std::string& getDataString() const;

		// Returns the label used to identify this Pentomino
		const std::string& getLabelString() const;

		// Returns the UniqueOrientation that defines this Pentomino.
		UniqueOrientation getOrientation() const { return mOrientation; }

		// Returns the BasePentominoType that corresponds to this Pentomino
		BasePentominoType getBasePiece() const;

		// Returns the Pentomino that is this Pentomino rotated 90 degrees clockwise
		Pentomino getRotated90() const;

		// Returns the Pentomino that is this Pentomino rotated 180 degrees counterclockwise
		Pentomino getRotated180() const;

		// Returns the chiral equivalent to this piece if it has one, or else returns this piece
		Pentomino getReflection() const;

		// Get the first UniqueOrientation associated with this piece's base
		UniqueOrientation getBaseUniqueOrientation() const { return getBaseOrientation(getBasePiece()); }

		explicit operator int() const { return static_cast<int>(mOrientation); }

	private:
		static const std::string cOrientationDataStrings[63];
		static const std::string cOrientationLabelStrings[63]; // mainly for debugging
		
		UniqueOrientation mOrientation;
		int mXOffset;
		int mRectangleWidth{};
		int mRectangleHeight{};

		// Print all data for this Pentomino
		void print() const;
		
	};
}