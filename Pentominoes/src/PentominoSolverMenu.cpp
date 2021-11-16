// Author: Michael D'Agostino

#include "PentominoSolverMenu.h"

namespace Pentominoes
{
	PentominoBoard PentominoSolverMenu::currentBoard{};

	void PentominoSolverMenu::solve(const std::vector<char>& options)
	{
		bool useMultithreading{ true };
		bool minimizeRepeats{ true };
		bool removeTrivials{ true };
		for (char option : options)
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

	void PentominoSolverMenu::exit()
	{
		std::exit(0);
	}

	void PentominoSolverMenu::view(const std::vector<char>& options)
	{
		bool displayBoard{};
		for (char option : options)
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

	void PentominoSolverMenu::enterBoard()
	{
		currentBoard.promptUserInputBoard();
	}

	void PentominoSolverMenu::beginMainLoop()
	{
		std::string lastLine{};
		std::string lastCmd{};
		std::vector<char> lastOptions{};
		promptOptions();
		while (true)
		{
			lastOptions.clear();
			std::getline(std::cin, lastLine);

			// Parse last line of input
			if (lastLine.find('-') == std::string::npos)
			{
				// Command has no options, so lastCmd is the whole line
				lastCmd = lastLine;
			}
			else
			{
				// Command has options, split the command and the options
				lastCmd = lastLine.substr(0, lastLine.find(' '));
				lastLine.erase(lastLine.begin(), lastLine.begin()); // delete the space
				// Find and add each option
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
					solve(lastOptions);
				}
			}
			else if (lastCmd.compare("board") == 0)
			{
				enterBoard();
			}
			else if (lastCmd.compare("view") == 0)
			{
				view(lastOptions);
			}
			else if (lastCmd.compare("exit") == 0)
			{
				exit();
			}
			else if (lastCmd.compare("help") == 0)
			{
				promptOptions();
			}
			else
			{
				std::cout << "Error: Invalid command. Enter \"help\" for a list of commands\n";
			}

		}
	}

	void PentominoSolverMenu::promptOptions()
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
