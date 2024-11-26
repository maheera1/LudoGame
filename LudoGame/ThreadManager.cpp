#include "ThreadManager.h"
#include <iostream>
#include <unistd.h>

ThreadManager::ThreadManager(Grid &g) : grid(g), currentPlayer(1), gameEnded(false) {
    // Initialize players (4 players)
    for (int i = 1; i <= 4; i++) {
        players.emplace_back(i);
    }
}

// Function to simulate dice rolls
int ThreadManager::rollDice() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    return dis(gen);
}

// Handle collisions: reset opponent token if hit
void ThreadManager::handleCollisions(int playerId, int tokenId) {
    int tokenPos = players[playerId - 1].getTokenPosition(tokenId);

    for (int i = 0; i < players.size(); i++) {
        if (i + 1 == playerId) continue;

        for (int j = 0; j < 4; j++) {
            if (players[i].getTokenPosition(j) == tokenPos) {
                players[i].resetToken(j);
                cout << "Player " << playerId << " hit Player " << (i + 1) << "'s token!\n";
                players[playerId - 1].incrementHitRate();
                return;
            }
        }
    }
}

// Check if any player has won the game
bool ThreadManager::checkWinningConditions() {
    for (const auto &player : players) {
        if (player.allTokensCompleted()) {
            gameEnded = true;
            return true;
        }
    }
    return false;
}

// Player thread: handles dice rolls and token movement
void* ThreadManager::playerThread(void* args) {
    PlayerData* data = (PlayerData*)args;
    int playerId = data->playerId;
    ThreadManager* manager = data->manager;

    while (!manager->gameEnded) {
        unique_lock<mutex> lock(manager->gridMutex);
        manager->cv.wait(lock, [&] { return playerId == manager->currentPlayer; });

        if (manager->gameEnded) break;

        int diceRoll = rollDice();
        cout << "Player " << playerId << " rolled: " << diceRoll << "\n";

        if (diceRoll == 6) {
            cout << "Player " << playerId << " gets a bonus roll!\n";
        }

        // Move tokens or pass turn if no valid move
        bool moved = false;
        for (int i = 0; i < 4; i++) {
            if (manager->players[playerId - 1].moveToken(i, diceRoll, manager->grid.getGrid())) {
                moved = true;
                manager->handleCollisions(playerId, i);
                break;
            }
        }

        if (!moved) {
            cout << "Player " << playerId << " cannot move any token.\n";
        }

        if (manager->checkWinningConditions()) {
            cout << "Player " << playerId << " wins the game!\n";
            manager->gameEnded = true;
            manager->cv.notify_all();
            return nullptr;
        }

        manager->currentPlayer = (manager->currentPlayer % 4) + 1;
        manager->cv.notify_all();
        lock.unlock();
        sleep(1);
    }
    return nullptr;
}

// Master thread: monitors game progress
void* ThreadManager::masterThread(void* args) {
    ThreadManager* manager = (ThreadManager*)args;

    while (!manager->gameEnded) {
        sleep(1); // Periodic checks
    }
    return nullptr;
}

// Start all threads
void ThreadManager::startGame() {
    pthread_t playersThreads[4];
    pthread_t masterThreadId;
    PlayerData playerData[4];

    for (int i = 0; i < 4; i++) {
        playerData[i].playerId = i + 1;
        playerData[i].manager = this;
        pthread_create(&playersThreads[i], nullptr, playerThread, &playerData[i]);
    }

    pthread_create(&masterThreadId, nullptr, masterThread, this);

    for (int i = 0; i < 4; i++) {
        pthread_join(playersThreads[i], nullptr);
    }

    pthread_join(masterThreadId, nullptr);

    cout << "Game Over! Final Results:\n";
    for (const auto &player : players) {
        cout << "Player " << player.getId() << ": "
             << "Tokens completed: " << player.getCompletedTokens()
             << ", Hits: " << player.getHitRate() << "\n";
    }
}
