#include "tictactoe.h"
#include <TouchScreen.h>

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

MCUFRIEND_kbv tft;

int xOffset = (tft.width() - BOARDSIZE) / 2;
int yOffset = (tft.height() - BOARDSIZE) / 2 + 5;

bool bGameOver = false;

char board[3][3];

char lines[8][3] = {
  { 0, 1, 2 },
  { 3, 4, 5 },
  { 6, 7, 8 },
  { 0, 3, 6 },
  { 1, 4, 7 },
  { 2, 5, 8 },
  { 0, 4, 8 },
  { 6, 4, 2 }
};

void initGame() {
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      board[row][col] = ' ';
  tft.fillScreen(TFT_BLACK);
  drawHeading("TIC TAC TOE");
  drawTicTacToeBoard();
  randomSeed(analogRead(0));
}

// Check for wins or a tie game, indicating the game is over
char evaluateBoard() {

  // Check for wins
  for (int i = 0; i < 8; i++) {
    int x = 0;
    int o = 0;
    for (int j = 0; j < 3; j++) {
      int v = lines[i][j];
      int row = v / 3;
      int col = v % 3;
      char c = board[row][col];
      if (c == 'X')
        x++;
      else if (c == 'O')
        o++;
    }
    if (x == 3)
      return 'X';
    else if (o == 3)
      return 'O';
  }

  // Check for tie
  int n = 0;
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      if (board[row][col] != ' ')
        n++;

  if (n == 9)
    return 'T';  // Tie game
  else
    return ' ';  // Game still in progress
}

void setup() {

  // Turn off the onboard LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // Init TFT display
  initDisplay();

  // Start the game
  initGame();

  // Debug stuff
  Serial.begin(9600);
  Serial.println("Begin");
}

void loop() {

  // Get touchscreen reading
  TSPoint point = ts.getPoint();
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  digitalWrite(XM, HIGH);
  digitalWrite(YP, HIGH);

  if (point.z > MINPRESSURE && point.z < MAXPRESSURE && !bGameOver) {

    // Which square did the player tap?
    int mapX = map(point.y, TS_MINX, TS_MAXX, tft.width(), 0);
    int mapY = map(point.x, TS_MINY, TS_MAXY, tft.height(), 0);
    int col = (mapX - xOffset) / CELLSIZE;
    int row = (mapY - yOffset) / CELLSIZE;
    if (col >= 0 && col < 3 && row >= 0 && row < 3 && board[row][col] == ' ') {

      // Player move
      drawX(row, col);
      char result = evaluateBoard();
      if (result == 'T')
        drawStatusMessage("Tie game!");
      else if (result == 'X')
        drawStatusMessage("Player wins!");
      else if (result == ' ') {

        // Computer move
        //delay(600);
        result = findMove();
        row = result / 3;
        col = result % 3;
        drawO(row, col);
        result = evaluateBoard();
        if (result == 'T')
          drawStatusMessage("Tie game!");
        else if (result == 'O')
          drawStatusMessage("Computer wins!");
      }
    }
  }
  delay(100);
}
