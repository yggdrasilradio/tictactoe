#include "tictactoe.h"

#define NOMOVE -1

struct StrategyEntry {
  const char *name;
  int (*strategyRoutine)();
};

StrategyEntry strategies[] = {
  { "findWin", findWin },
  { "findTrap", findTrap },
  { "findFork", findFork },
  { "findCenter", findCenter },
  { "findCorner", findCorner },
  { "findRandom", findRandom }
};

// Percentage chance of making a (likely) dumb move
// If it's zero, the game is unwinnable
// If it's 100, you're playing against a drunk
#define STUPIDITY 20  // %

// Figure out the best move for O
int findMove() {
  if (random(100) < STUPIDITY) {

    // A random unoccupied square might be a brilliant move but I doubt it
    Serial.println("Stupid strategy used: findRandom");

    return findRandom();
  }
  for (auto &entry : strategies) {
    int move = entry.strategyRoutine();
    if (move != NOMOVE) {
      Serial.print("Strategy used: ");
      Serial.println(entry.name);
      return move;
    }
  }
  return NOMOVE;
}

// Avoid the classic corner fork trap
int findTrap() {

  // Serialize current state of board
  String state = "";
  for (int row = 0; row < 3; ++row) {
    for (int col = 0; col < 3; ++col) {
      char c = board[row][col];
      state += (c == ' ') ? '.' : c;
    }
  }

  // Is the player one move away from a classic corner fork trap?
  if (state == "X...O...X" || state == "..X.O.X..") {

    // Throw a monkey wrench into the works by taking any of the side squares
    int sideMoves[] = { 1, 3, 5, 7 };
    return sideMoves[random(0, 4)];
  }

  return NOMOVE;
}

// Take a winning move if you find one
// Otherwise block winning move for player if you find one
int findWin() {

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

  int scores[9] = { 0 };

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
          scores[i] += 2;  // Prioritize this one

        // Potential fork for X?  Block it!
        if (xCount == 1 && oCount == 0)
          scores[i]++;
      }
    }
  }

  // Choose square with most potential forks (must be more than one)
  int bestScore = -1;
  int maxScore = 1;
  for (int i = 0; i < 9; i++) {
    if (scores[i] > maxScore) {
      bestScore = i;
      maxScore = scores[i];
    }
  }
  if (bestScore >= 0)
    return bestScore;
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