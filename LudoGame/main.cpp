// In main.cpp
#include "ThreadManager.h"
#include "Grid.h"
#include "Player.h"
#include <iostream>
#include <vector>
using namespace std;

int main() {
    // Initialize the Ludo grid
    Grid board;

    // Initialize player names vector
    vector<string> playerNames;

    // Take player names as input
    for (int i = 1; i <= 4; i++) {
        string name;
        cout << "Enter name for Player " << i << ": ";
        getline(cin, name); // Read player names
        playerNames.push_back(name);
    }

    // Start the game with player names
    ThreadManager threadManager(board, playerNames);  // Pass player names to ThreadManager

    // Start threads for players and simulate the game
    threadManager.startGame();

    return 0;
}
