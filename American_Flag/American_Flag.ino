#include <FastLED.h>

// ***
// *** Define the pin for the RGB matrix.
// ***
#define RGB_MATRIX_PIN 13

// ***
// *** Define the size of the matrix. The LED strip is
// *** still wired as a single strip, but arranged in a
// *** matrix (square or rectangle).
// ***
#define MATRIX_ROWS     16
#define MATRIX_COLUMNS  16

#define NUM_LEDS        MATRIX_ROWS * MATRIX_COLUMNS
#define BRIGHTNESS      64
#define LED_TYPE        WS2812B
#define COLOR_ORDER     GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 20

void setup()
{
  Serial.begin(115200);
  delay(3000);

  Serial.println("Initializing matrix...");
  FastLED.addLeds<LED_TYPE, RGB_MATRIX_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear();
  FastLED.show();
  Serial.println("Matrix has been initialized.");

  // ***
  // *** Show corner the lower left corner.
  // ***
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(1000);

  // ***
  // *** Seed the random number generator.
  // ***
  randomSeed(analogRead(0));

  Serial.println("Ready");
}

uint16_t _offset = 0;

void loop()
{
  // ***
  // *** Animate the red and white stripes.
  // ***
  for (int row = 1; row <= MATRIX_ROWS; row++)
  {
    bool even = row % 2 == 0;
    animateStripe(row, even ? CRGB::Red : CRGB::White, even ? CRGB::White : CRGB::Red, _offset + (row * 3));
  }

  // ***
  // *** The offset moves the stripes horizontally.
  // ***
  _offset++;

  // ***
  // *** Draw the blue background over the stripes. This
  // *** uses 1/4 of the upper left corner of the matrix.
  // ***
  drawBlue();

  // ***
  // *** Cover the blue field with 25% random stars.
  // ***
  static uint16_t starCount = ((MATRIX_ROWS / 2) * (MATRIX_COLUMNS / 2)) * .25;

  // ***
  // *** Draw random stars over the blue background. The brightness
  // *** is random as well.  Note the random functions never returns
  // *** the upper bound, so use +1.
  // ***
  for (int i = 0; i < starCount; i++)
  {
    animateStar(random(MATRIX_ROWS / 2, MATRIX_ROWS + 1), random(1, MATRIX_COLUMNS + 1), CRGB::White, CRGB::Blue, random(0, 256));
  }

  // ***
  // *** Update the display.
  // ***
  FastLED.show();

  // ***
  // *** Delay for the animation effect.
  // ***
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

// ***
// *** Draw the blue background.
// ***
void drawBlue()
{
  for (uint16_t row = MATRIX_ROWS; row >= MATRIX_ROWS - 8; row--)
  {
    for (uint16_t column = 0; column <= MATRIX_COLUMNS - 8; column++)
    {
      leds[xy(row, column)] = CRGB::Blue;
    }
  }
}

// ***
// *** Animate a single star.
// ***
void animateStar(uint16_t row, uint16_t column, uint32_t foregroundColor, uint32_t backgroundColor, uint8_t brightness)
{
  leds[xy(row, column)] = blend(foregroundColor, backgroundColor, brightness);
}

// ***
// *** Animate a single stripe on a specified row.
// ***
void animateStripe(uint16_t row, uint32_t color1, uint32_t color2, uint16_t startOffset)
{
  uint16_t stripWidth = MATRIX_COLUMNS / 2;

  for (int column = 1; column <= MATRIX_COLUMNS; column++)
  {
    // ***
    // *** Adjust the offset so that it is never larger than the total columns.
    // ***
    uint16_t actualOffset = startOffset % (MATRIX_COLUMNS + 1);
    uint16_t actualColumn = (column + actualOffset)  % (MATRIX_COLUMNS + 1);
    uint16_t i = xy(row, actualColumn);

    if (column <= stripWidth)
    {
      leds[i] =  color1;
    }
    else
    {
      leds[i] =  color2;
    }
  }
}

// ***
// *** Calculates the absolute index of a pixel base
// *** on a row and column value. Rows and columns
// *** start with 1. If the pixels are wired in an
// *** interleaved fashion (the rows increased right
// *** to left and then left to right every other
// *** row) set interleaved to true. If every row is
// *** wired left to right (or right to left) then set
// *** interleaved to false.
// ***
uint16_t xy(uint16_t row, uint16_t column)
{
  uint16_t returnValue = 0;
  bool interleaved = true;

  row = constrain(row, 1, MATRIX_ROWS);
  column = constrain(column, 1, MATRIX_COLUMNS);

  if (!interleaved)
  {
    // ***
    // *** Map a row and column to an LED index.
    // ***
    returnValue = ((row - 1) * MATRIX_ROWS) + (column - 1);
  }
  else
  {
    bool evenRow = ((row - 1) % 2) == 0;

    if (evenRow)
    {
      returnValue = ((row - 1) * MATRIX_ROWS) + (column - 1);
    }
    else
    {
      returnValue = (((row - 1) * MATRIX_ROWS) + (MATRIX_COLUMNS - (column - 1))) - 1;
    }
  }

  return returnValue;
}
