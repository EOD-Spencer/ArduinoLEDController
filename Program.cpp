/* -------------------------------------------------------------------------- */
/*                                Project Info                                */
/* -------------------------------------------------------------------------- */
/**
 * @file      NeoPixel_WarningLights.ino
 * @author    EOD-Spencer (EOD.Spencer@iCloud.com)
 * @link      github.com/EODSpencer/NeoPixelWarningLights
 * @brief     Arduino driven LED leds with Police, Fire, and EMS flash patterns
 * @date      September 25, 2023
 */

/**
 * @copyright NeoPixelWarningLights © 2023 by William C. Spencer is licensed under Attribution-NonCommercial-NoDerivatives 4.0 International.
 * To view a copy of this license, visit http://creativecommons.org/licenses/by-nc-nd/4.0/
*/

/* -------------------------------------------------------------------------- */
/*                                  Libraries                                 */
/* -------------------------------------------------------------------------- */
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

/* -------------------------------------------------------------------------- */
/*                            Function Declarations                           */
/* -------------------------------------------------------------------------- */
void ChangePattern(void);
void Alternating_TwoColorWhite(RgbwColor, RgbwColor, int, int, bool);
void Flash(RgbwColor, int, int, int, int);
void Alternating_TwoColor(RgbwColor, RgbwColor, int, int);
void ArrowRight(RgbwColor, int);
void ArrowLeft(RgbwColor, int);
void Alternating_SingleColor(RgbwColor, int);
void SteadyOn_SingleColor(RgbwColor);
void ArrowCenter(RgbwColor, int);
void effect1(RgbwColor, RgbwColor, int);
void LightningFlash(RgbwColor, int, int, int);
void Cylon(RgbwColor, int, int);
void TestLEDs(bool);
void ShowConfiguration(void);
void ButtonISR(void);
void ButtonTest(void);

/* -------------------------------------------------------------------------- */
/*                                  Constants                                 */
/*    NOTE: The "NUMBER_OF_LEDS" MUST be an even number                       */
/* -------------------------------------------------------------------------- */
const int PATTERN_BUTTON_PIN = 2;         // Digital IO pin connected to the button.
const int PATTERN_BUTTON_DEBOUNCE = 150;  // Length of time to allow for debounce
const int LED_OUTPUT_PIN = 10;             // Digital IO pin connected to the LED leds.
const int LED_BRIGHTNESS = 20;            // LED brightness, 0 (min) to 255 (max)
const int NUMBER_OF_LEDS = 20;            // Number of LEDs on the NeoPixel leds
const int FLASH_SLOW = 250;               // Amount of time to wait between flashes (Slow)
const int FLASH_MEDIUM = 50;              // Amount of time to wait between flashes (Medium)
const int FLASH_QUICK = 10;                // Amount of time to wait between flashes (Quick)
const int NUMBER_OF_PATTERNS = 21;        // Total number of patterns available


/* -------------------------------------------------------------------------- */
/*                              Global Variables                              */
/* -------------------------------------------------------------------------- */
unsigned long lastButtonTime_patternButton = 0;       // The last time the pattern button was pressed
int patternButtonCounter = 0;                         // counter for the number of button presses
int buttonState_patternButton = 0;                    // current state of the button
int lastButtonState_patternButton = 0;                // previous state of the button
int currentPattern = 0;                               // Current pattern number
bool interruptLoop = false;                           // Interrupts loop to change pattern


/* -------------------------------------------------------------------------- */
/*                               LED Strip Setup                              */
/* -------------------------------------------------------------------------- */
NeoPixelBus<NeoGrbwFeature, NeoWs2812xMethod> leds(NUMBER_OF_LEDS, LED_OUTPUT_PIN);

RgbColor red(255, 0, 0);
RgbColor green(0, 255, 0);
RgbColor blue(0, 0, 255);
RgbColor white(255, 255, 255);
RgbColor amber(255, 215, 0);
RgbColor black(0, 0, 0);

HslColor hslRed(red);
HslColor hslGreen(green);
HslColor hslBlue(blue);
HslColor hslWhite(white);
HslColor hslAmber(amber);
HslColor hslBlack(black);



int modules[3][3] = {. //Each row is a module
{1, 2, 3},
{4, 5, 6},
{7, 8, 9}
};

// Access and print individual elements of the matrix
for (int row = 0; row < 3; ++row) {
for (int col = 0; col < 3; ++col) {
std::cout << matrix[row][col] << " ";
}
std::cout << std::endl;
}

// Accessing elements of the matrix using indices
int element = matrix[1][2]; // Accesses the element in the second row and third column (6)
std::cout << "Element at matrix[1][2]: " << element << std::endl;

