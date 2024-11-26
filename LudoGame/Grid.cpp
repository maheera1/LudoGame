#include "Grid.h"

// Constructor to initialize the Ludo board
Grid::Grid() {
    // Initialize the board with regular spaces ('.')
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            board[i][j] = '.'; // Default empty space
        }
    }

    // Define player home areas (H1, H2, H3, H4)
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = '1';          // Player 1's home
            board[i][14 - j] = '2';     // Player 2's home
            board[14 - i][j] = '3';     // Player 3's home
            board[14 - i][14 - j] = '4'; // Player 4's home
        }
    }

    // Define safe zones (S)
    for (int i = 1; i < 14; i++) {
        if (i % 4 == 1) { // Set safe zones along rows and columns
            board[i][7] = 'S';
            board[7][i] = 'S';
        }
    }
}

// Function to display the grid
void Grid::displayGrid() const {
    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            cout << board[i][j] << ' ';
        }
        cout << '\n';
    }
}

// Function to return the grid array
char (*Grid::getGrid())[15] {
    return board;
}
