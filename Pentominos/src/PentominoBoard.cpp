#include <vector>
#include <string>
#include <iostream>

#include "PentominoBoard.h"

namespace Pentominoes
{
	void PentominoBoard::promptUserInputBoard()
	{
		std::cout << "Please enter the board to be solved:\n";
		std::string line;
		std::getline(std::cin, line);
		while (line[0])
		{
			for (char& c : line)
			{
				if (c != '0')
					c = '1';
			}
			mBoard += line + '\n';
			std::getline(std::cin, line);
		}

		std::cout << mBoard;
	}
}