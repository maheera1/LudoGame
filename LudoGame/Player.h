#ifndef PLAYER_H
#define PLAYER_H

#include <array>
using namespace std;

class Player {
private:
    int playerId;               // Player ID
    array<int, 4> tokens;       // Positions of the 4 tokens (-1 = in yard)
    int hitRate;                // Number of hits made
    int completedTokens;        // Tokens in the home triangle

public:
    Player(int id);
    int rollDice();             // Simulates a dice roll
    bool moveToken(int tokenId, int steps, char grid[15][15]); // Move token
    bool allTokensCompleted() const; // Check if all tokens are in home
    void resetToken(int tokenId);    // Reset token to yard after being hit
    int getTokenPosition(int tokenId) const; // Get position of a specific token
    int getCompletedTokens() const;
    int getHitRate() const;
    void incrementHitRate();
    int getId() const;          // Get player ID
};

#endif
