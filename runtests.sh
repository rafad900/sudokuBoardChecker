#!/bin/bash
echo
echo "Running the sudoku executable with threads first and fork second"
echo "-------------------------"
echo "This is with example1.txt"
echo "-------------------------"
./sudoku.x < tests/example1.txt
./sudoku.x -f < tests/example1.txt

echo "-------------------------"
echo "This is with example2.txt"
echo "-------------------------"
./sudoku.x < tests/example2.txt
./sudoku.x -f < tests/example2.txt

echo "-------------------------"
echo "This is example1 with whitespace"
echo "-------------------------"
./sudoku.x < tests/example1-whitespace.txt
./sudoku.x -f < tests/example1-whitespace.txt

