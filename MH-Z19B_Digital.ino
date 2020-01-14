#include <M5Stack.h>

uint8_t readCommand[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
uint8_t resetCommand[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
uint8_t response[9] = {};
uint8_t pastData[320];
float measuredPPM = 400;

extern const unsigned short BGPicture[];
void drawNowPPMData(uint16_t);

void setup()
{
  M5.begin();
  Serial2.begin(9600);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextFont(4);
  M5.Lcd.setTextSize(2);
}

void loop()
{
  // Calibration
  if (M5.BtnB.isPressed())
  {
    Serial2.write(resetCommand, 9);
    M5.Lcd.fillScreen(TFT_BLUE);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("Resetting...", 0, 0);
    delay(5000);
    M5.Power.reset();
  }
  else
  {
    Serial2.write(readCommand, 9);
  }
  Serial2.readBytes(response, 9);

  if (response[0] == 0xFF && response[1] == 0x86)
  {
    measuredPPM = measuredPPM * 0.9 + ((256 * response[2]) + response[3]) * 0.1;
    drawNowPPMData((uint16_t)measuredPPM);
  }
  else
  {
    M5.Lcd.fillScreen(TFT_BLUE);
    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.drawString("Invalid Response!", 0, 100);
    delay(5000);
    M5.Power.reset();
  }

  M5.update();
  delay(5000);
}

// Draw Process
void drawNowPPMData(uint16_t ppm)
{
  M5.Lcd.fillScreen(TFT_BLACK);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.drawString("CO2 Meter", 0, 0);
  M5.Lcd.drawString("ppm", 215, 80);

  if (ppm >= 1500)
  {
    M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.drawString("Warning!", 10, 160);
  }
  else if (ppm >= 1000 && ppm < 1500)
  {
    M5.Lcd.setTextColor(TFT_ORANGE);
  }
  else if (ppm >= 500 && ppm < 1000)
  {
    M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.drawString("Good", 10, 160);
  }
  else if (ppm < 500)
  {
    M5.Lcd.setTextColor(TFT_BLUE);
    M5.Lcd.drawString("Good", 10, 160);
  }
  M5.Lcd.drawNumber(ppm, 0, 60, 6);

  // Draw Graph
  for (int i = 0; i < 319; i++)
  {
    pastData[i] = pastData[i + 1];
  }
  pastData[319] = (uint8_t)(ppm / 20);

  for (int i = 0; i < 320; i++)
  {
    M5.Lcd.drawPixel(i, 230 - pastData[i] + 20, WHITE);
  }
}
