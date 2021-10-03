#include <array>
#include <string>

#pragma once
namespace Pentominoes
{
	// Enum for identifying all 63 unique pentomino orientations
	// Number suffix denotes the number of 90 degree clockwise rotations applied
	// M indicates that this is the reflection of the named piece
	enum class PieceOrientation
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
	enum class OrientationBase
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
	

	class Pentomino
	{
	public:
		static constexpr int cTotalOrientations = 63;
		static constexpr int cTotalBasePieces = 12;


		Pentomino() = delete;
		Pentomino(PieceOrientation piece);

		static void printAll();
		static void removeNewLines(std::string& str); // questionable placement
		static PieceOrientation getBaseOrientation(OrientationBase base);
		static int getNumberOfOrientations(OrientationBase base);

		int getXOffset() const { return mXOffset; }
		int getRectangleWidth() const { return mRectangleWidth; }
		int getRectangleHeight() const { return mRectangleHeight; }
		bool hasUniqueReflection() const;
		const std::string& getDataString() const;
		const std::string& getLabelString() const;
		PieceOrientation getOrientation() const { return mOrientation; }
		OrientationBase getBasePiece() const;
		Pentomino getRotated90() const;
		Pentomino getRotated180() const;
		Pentomino getReflection() const;

		// Get the first PieceOrientation associated with this piece's base
		PieceOrientation getBasePieceOrientation() const { return getBaseOrientation(getBasePiece()); }

		explicit operator int() const { return static_cast<int>(mOrientation); }

	private:
		static const std::string cOrientationDataStrings[63];
		static const std::string cOrientationLabelStrings[63]; // mainly for debugging
		
		PieceOrientation mOrientation;
		int mXOffset;
		int mRectangleWidth{};
		int mRectangleHeight{};

		
		void print() const;
		
	};
}