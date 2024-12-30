#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Graphics.h>
#include <TJpg_Decoder.h>
#include <FS.h>
#include "LittleFS.h"
#include "metropolis-reg.h"


TFT_eSPI tft = TFT_eSPI(); //Create the screen

int32_t toInt(int r, int g, int b) {
			
			return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
		}


//Spotify Black 18 18 18
//Spotify White 255, 255, 255
//Spotify Green 30 215 96

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

void initialiseGraphics() {

    tft.init(); //Initialise the screen
    tft.begin();

    //Todo: Fix this
    //Initialise LittleFS
    if(!LittleFS.begin()) {
        Serial.println("LittleFS Init Failed!");
    }
    Serial.println("\r\nInit Done - LFS");

    //TJpg Decoder
     // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    TJpgDec.setJpgScale(1);

    // The decoder must be given the exact name of the rendering function above
    TJpgDec.setCallback(tft_output);

    //Temp
    tft.setRotation(1);
    tft.fillScreen(toInt(30, 215, 96));

}

void startupGraphics(String msg) {
    tft.fillScreen(toInt(18, 18, 18));


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(5);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Spotify", (tft.width() / 2)-65, tft.height() / 2);


    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(5);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Mate", (tft.width() / 2) + 110, tft.height() / 2);


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(msg, tft.width() / 2, 250);

}

void captiveGraphics(const char *ssid) {
    tft.fillScreen(toInt(18, 18, 18));


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(5);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Spotify", (tft.width() / 2)-65, 30);


    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(5);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Mate", (tft.width() / 2) + 110, 30);

    tft.setTextColor(toInt(255, 255, 255));
    tft.setTextSize(2);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Welcome to SpotifyMate", (tft.width() /2), 90);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Open WiFi settings on your phone,", (tft.width() / 2), 120);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("and connect to:", (tft.width() / 2), 150);

    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(4);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(ssid, tft.width()/2, (tft.height()/2) + 40);

    tft.setTextColor(toInt(255, 255, 255));
    tft.setTextSize(2);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("Follow the instructions to connect", (tft.width() / 2), 250);

    tft.setTextDatum(MC_DATUM);
    tft.drawString("to your network, and link with Spotify", (tft.width() / 2), 280);

    



}