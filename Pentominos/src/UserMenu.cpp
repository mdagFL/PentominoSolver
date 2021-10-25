#include "UserMenu.h"

namespace Pentominoes
{
	PentominoBoard UserMenu::currentBoard{};
	void UserMenu::FindSolutions()
	{

	}

	void UserMenu::Exit()
	{
		std::exit(0);
	}

	void UserMenu::BeginMainLoop()
	{
		std::string lastLine{};
		std::string lastCmd{};
		std::vector<char> lastOptions{};
		PromptOptions();
		while (true)
		{
			lastOptions.clear();
			std::getline(std::cin, lastLine);

			// Parse last line of input
			if (lastLine.find('-') == std::string::npos)
			{
				lastCmd = lastLine;
			}
			else
			{
				lastCmd = lastLine.substr(0, lastLine.find(' '));
				lastLine.erase(lastLine.begin(), lastLine.begin());
				while (lastLine.find('-') != std::string::npos)
				{
					lastLine = lastLine.substr(lastLine.find('-') + 1);
					lastOptions.push_back(lastLine[0]);
				}
			}

			// Execute command
			if (lastCmd.compare("solve") == 0)
			{
				if (currentBoard.getWidth() == 0 && currentBoard.getHeight() == 0)
				{
					std::cout << "Error: please enter a board first by typing \"board\"\n";
				}
				else
				{
					bool useMultithreading{ true };
					bool minimizeRepeats{ true };
					bool removeTrivials{ true };
					for (char option : lastOptions)
					{
						switch (option)
						{
						case '1':
							useMultithreading = false;
							break;
						case 'D':
							minimizeRepeats = false;
							break;
						case 'T':
							removeTrivials = false;
							break;
						}
					}
					PentominoSolver::beginSearch(currentBoard, minimizeRepeats, useMultithreading);
					if (removeTrivials)
						PentominoSolver::removeTrivialSolutions();
				}
			}
			else if (lastCmd.compare("board") == 0)
			{
					currentBoard.promptUserInputBoard();
					std::cout << "Board entered!\n";
			}
			else if (lastCmd.compare("view") == 0)
			{
				bool displayBoard{};
				for (char option : lastOptions)
				{
					if (option == 'B')
					{
						displayBoard = true;
					}
				}
				if (displayBoard)
				{
					if (currentBoard.getWidth() == 0 && currentBoard.getHeight() == 0)
					{
						std::cout << "Error: Please enter a board first by typing \"board\"\n";
					}
					else
					{
						currentBoard.printBoard();
					}
				}
				else
				{
					std::cout << "Count: " << PentominoSolver::getNumSolutionsFound() << "\n";
					PentominoSolver::printSolutions();
				}
			}
			else if (lastCmd.compare("exit") == 0)
			{
				Exit();
			}
			else if (lastCmd.compare("help") == 0)
			{
				PromptOptions();
			}
			else
			{
				std::cout << "Error: Invalid command. Enter \"help\" for a list of commands\n";
			}

		}
	}

	void UserMenu::PromptOptions()
	{
		std::cout << "Pentomino Solver Commands:\n";
		std::cout << std::left << std::setw(30) << "board" << "-Input a board to be solved" << std::endl;;
		std::cout << std::setw(30) << "solve" << "-Find all solutions to the last board that was entered" << std::endl;

		std::cout << std::left << std::setw(35) << " " << "Additional options:\n";
		std::cout << std::setw(35) << " " << "-T: Include trivial solutions\n";
		std::cout << std::setw(35) << " " << "-D: Allow the same piece to be placed any number of times\n";
		std::cout << std::setw(35) << " " << "-1: Disable multithreading\n";

		std::cout << std::setw(30) << "view" << "-Display all found solutions" << std::endl;

		std::cout << std::left << std::setw(35) << " " << "Additional options:\n";
		std::cout << std::setw(35) << " " << "-B: View only the board\n";

		std::cout << std::setw(30) << "help" << "-Show this prompt" << std::endl;
		std::cout << std::setw(30) << "exit" << "-Exit the program" << std::endl;
	}
}
