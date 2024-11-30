#include "ThreadManager.h"
#include <iostream>
#include <unistd.h>
#include <random>
#include <algorithm>

ThreadManager::ThreadManager(Grid &g, vector<string>& playerNames)
    : grid(g), currentPlayer(0), gameEnded(false) {
    for (int i = 0; i < playerNames.size(); i++) {
        players.emplace_back(i + 1, playerNames[i]);
    }
    noSixTurns.resize(playerNames.size(), 0);
    threadActive.resize(playerNames.size(), true); // All threads active at start
}

void ThreadManager::randomizeTurnOrder() {
    turnOrder = {1, 2, 3, 4};
    random_device rd;
    mt19937 gen(rd());
    shuffle(turnOrder.begin(), turnOrder.end(), gen);
}

int ThreadManager::rollDice() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    return dis(gen);
}

void ThreadManager::handleCollisions(int playerId, int tokenId) {
    int tokenPos = players[playerId - 1].getTokenPosition(tokenId);

    for (int i = 0; i < players.size(); i++) {
        if (i + 1 == playerId) continue;

        for (int j = 0; j < 4; j++) {
            if (players[i].getTokenPosition(j) == tokenPos) {
                players[i].resetToken(j);
                cout << players[playerId - 1].getName() << " hit " << players[i].getName() << "'s token!\n";
                players[playerId - 1].incrementHitRate();
                return;
            }
        }
    }
}

bool ThreadManager::checkWinningConditions() {
    for (const auto &player : players) {
        if (player.allTokensCompleted()) {
            gameEnded = true;
            return true;
        }
    }
    return false;
}

void* ThreadManager::playerThread(void* args) {
    PlayerData* data = (PlayerData*)args;
    int playerId = data->playerId;
    ThreadManager* manager = data->manager;

    while (!manager->gameEnded && manager->threadActive[playerId - 1]) {
        unique_lock<mutex> lock(manager->gridMutex);
        manager->cv.wait(lock, [&] {
            return manager->turnOrder[manager->currentPlayer] == playerId;
        });

        if (!manager->threadActive[playerId - 1] || manager->gameEnded) break;

        int consecutiveSixes = 0;
        bool bonusRoll = true;

        while (bonusRoll && !manager->gameEnded) {
            int diceRoll;
            {
                unique_lock<mutex> diceLock(manager->diceMutex);
                cout << manager->players[playerId - 1].getName() << "'s turn. Press Enter to roll the dice...\n";
                cin.get();
                diceRoll = manager->rollDice();
                cout << manager->players[playerId - 1].getName() << " rolled: " << diceRoll << "\n";
            }

            bonusRoll = (diceRoll == 6);
            if (bonusRoll) {
                consecutiveSixes++;
                manager->noSixTurns[playerId - 1] = 0; // Reset no-six counter
                if (consecutiveSixes == 3) {
                    cout << manager->players[playerId - 1].getName() << " rolled three 6s in a row and loses their turn!\n";
                    break;
                }
            } else {
                consecutiveSixes = 0;
                manager->noSixTurns[playerId - 1]++; // Increment no-six counter
            }

            bool moved = false;
            for (int i = 0; i < 4; i++) {
                if (manager->players[playerId - 1].moveToken(i, diceRoll, manager->grid.getGrid())) {
                    moved = true;
                    manager->handleCollisions(playerId, i);
                    break;
                }
            }

            if (!moved) {
                cout << manager->players[playerId - 1].getName() << " cannot move any token.\n";
                break;
            }

            if (manager->checkWinningConditions()) {
                cout << manager->players[playerId - 1].getName() << " wins the game!\n";
                manager->gameEnded = true;
                manager->cv.notify_all();
                return nullptr;
            }
        }

        manager->currentPlayer = (manager->currentPlayer + 1) % 4;
        if (manager->currentPlayer == 0) {
            manager->randomizeTurnOrder();
        }

        manager->cv.notify_all();
        lock.unlock();
        sleep(1);
    }
    return nullptr;
}

void* ThreadManager::masterThread(void* args) {
    ThreadManager* manager = (ThreadManager*)args;

    while (!manager->gameEnded) {
        sleep(1);

        for (size_t i = 0; i < manager->players.size(); ++i) {
            if (!manager->threadActive[i]) continue;

            if (manager->players[i].allTokensCompleted()) {
                cout << manager->players[i].getName() << " has completed all tokens and is removed from the game.\n";
                manager->threadActive[i] = false;
                manager->cv.notify_all();
            } else if (manager->noSixTurns[i] >= 20) {
                cout << manager->players[i].getName() << " didn't roll a six for 20 turns and is removed from the game.\n";
                manager->threadActive[i] = false;
                manager->cv.notify_all();
            }
        }

        if (all_of(manager->threadActive.begin(), manager->threadActive.end(), [](bool active) { return !active; })) {
            manager->gameEnded = true;
            manager->cv.notify_all();
        }
    }

    return nullptr;
}

void ThreadManager::startGame() {
    pthread_t playersThreads[4];
    pthread_t masterThreadId;
    PlayerData playerData[4];

    randomizeTurnOrder();

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

    vector<Player> rankedPlayers = players;
    sort(rankedPlayers.begin(), rankedPlayers.end(), [](const Player &a, const Player &b) {
        if (a.getCompletedTokens() != b.getCompletedTokens()) {
            return a.getCompletedTokens() > b.getCompletedTokens();
        }
        return a.getHitRate() > b.getHitRate();
    });

    cout << "Game Over! Final Results:\n";
    for (size_t i = 0; i < rankedPlayers.size(); ++i) {
        cout << i + 1 << ". Player " << rankedPlayers[i].getName() << ": "
             << "Tokens completed: " << rankedPlayers[i].getCompletedTokens()
             << ", Hits: " << rankedPlayers[i].getHitRate() << "\n";
    }
}
