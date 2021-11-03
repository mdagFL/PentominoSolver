# Introduction
The purpose of this program is to find all possible solutions to pentomino puzzles which can be entered by the user, including non-rectangular patterns.
While the traditional pentomino puzzle contains 60 available cells which must be filled by placing exactly 1 of each of the 12 unique, achiral pentominoes, this program is extended to solve both this version of the puzzle with 60 or fewer cells, where board with fewer cells may simply omit placing any pentominoes as necessary, and an alternate version of the puzzle where any pentomino may be placed any number of times on the board.

NOTE: Commands are case-sensitive and must be entered exactly as they are shown.

# Inputting a Board
Boards are input by using the "board" command and entering lines of '1's and '0's.

'1's represent walls that may not be covered by any pentominoes.
'0's represent spaces which must be covered by pentominoes in solutions.
To finish entering a board, simply enter a blank line.

NOTE: Any board containing a number of zeroes not divisible by 5, or islands of zeroes not divislbe by 5, will never have any solutions.

Any characters that are not 0s, 1s, or new lines are automatically converted to 1.
If the entry is not a rectangle of 1s and 0s, any empty space is assumed to be filled with ones.

# Solving a Board
Boards are solved by using the "solve" command. This command should only be used after a board has already been entered using "board"

By default, all solutions will be found to the traditional pentomino puzzle (no repeated pieces allowed) using a backtracking algorithm, and then trivial solutions will be removed. Depending on the board, trivial solutions may be generated in the initial search, meaning solutions which are a simple transformation of an existing solution.

To skip omitting trivial solutions in the result, use the option "-T"
To solve the alternate form of the puzzle (pieces may be placed any number of times), use the option "-D"
To disable multithreading, use the option "-1". In my testing, this is usually about 50% slower but your mileage may vary.

# Displaying Solutions
To display solutions, use the command "view"
This command may also be used to view only the board that was previously entered by using the option "-B"

Solutions are generally not displayed in any particular order, however if you solve with multithreading disabled, they should display in the order of the piece placed in the top left corner of the board. Each placed piece is assigned a color based on the order it was placed on the board, which roughly correlates to where it was placed. This usually ensures that adjacent pieces don't have the same color, although it may not be impossible. They should always have a unique character representing them, however, which can be seen by copying a solution and pasting into a text editor.

# Current Limitations
The basic backtracking algorithm currently used has a pretty steep time complexity, roughly squaring for each additional 5 available cells. Boards with greater ratios of space volume to wall surface area will generally take longer to solve, as they usually have more unique solutions.


