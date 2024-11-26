#include "Grid.h"
#include "ThreadManager.h"
#include <iostream>
using namespace std;

int main() {
    // Initialize the Ludo grid
    Grid board;

    // Display the initial grid
    cout << "Initial Ludo Board:\n";
    board.displayGrid();

    // Start threads for players and simulate the game
    ThreadManager threadManager(board);
    threadManager.startGame();

    return 0;
}
