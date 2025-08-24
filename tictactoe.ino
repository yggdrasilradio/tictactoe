#include <MCUFRIEND_kbv.h>
#include <TouchScreen.h>

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

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

MCUFRIEND_kbv tft;

#define BOARDSIZE 220
#define CELLSIZE (BOARDSIZE / 3)

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

void startGame() {
  for (int row = 0; row < 3; row++)
    for (int col = 0; col < 3; col++)
      board[row][col] = ' ';
  tft.fillScreen(TFT_BLACK);
  drawHeading("TIC TAC TOE");
  drawTicTacToeBoard();
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
    return 'T'; // Tie game
  else
    return ' '; // Game still in progress
}

// Choose the best move for O to make
char findMove() {

  int blockMove = -1;

  // For each winning line
  for (int i = 0; i < 8; i++) {

    char x = 0;  // Count Xs
    char o = 0;  // Count Os
    int e = -1;  // Empty square

    // For each square in this line
    for (int j = 0; j < 3; j++) {
      int v = lines[i][j];
      int row = v / 3;
      int col = v % 3;
      char c = board[row][col];
      if (c == ' ')
        e = v;  // Found empty square
      else if (c == 'X')
        x++;  // Found one more X
      else if (c == 'O')
        o++;  // Found one more O
    }
    if (o == 2 && e >= 0)
      return e;  // Winning move
    else if (x == 2 && e >= 0)
      blockMove = e;  // Blocking move
  }
  if (blockMove >= 0) return blockMove;

  // Prefer center square if available
  if (board[1][1] == ' ') return 4;

  // Hail Mary: pick a random empty square
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
  return -1;  // No move suggested
}

void drawX(int row, int col) {

  int x = xOffset + col * CELLSIZE;
  int y = yOffset + row * CELLSIZE;
  int padding = 20;
  int thickness = 4;

  board[row][col] = 'X';
  for (int i = 0; i < thickness; i++) {
    tft.drawLine(x + padding + i, y + padding, x + CELLSIZE - padding + i, y + CELLSIZE - padding, TFT_RED);
    tft.drawLine(x + CELLSIZE - padding - i, y + padding, x + padding - i, y + CELLSIZE - padding, TFT_RED);
  }
}

void drawO(int row, int col) {

  int x = xOffset + col * CELLSIZE + CELLSIZE / 2;
  int y = yOffset + row * CELLSIZE + CELLSIZE / 2;
  int radius = CELLSIZE / 2 - 20;
  int thickness = 4;

  board[row][col] = 'O';
  for (int i = 0; i < thickness; i++)
    tft.drawCircle(x, y, radius - i, TFT_BLUE);
}

void drawTicTacToeBoard() {

  int lineThickness = 4;

  // Vertical lines
  for (int i = 1; i <= 2; i++) {
    int x = xOffset + i * CELLSIZE;
    tft.fillRect(x - lineThickness / 2, yOffset, lineThickness, BOARDSIZE, TFT_WHITE);
  }

  // Horizontal lines
  for (int i = 1; i <= 2; i++) {
    int y = yOffset + i * CELLSIZE;
    tft.fillRect(xOffset, y - lineThickness / 2, BOARDSIZE, lineThickness, TFT_WHITE);
  }
}

void setup() {

  randomSeed(analogRead(0));

  tft.begin(tft.readID());
  tft.setRotation(0);
  tft.setTextWrap(false);

  startGame();

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

  if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
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