// You can also modify elements of the matrix
matrix[0][0] = 99; // Sets the element in the first row and first column to 99

// Printing the modified matrix
std::cout << "Modified Matrix:" << std::endl;
for (int row = 0; row < 3; ++row) {
for (int col = 0; col < 3; ++col) {
std::cout << matrix[row][col] << " ";
}
std::cout << std::endl;
}
return 0;
}


/* -------------------------------------------------------------------------- */
/*                                    SETUP                                   */
/* -------------------------------------------------------------------------- */
void setup()
{
  Serial.begin(9600);
  while (!Serial);
  pinMode(PATTERN_BUTTON_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  NeoPixelBus<NeoRgbwFeature, NeoWs2812xMethod> leds(NUMBER_OF_LEDS, LED_OUTPUT_PIN);
  leds.Begin();
  leds.ClearTo(black);
  leds.Show();
  attachInterrupt(digitalPinToInterrupt(PATTERN_BUTTON_PIN), ButtonISR, FALLING);

  TestLEDs(false);
}


/* -------------------------------------------------------------------------- */
/*                                    LOOP                                    */
/* -------------------------------------------------------------------------- */
void loop()
{
  if (interruptLoop == false)
  {
    switch (currentPattern)
    {
      case 0:
        leds.ClearTo(black);
        leds.Show();
        break;
      case 1:
        LightningFlash(blue, 10, 10, FLASH_QUICK);
        LightningFlash(white, 10, 10, FLASH_QUICK);
        LightningFlash(blue, 10, 10, FLASH_QUICK);
        LightningFlash(red, 10, 10, FLASH_QUICK);
        LightningFlash(white, 10, 10, FLASH_QUICK);
        LightningFlash(red, 10, 10, FLASH_QUICK);
        break;
      case 2:
        Alternating_TwoColorWhite(blue, red, 4, FLASH_QUICK, true);
        break;
      case 3:
        ArrowCenter(amber, FLASH_SLOW);
        break;
      case 4:
        effect1(blue, red, 4);
        break;
      default:
        currentPattern = 0;
        break;
    }
  }
  else
  {
    ChangePattern();
  }
}


/* -------------------------------------------------------------------------- */
/*                              General Functions                             */
/* -------------------------------------------------------------------------- */

/* ------------------------ Interrupt Service Routine ----------------------- */
void ButtonISR()
{
  interruptLoop = true;
}


/* -------------------------- Verbose Configuration ------------------------- */
void ShowConfiguration()
{
  Serial.println("-----------------------------------------------");
  Serial.println("            STATE OF VARIABLES");
  Serial.println("-----------------------------------------------");
  Serial.print("Time of Last Button Press:  ");
  Serial.println(lastButtonTime_patternButton);

  Serial.print("Number of Presses:  ");
  Serial.println(patternButtonCounter);

  Serial.print("Current Button State:  ");
  Serial.println(buttonState_patternButton);

  Serial.print("Last Button State:  ");
  Serial.println(lastButtonState_patternButton);

  Serial.print("Current Pattern Number:  ");
  Serial.println(currentPattern);

  Serial.print("Change Pattern:  ");
  Serial.println(interruptLoop);
  Serial.println("-----------------------------------------------");
  delay(2500);
}


/* -------------------------------- LED Test -------------------------------- */
void TestLEDs(bool run)
{
  if (run) {
    int wait = 5000;
    Serial.println("Colors will be shown in the following order: ");
    Serial.println("  - RED");
    Serial.println("  - GREEN");
    Serial.println("  - BLUE");
    Serial.println("  - WHITE");
    Serial.println("If leds do not show in this order, try using the NeoGrbFeature");
    delay(wait);
    /* ----------------------------- Set the colors ----------------------------- */
    /* --- if they don't match in order, use NeoGrbFeature feature -- */
    leds.SetPixelColor(0, red);
    leds.SetPixelColor(1, green);
    leds.SetPixelColor(2, blue);
    leds.SetPixelColor(3, white);
    leds.Show();
    Serial.println("Showing the colors...");
    delay(wait);
    /* ------------------------------ Turn off LEDs ----------------------------- */
    leds.SetPixelColor(0, black);
    leds.SetPixelColor(1, black);
    leds.SetPixelColor(2, black);
    leds.SetPixelColor(3, black);
    leds.Show();
    Serial.println("LEDs Off...");
    delay(wait);

    /* ------------------------ Set the colors using HSL ------------------------ */
    leds.SetPixelColor(0, hslRed);
    leds.SetPixelColor(1, hslGreen);
    leds.SetPixelColor(2, hslBlue);
    leds.SetPixelColor(3, hslWhite);
    leds.Show();
    Serial.println("Showing colors using HSL Values...");
    delay(wait);
    /* ------------------------------ Turn Off LEDs ----------------------------- */
    leds.SetPixelColor(0, hslBlack);
    leds.SetPixelColor(1, hslBlack);
    leds.SetPixelColor(2, hslBlack);
    leds.SetPixelColor(3, hslBlack);
    leds.Show();
    Serial.println("LEDs Off...");
    delay(wait);
    Serial.println("LED Test Complete");
    Serial.println("-----------------------------------------------");
  }
}


void ButtonTest()
{
  leds.ClearTo(black);
  leds.Show();
  Serial.println("Pattern Button Pressed");
  Serial.print("Pattern Number: ");
  Serial.println(currentPattern);
}


/* -------------------------------------------------------------------------- */
/*                              Pattern Functions                             */
/* -------------------------------------------------------------------------- */

/* ------------------------------ ChangePattern ----------------------------- */
void ChangePattern()
{
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > PATTERN_BUTTON_DEBOUNCE)
  {
    currentPattern++;
    if (currentPattern > NUMBER_OF_PATTERNS)
    {
      currentPattern = 0;
    }
  }
  Serial.println("Pattern Button Pressed");
  last_interrupt_time = interrupt_time;
  interruptLoop = false;
  /*
    * @todo Add logic to check for a long button press that will stop the loop and reset currentPattern to 0
    */
}


