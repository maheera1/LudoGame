#ifndef GRID_H
#define GRID_H

#include <iostream>
using namespace std;

class Grid {
private:
    char board[15][15]; // Fixed-size 2D array representing the grid

public:
    Grid();                   // Constructor to initialize the grid
    void displayGrid() const; // Display the grid on the console
    char (*getGrid())[15];    // Function to return the grid
    void initializeTokens();  // Place tokens in their initial positions
};

#endif
