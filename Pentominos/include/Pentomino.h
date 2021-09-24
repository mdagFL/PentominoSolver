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
		F0,  F1,  F2,  F3,
		FM0, FM1, FM2, FM3,
		L0,  L1,  L2,  L3,
		LM0, LM1, LM2, LM3,
		N0,  N1,  N2,  N3,
		NM0, NM1, NM2, NM3,
		P0,  P1,  P2,  P3,
		PM0, PM1, PM2, PM3,
		Y0,  Y1,  Y2,  Y3,
		YM0, YM1, YM2, YM3,
		T0,  T1,  T2,  T3,
		U0,  U1,  U2,  U3,
		V0,  V1,  V2,  V3,	
		W0,  W1,  W2,  W3,
		Z0,  Z1,
		ZM0, ZM1,
		I0,  I1,		
		X0
	};

	

	class Pentomino
	{
	public:
		static constexpr int cTotalOrientations = 63;

		Pentomino() = delete;
		Pentomino(PieceOrientation piece);

		static void printAll();
		static void removeNewLines(std::string& str); // questionable placement

		int GetXOffset() const { return mXOffset; }
		int getRectangleWidth() const { return mRectangleWidth; }
		int getRectangleHeight() const { return mRectangleHeight; }
		const std::string& getDataString() const;
		const std::string& getLabelString() const;
		
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