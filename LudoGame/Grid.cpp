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

// Function to initialize tokens in their starting positions
void Grid::initializeTokens() {
    // Set initial positions for each player's tokens in their home yards
    for (int i = 0; i < 4; i++) {
        board[0][i] = '1';          // Player 1 tokens
        board[0][14 - i] = '2';     // Player 2 tokens
        board[14][i] = '3';         // Player 3 tokens
        board[14][14 - i] = '4';    // Player 4 tokens
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

// Function to update a token's position on the grid
void Grid::updateTokenPosition(int playerId, int tokenId, int newPosition, int oldPosition) {
    // Clear the old position
    if (oldPosition >= 0 && oldPosition < 50) {
        int oldRow = oldPosition / 15;
        int oldCol = oldPosition % 15;
        board[oldRow][oldCol] = '.'; // Reset to empty
    }

    // Update to the new position
    if (newPosition >= 0 && newPosition < 50) {
        int newRow = newPosition / 15;
        int newCol = newPosition % 15;
        board[newRow][newCol] = '1' + playerId - 1; // Display as 1, 2, 3, 4 for each player
    }
}

// Function to return the grid array
char (*Grid::getGrid())[15] {
    return board;
}
