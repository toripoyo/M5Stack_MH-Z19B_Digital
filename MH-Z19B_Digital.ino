#include <M5Stack.h>

const uint8_t kCommandLen = 9;
const uint8_t kResponseLen = 9;
const uint8_t kReadCommand[kCommandLen] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t kResetCommand[kCommandLen] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

// BackGround Picture Data
extern const unsigned short BGPicture[];

// Function Define
void drawPPMVal(uint16_t);
void drawBlueScreen(String);

void setup()
{
  M5.begin();
  Serial2.begin(9600);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextFont(4);

  // Calibration
  M5.update();
  if (M5.BtnB.isPressed())
  {
    Serial2.write(kResetCommand, kCommandLen);
    drawBlueScreen("Resetting...");
    delay(5000);
    M5.Power.reset();
  }
}

void loop()
{
  uint8_t response[kResponseLen] = {};

  // Read Current ppm
  Serial2.write(kReadCommand, kCommandLen);
  Serial2.readBytes(response, kResponseLen);

  // resnponse is ok
  if (response[0] == 0xFF && response[1] == 0x86)
  {
    uint16_t meas_PPM = (256 * response[2]) + response[3];
    drawPPMVal(meas_PPM);
    delay(1000);
  }
  else
  {
    drawBlueScreen("Invalid Response!");
    delay(3000);
    M5.Power.reset();
  }
}

// ---------------------------------------------------------------------

// Draw Blue Screen
void drawBlueScreen(String s)
{
  M5.Lcd.fillScreen(TFT_BLUE);
  M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.drawString(s, 0, 100);
}

// Draw Now Data
void drawPPMVal(uint16_t ppm)
{
  const uint16_t x_offset = 40;
  static uint16_t ppm_old = 0;

  if (ppm != ppm_old)
  {
    ppm_old = ppm;

    M5.Lcd.setTextColor(TFT_WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.drawString("CO2 Meter", 0, 0);
    M5.Lcd.setTextSize(2);
    M5.Lcd.drawString("ppm", 220, 160);

    if (ppm >= 1500)
    {
      M5.Lcd.setTextColor(TFT_RED);
      M5.Lcd.drawString("Warning!", 5, 160);
    }
    else if (ppm >= 1000 && ppm < 1500)
    {
      M5.Lcd.setTextColor(TFT_ORANGE);
    }
    else if (ppm >= 500 && ppm < 1000)
    {
      M5.Lcd.setTextColor(TFT_GREEN);
    }
    else if (ppm < 500)
    {
      M5.Lcd.setTextColor(TFT_BLUE);
    }
    M5.Lcd.fillRect(30, 50, 240, 100, TFT_BLACK);
    M5.Lcd.drawNumber(ppm, 40, 60, 6);
  }
}
