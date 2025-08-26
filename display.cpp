#include "tictactoe.h"

void initDisplay() {

  tft.begin(tft.readID());
  tft.setRotation(0);
  tft.setTextWrap(false);
}

void drawTicTacToeBoard() {

  // Vertical lines
  for (int i = 1; i <= 2; i++) {
    int x = xOffset + i * CELLSIZE;
    tft.fillRect(x - LINEWIDTH / 2, yOffset, LINEWIDTH, BOARDSIZE, TFT_WHITE);
  }

  // Horizontal lines
  for (int i = 1; i <= 2; i++) {
    int y = yOffset + i * CELLSIZE;
    tft.fillRect(xOffset, y - LINEWIDTH / 2, BOARDSIZE, LINEWIDTH, TFT_WHITE);
  }
}

void drawX(int row, int col) {

  int x = xOffset + col * CELLSIZE;
  int y = yOffset + row * CELLSIZE;
  int padding = 20;

  board[row][col] = 'X';
  for (int i = 0; i < LINEWIDTH; i++) {
    tft.drawLine(x + padding + i, y + padding, x + CELLSIZE - padding + i, y + CELLSIZE - padding, TFT_RED);
    tft.drawLine(x + CELLSIZE - padding - i, y + padding, x + padding - i, y + CELLSIZE - padding, TFT_RED);
    delay(120);
  }
}

void drawO(int row, int col) {
  int x = xOffset + col * CELLSIZE + CELLSIZE / 2;
  int y = yOffset + row * CELLSIZE + CELLSIZE / 2;
  int radius = CELLSIZE / 2 - 16;

  board[row][col] = 'O';
  for (int i = 0; i < LINEWIDTH; i++) {
    tft.drawCircle(x, y, radius - i, TFT_BLUE);
    delay(100);
  }
}

void drawHeading(const char* heading) {
  tft.setTextSize(3);                     // Larger heading size
  tft.setTextColor(TFT_CYAN, TFT_BLACK);  // Foreground and background
  int16_t x1, y1;
  uint16_t w, h;

  // Measure text dimensions
  tft.getTextBounds(heading, 0, 0, &x1, &y1, &w, &h);

  // Calculate centered horizontal position
  int x = (tft.width() - w) / 2;
  int y = 4;  // Slight padding from top

  // Draw the heading
  tft.setCursor(x, y);
  tft.print(heading);
}

void drawStatusMessage(const char* message) {
  tft.setTextSize(2);                     // Adjust size as needed
  tft.setTextColor(TFT_CYAN, TFT_BLACK);  // Foreground and background
  int16_t x1, y1;
  uint16_t w, h;

  // Measure text width and height
  tft.getTextBounds(message, 0, 0, &x1, &y1, &w, &h);

  // Calculate centered position
  int x = (tft.width() - w) / 2;
  int y = tft.height() - h - 4;  // Slight padding from bottom

  // Draw the message
  tft.setCursor(x, y);
  tft.print(message);

  // Game must be over
  bGameOver = true;
}
