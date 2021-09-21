#pragma once
#include <vector>

#include "Pentomino.h"
#include "PentominoBoard.h"

namespace Pentominoes
{
	template <int width, int height>
	class PentominoSolver
	{
	public:
		PentominoSolver() = delete;
		
		static std::vector<Pentominoes::PentominoBoard<width, height>> FindAllSolutions();
	private:
	};
}