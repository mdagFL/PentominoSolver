# Purpose
The purpose of this program is to find all possible solutions to pentomino puzzles which can be entered by the user, including non-rectangular patterns.
While the traditional pentomino puzzle contains 60 available cells which must be filled by placing exactly 1 of each of the 12 unique, achiral pentominoes, this program is extended to solve both this version of the puzzle with 60 or fewer cells, where board with fewer cells may simply omit placing any pentominoes as necessary, and an alternate version of the puzzle where any pentomino may be placed any number of times on the board.

# Inputting a board
Boards are input by using the "board" command and entering lines of '1's and '0's.
'1's represent walls that may not be covered by any pentominoes.
'0's represent spaces which must be covered by pentominoes in solutions.
To finish entering a board, simply enter a blank line.

Keep in mind that any board containing a number of zeroes not divisible by 5, or islands of zeroes not divislbe by 5, will never have any solutions.

Any characters that are not zero or 1 are automatically be converted to 1.
If the entry is not a rectangle of 1s and 0s, any empty space is be assumed to be filled with ones.

# Solving a board
Boards are solved by using the "solve" command. 
