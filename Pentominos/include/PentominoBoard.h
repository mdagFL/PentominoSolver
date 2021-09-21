#pragma once
#include <string>
#include "Pentomino.h"

namespace Pentominoes
{
	struct Point
	{
		int x{ 0 };
		int y{ 0 };
		Point(int a_x, int a_y) : x{ a_x }, y{ a_y }
		{}
	};

	template <int width, int height>
	class PentominoBoard
	{
	public:
		PentominoBoard();
		PentominoBoard(char board[width][height]);
		bool hasNoSolution() const;
		
	private:
		char board[width][height]{};
		placePentomino(const Pentomino& piece, Point pos);

	};


}