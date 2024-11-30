#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "Grid.h"
#include "Player.h"
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>
#include <algorithm>

using namespace std;

struct PlayerData {
    int playerId;
    class ThreadManager* manager;
};

class ThreadManager {
private:
    Grid &grid;                 // Shared Ludo grid
    vector<Player> players;     // List of players
    vector<int> turnOrder;      // Randomized order of player turns
    mutex gridMutex;            // Mutex for grid access
    mutex diceMutex;            // Mutex for dice access
    condition_variable cv;      // Condition variable for player turns
    int currentPlayer;          // Index of the current player in turnOrder
    bool gameEnded;             // Flag to indicate the game has ended
    vector<int> noSixTurns;     // Tracks consecutive turns without rolling a six for each player
    vector<bool> threadActive;  // Tracks if a thread is still active

    void randomizeTurnOrder();  // Helper function to shuffle turn order
    static int rollDice();      // Function to simulate dice rolls
    void handleCollisions(int playerId, int tokenId);
    bool checkWinningConditions();

public:
    ThreadManager(Grid &g, vector<string>& playerNames);
    void startGame();           // Start threads for players and master thread
    static void* playerThread(void* args); // Function for player threads
    static void* masterThread(void* args); // Master thread to monitor the game
};

#endif
