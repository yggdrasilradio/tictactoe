#include "tictactoe.h"

typedef int (*MoveStrategy)();

struct StrategyEntry {
  const char* name;
  MoveStrategy func;
};

StrategyEntry strategies[] = {
  { "findWinOrBlock", findWinOrBlock },
  { "findFork", findFork },
  { "findCenter", findCenter },
  { "findCorner", findCorner },
  { "findRandom", findRandom }
};

int findMove() {
  for (auto& entry : strategies) {
    int move = entry.func();
    if (move != NOMOVE) {
      Serial.print("Strategy used: ");
      Serial.println(entry.name);
      return move;
    }
  }
  return NOMOVE;
}

// Take a winning move if you find one
// Otherwise block winning move for player if you find one
int findWinOrBlock() {

  int blockMove = -1;

  // For each winning line
  for (int i = 0; i < 8; i++) {

    char xCount = 0;      // Count Xs
    char oCount = 0;      // Count Os
    int emptyIndex = -1;  // Empty square

    // For each square in this line
    for (int j = 0; j < 3; j++) {
      int v = lines[i][j];
      int row = v / 3;
      int col = v % 3;
      char c = board[row][col];
      if (c == ' ')
        emptyIndex = v;  // Found empty square
      else if (c == 'X')
        xCount++;  // Found one more X
      else if (c == 'O')
        oCount++;  // Found one more O
    }
    if (oCount == 2 && emptyIndex >= 0)
      return emptyIndex;  // Winning move
    else if (xCount == 2 && emptyIndex >= 0)
      blockMove = emptyIndex;  // Blocking move
  }
  if (blockMove >= 0)
    return blockMove;
  else
    return NOMOVE;
}

// Attempt a fork or prevent a fork
int findFork() {

  int forkScores[9] = { 0 };

  // For each empty square
  for (int i = 0; i < 9; i++) {

    // If square is occupied, skip it
    int row = i / 3;
    int col = i % 3;
    if (board[row][col] != ' ') continue;

    // Check all lines for potential forks
    for (int l = 0; l < 8; l++) {
      bool bContainsSquare = false;
      char xCount = 0;
      char oCount = 0;

      // For each square in this line
      for (int j = 0; j < 3; j++) {
        int v = lines[l][j];
        if (v == i) bContainsSquare = true;
        char c = board[v / 3][v % 3];
        if (c == 'X') xCount++;
        else if (c == 'O') oCount++;
      }

      // Does this line intersect the empty square we're scoring?
      if (bContainsSquare) {

        // Potential fork for O?  Try that
        if (xCount == 0 && oCount == 1)
          forkScores[i] += 2;

        // Potential fork for X?  Block it!
        if (xCount == 1 && oCount == 0)
          forkScores[i]++;
      }
    }
  }

  // Choose square with most potential forks (must be more than one)
  int bestFork = -1;
  int maxScore = 1;
  for (int i = 0; i < 9; i++) {
    if (forkScores[i] > maxScore) {
      bestFork = i;
      maxScore = forkScores[i];
    }
  }
  if (bestFork >= 0)
    return bestFork;
  else
    return NOMOVE;
}

// Take center square if available
int findCenter() {

  if (board[1][1] == ' ')
    return 4;
  else
    return NOMOVE;
}

// Take random unoccupied corner if available
int findCorner() {

  int corners[4] = { 0, 2, 6, 8 };
  int availableCorners[4];
  int count = 0;

  for (int i = 0; i < 4; i++) {
    int v = corners[i];
    int row = v / 3;
    int col = v % 3;
    if (board[row][col] == ' ')
      availableCorners[count++] = v;
  }

  if (count > 0) {
    int r = random(count);  // Pick random index
    return availableCorners[r];
  }
  return NOMOVE;
}

// Hail Mary: pick a random unoccupied square
findRandom() {

  int emptySquares[9];
  int count = 0;
  for (int i = 0; i < 9; i++) {
    int row = i / 3;
    int col = i % 3;
    if (board[row][col] == ' ')
      emptySquares[count++] = i;
  }
  if (count > 0) {
    int r = random(count);  // Pick random index
    return emptySquares[r];
  }
  return NOMOVE;  // No move found
}