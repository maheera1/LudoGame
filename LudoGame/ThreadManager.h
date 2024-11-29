//ThreadManager.h
#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "Grid.h"
#include "Player.h"
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>

using namespace std;

struct PlayerData {
    int playerId;
    class ThreadManager* manager;
};

class ThreadManager {
private:
    Grid &grid;
    vector<Player> players;     // List of players
    mutex gridMutex;            // Mutex for grid access
    mutex diceMutex;            // Mutex for dice access
    condition_variable cv;      // Condition variable for player turns
    int currentPlayer;          // ID of the current player
    bool gameEnded;             // Flag to indicate the game has ended

    static int rollDice();      // Function to simulate dice rolls
    void handleCollisions(int playerId, int tokenId);
    bool checkWinningConditions();

public:
    ThreadManager(Grid &g, vector<string>& playerNames); // Updated constructor
    void startGame();           // Start threads for players and master thread
    static void* playerThread(void* args); // Function for player thread
    static void* masterThread(void* args); // Master thread to monitor the game
};

#endif