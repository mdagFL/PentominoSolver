#pragma once
#include <iostream>
#include <iomanip>
#include <string>

#include "PentominoBoard.h";
#include "PentominoSolver.h";
#include "Pentomino.h";

namespace Pentominoes
{
	class UserMenu
	{
	public:
		static void BeginMainLoop();
	private:
		static void PromptOptions();
		static void FindSolutions();
		static void Exit();
		static void TrimString(std::string& str);
		static Pentominoes::PentominoBoard currentBoard;
	};
}
