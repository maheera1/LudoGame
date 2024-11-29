
//player.cpp
#include "Player.h"
#include <random>

Player::Player(int id, const string &playerName) : playerId(id), name(playerName), hitRate(0), completedTokens(0) {
    tokens.fill(-1); // All tokens start in the yard
}
// string Player::getName() const {
//     return name;
// }
int Player::rollDice() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    return dis(gen);
}

bool Player::moveToken(int tokenId, int steps, char grid[15][15]) {
    if (tokens[tokenId] == -1 && steps == 6) {
        tokens[tokenId] = playerId * 10; // Enter the board
        return true;
    } else if (tokens[tokenId] >= 0) {
        tokens[tokenId] += steps; // Advance token
        if (tokens[tokenId] >= 50) {
            tokens[tokenId] = -2; // Token reaches home
            completedTokens++;
        }
        return true;
    }
    return false;
}

bool Player::allTokensCompleted() const {
    return completedTokens == 4;
}

void Player::resetToken(int tokenId) {
    tokens[tokenId] = -1;
}

int Player::getTokenPosition(int tokenId) const {
    return tokens[tokenId];
}

int Player::getCompletedTokens() const {
    return completedTokens;
}

int Player::getHitRate() const {
    return hitRate;
}

void Player::incrementHitRate() {
    hitRate++;
}

int Player::getId() const {
    return playerId;
}