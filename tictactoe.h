#ifndef TICTACTOE_H
#define TICTACTOE_H

#include <MCUFRIEND_kbv.h>

// Touchscreen pin definitions
#define YP A2
#define XM A3
#define YM 8
#define XP 9

// Touchscreen calibration values (adjust as needed)
#define TS_MINX 142
#define TS_MAXX 895
#define TS_MINY 130
#define TS_MAXY 902

// Pressure thresholds
#define MINPRESSURE 10
#define MAXPRESSURE 1000

// Tictactoe board dimensions
#define BOARDSIZE 220
#define CELLSIZE (BOARDSIZE / 3)
#define LINEWIDTH 4

extern MCUFRIEND_kbv tft;
extern char board[3][3];
extern char lines[8][3];
extern int xOffset;
extern int yOffset;
extern bool bGameOver;

// tictactoe.ino
void initGame();
char evaluateBoard();

// display.cpp
void initDisplay();
void drawX(int row, int col);
void drawO(int row, int col);
void drawHeading(const char *heading);
void drawStatusMessage(const char *message);
void drawTicTacToeBoard();

// strategy.cpp
int findWin();
int findTrap();
int findFork();
int findCenter();
int findCorner();
int findRandom();
int findMove();

#endif