/* ----------------------------- Flash Section ------------------------------ */
void Flash(RgbwColor color, int numFlashes, int rangeStart, int rangeEnd, int flashDelay)
{
  int completedFlashes = 0;
  do
  {
    for (int i = rangeStart; i < rangeEnd; i++)
    {
      leds.SetPixelColor(i, color);
    }
    leds.Show();
    delay(flashDelay);
    leds.ClearTo(black);
    leds.Show();
    delay(flashDelay);
    completedFlashes++;
  }
  while (completedFlashes < numFlashes);
}


/* ------------------------------ Switch Colors ----------------------------- */
void SwitchColors()
{
/**
*@todo Add code for switching colors.
*/
}


/* --------------------- Alternating Two-Color and white -------------------- */
void Alternating_TwoColorWhite(RgbwColor color1, RgbwColor color2, int numFlashes, int flashDelay, bool switchColors)
{
  if (switchColors)
  {
    Flash(color1, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(white, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
    Flash(white, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(color2, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
    Flash(color2, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(white, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
    Flash(white, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(color1, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
  }
  else {
    Flash(color1, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(white, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
    Flash(white, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
    Flash(color2, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
  }
}


/* ------------------------ Alternating Single Color ------------------------ */
void Alternating_SingleColor(RgbwColor color, int flashDelay)
{
  for (int i = 0; i < NUMBER_OF_LEDS; i = i + 1)
  {
    leds.SetPixelColor(i, color);
  }
  for (int i = (NUMBER_OF_LEDS / 2); i < NUMBER_OF_LEDS; i = i + 1)
  {
    leds.SetPixelColor(i, color);
  }
  leds.Show();
  delay(flashDelay);

  for (int i = 0; i < NUMBER_OF_LEDS; i = i + 1)
  {
    leds.SetPixelColor(i, color);
  }
  for (int i = (NUMBER_OF_LEDS / 2); i < NUMBER_OF_LEDS; i = i + 1)
  {
    leds.SetPixelColor(i, color);
  }
  leds.Show();
  delay(flashDelay);
}


/* -------------------------- Alternating Two-Color ------------------------- */
void Alternating_TwoColor(RgbwColor color1, RgbwColor color2, int numFlashes, int flashDelay)
{
  Flash(color1, numFlashes, 0, NUMBER_OF_LEDS / 2, flashDelay);
  Flash(color2, numFlashes, NUMBER_OF_LEDS / 2, NUMBER_OF_LEDS, flashDelay);
}


/* -------------------------------- Steady On Single Color ------------------------------- */
void SteadyOn_SingleColor(RgbwColor color)
{
  for (int i = 0; i < NUMBER_OF_LEDS; i = i + 1)
  {
    leds.SetPixelColor(i, color);
  }
  leds.Show();
}


void effect1(RgbwColor color1, RgbwColor color2, int numFlashes)
{
  int numFlashes_color1 = 0;
  while (numFlashes_color1 < numFlashes)
  {
    for (int i = 0; i < (NUMBER_OF_LEDS / 2); i++)
    {
      leds.SetPixelColor(i, color1);
    }
    leds.Show();
    delay(FLASH_MEDIUM);

    for (int i = 0; i < (NUMBER_OF_LEDS / 2); i++)
    {
      leds.SetPixelColor(i, color2);
    }
    leds.Show();
    delay(FLASH_MEDIUM);

    numFlashes_color1++;
  }

  leds.ClearTo(black);
  leds.Show();
  delay(FLASH_MEDIUM);

  int numFlashes_color2 = 0;
  while (numFlashes_color2 < numFlashes)
  {
    for (int i = (NUMBER_OF_LEDS / 2); i < (NUMBER_OF_LEDS / 2) * 2; i++)
    {
      leds.ClearTo(black);
    }
    leds.Show();
    delay(FLASH_MEDIUM);

    for (int i = (NUMBER_OF_LEDS / 2); i < (NUMBER_OF_LEDS / 2) * 2; i++)
    {
      leds.SetPixelColor(i, color2);
    }
    leds.Show();
    delay(FLASH_MEDIUM);

    numFlashes_color2++;
  }

  leds.ClearTo(black);
  delay(FLASH_MEDIUM);
}



void disolve(int simultaneous, int cycles, int speed)
{
//   for (int i = 0; i < cycles; i++) {
//     for (int j = 0; j < simultaneous; j++) {
//       int idx = random(NUMBER_OF_LEDS);
//       leds[idx] = Black;
//     }
// leds.Show();
//     delay(speed);
//   }

//   allColor(Black);
}


void LightningFlash(RgbwColor color, int simultaneous, int cycles, int speed)
{
  int flashes[simultaneous];

  for (int i = 0; i < cycles; i++) {
    for (int j = 0; j < simultaneous; j++) {
      int idx = random(NUMBER_OF_LEDS);
      flashes[j] = idx;
      leds.SetPixelColor(idx, color);
    }
    leds.Show();
    delay(speed);
    for (int s = 0; s < simultaneous; s++) {
      leds.SetPixelColor(flashes[s], black);
    }
    delay(speed);
  }
}


void Cylon(RgbwColor color, int width, int speed)
{
// First slide the leds in one direction
  for (int i = 0; i <= NUMBER_OF_LEDS - width; i++) {
    for (int j = 0; j < width; j++) {
      leds.SetPixelColor(i + j, color);
    }

    leds.Show();

                    // now that we've shown the leds, reset to Black for next loop
    for (int j = 0; j < 5; j++) {
      leds.SetPixelColor(i + j, black);
    }
    delay(speed);
  }

  // Now go in the other direction.
  for (int i = NUMBER_OF_LEDS - width; i >= 0; i--) {
    for (int j = 0; j < width; j++) {
      leds.SetPixelColor(i + j, color);
    }
    leds.Show();
    for (int j = 0; j < width; j++) {
      leds.SetPixelColor(i + j, black);
    }

    delay(speed);
  }
}


/* -------------------------------------------------------------------------- */
/*                             Directional Arrows                             */
/* -------------------------------------------------------------------------- */

/* ---------------------------- Right Directional --------------------------- */
void ArrowRight(RgbwColor color, int flashDelay)
{
  for (int i = 0; i < NUMBER_OF_LEDS; i++)
  {
    leds.SetPixelColor(i, color);
    leds.Show();
    delay(flashDelay);
    leds.ClearTo(black);
  }
}


/* ---------------------------- Left Directional ---------------------------- */
void ArrowLeft(RgbwColor color, int flashDelay)
{
  for (int i = NUMBER_OF_LEDS; i > 0; i--)
  {
    leds.SetPixelColor(i, color);
    leds.Show();
    delay(flashDelay);
    leds.ClearTo(black);
  }
}


/* ------------------------------- Center Out ------------------------------- */
void ArrowCenter(RgbwColor color, int flashDelay)
{
  int j = 1;
  int HALFWAY = (NUMBER_OF_LEDS / 2) - 1;

  for (int i = HALFWAY; i < NUMBER_OF_LEDS; i++)
  {
    leds.SetPixelColor(HALFWAY - j, color);
    leds.SetPixelColor(i, color);
    j++;
    leds.Show();
    delay(flashDelay);
    if (j == HALFWAY)
    {
      j = 1;
    }
  }

  j = 1;
  HALFWAY = (NUMBER_OF_LEDS / 2) - 1;

  for (int i = HALFWAY; i < NUMBER_OF_LEDS; i++)
  {
    leds.SetPixelColor(HALFWAY - j, black);
    leds.SetPixelColor(i, black);
    j++;
    leds.Show();
    delay(flashDelay);
    if (j == HALFWAY)
    {
      j = 1;
    }
  }
}
