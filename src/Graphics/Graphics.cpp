#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Graphics.h>
#include <FS.h>
#include <HTTPClient.h>
#include <SpotifyAuth.h>
#include <qrcode.h>
#include <TJpg_Decoder.h>
#include <LittleFS.h>
#include "Web_Fetch.h"
#include "List_LittleFS.h"

TFT_eSPI tft = TFT_eSPI(); //Create the screen

int32_t toInt(int r, int g, int b) {
			
			return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
		}


//Spotify Black 18 18 18
//Spotify White 255, 255, 255
//Spotify Green 30 215 96

//JPG Try 2
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
  // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // Return 1 to decode next block
  return 1;
}

void initialiseGraphics() {

    tft.init(); //Initialise the screen
    tft.resetViewport();

    tft.setRotation(0); //Portrait



    //Jp2
    if(!LittleFS.begin()){
      Serial.println("LitleFS Init Failed");
      while(1) yield();
    }
    Serial.println("LittleFS Initialised");


    TJpgDec.setJpgScale(4);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);
}

void startupGraphics(String msg) {
    tft.fillScreen(toInt(18, 18, 18));


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3.2);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Spotify", 9, 46);


    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(3.2);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Mate", (tft.width() - 9), 46);

    //tft.pushImage(130, 90, 60, 60, sm_logo);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1.25);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(msg, tft.width() / 2, 250);

}

void captiveGraphics(const char *ssid) {
    tft.fillScreen(toInt(18, 18, 18));


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3.2);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Spotify", 9, 12);


    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(3.2);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Mate", (tft.width() - 9), 12);

    tft.setTextColor(toInt(255, 255, 255));
    tft.setTextSize(1);

    tft.setTextDatum(TC_DATUM);
    tft.drawString("Welcome to SpotifyMate", (tft.width() /2), 59);

    tft.setTextDatum(TC_DATUM);
    tft.drawString("Open WiFi settings on your phone,", (tft.width() / 2), 87);

    tft.setTextDatum(TC_DATUM);
    tft.drawString("and connect to:", (tft.width() / 2), 100);

    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(2.5);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(ssid, tft.width()/2, 137);

    tft.setTextColor(toInt(255, 255, 255));
    tft.setTextSize(1);

    tft.setTextDatum(TC_DATUM);
    tft.drawString("Follow the instructions to connect to", (tft.width() / 2), 184);

    tft.setTextDatum(TC_DATUM);
    tft.drawString("your network. The device will restart!", (tft.width() / 2), 194);

    //Logo??

}

void spotifyConnectScreen() {
    tft.fillScreen(toInt(18, 18, 18));


    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Scan to link with Spotify", (tft.width() / 2)-65, 30);

    QRcode qrcode(&tft);
    qrcode.init();
    qrcode.create(getAuthURL());
    //Doesn't scan :/ Fix Later
    


}


String msToFormattedTime(int milliseconds_in){
  int totalSeconds = milliseconds_in / 1000;
  int resultMinutes = totalSeconds / 60;
  int resultSeconds = totalSeconds % 60;

  resultMinutes = floor(resultMinutes);
  resultSeconds = floor(resultSeconds);

  String formattedTime;
  formattedTime = String(resultMinutes) + ":" + String(resultSeconds);

  Serial.begin(115200);
  Serial.println(formattedTime);

  return formattedTime;
}

// Draw the progress bar
void drawProgressBar(int x, int y, int width, int height, int currentTime, int duration) {
  // Calculate progress percentage
  float progress = (float)currentTime / duration;

  // Ensure progress is clamped between 0.0 and 1.0
  if (progress < 0.0) progress = 0.0;
  if (progress > 1.0) progress = 1.0;

  // Calculate the filled width based on progress
  int filledWidth = progress * width;

  // Draw the background of the progress bar
  tft.fillRect(x, y, width, height, toInt(24, 24, 24));

  // Draw the filled portion of the progress bar
  tft.fillRect(x, y, filledWidth, height, toInt(30, 215, 96));
  
  // Optionally, draw a border around the progress bar
  tft.drawRect(x, y, width, height, TFT_WHITE);
}

//18 Characters

void getAlbumArt(String url){

  //listLittleFS();

  if(LittleFS.exists("/album.jpg") == true){
    Serial.println("Removing File!");
    LittleFS.remove("/album.jpg");
  }

  bool loaded_ok = getFile(url, "/album.jpg");

  //listLittleFS();

  TJpgDec.setJpgScale(4);
  TJpgDec.drawFsJpg(43, 16, "/album.jpg", LittleFS);


}

String currentlyPlayingURL = "";

void drawCurrentPlaying(String title, String artist, String url, int progress_ms, int duration_ms, bool explicit_song){
  Serial.begin(115200);
  //Get duration in terms of minutes and seconds
  Serial.println("P: " + String(progress_ms));
  Serial.println("D: " + String(duration_ms));
  
  String progressTime = msToFormattedTime(progress_ms);
  String durationTime = msToFormattedTime(duration_ms);

  tft.fillScreen(0x0000); //Set background

  //Draw Album Art
  if(url != currentlyPlayingURL){
    //The song currently playing is not what was playing last time the call was made
    //Redraw the image

    getAlbumArt(url);
    currentlyPlayingURL = url;
  } else {
    TJpgDec.drawFsJpg(43, 16, "/album.jpg", LittleFS);
  }

//EDIT THE README

  //Draw Song Name
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(TC_DATUM);


  if(title.length() > 20){
    //More than 20 chars
    //Wrap Around
    tft.setTextWrap(true, false);
    tft.setCursor(0, 196);
    tft.print(title.c_str());
  } else {
    //Less than 20
    tft.drawString(title.c_str(), tft.width()/2, 196);
  }
  
  
  

  tft.setTextColor(toInt(155, 155, 155));
  tft.setTextSize(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(artist.c_str(), tft.width()/2, 246);

  //Draw progress bar
  tft.setTextColor(toInt(255, 255, 255));
  tft.setTextSize(1);

  tft.setTextDatum(TL_DATUM);
  tft.drawString(progressTime, 10, 280); //Progress

  tft.setTextDatum(TR_DATUM);
  tft.drawString(durationTime, (tft.width()-10), 280); //Duration
  
  //Actual Bar
  drawProgressBar(10, 298, 220, 12, progress_ms, duration_ms);

  //If Explicit
  if(explicit_song){
    TJpgDec.setJpgScale(1);
    TJpgDec.drawFsJpg(211, 156, "/explicit.jpg", LittleFS);
    //Need to make sure it changes 
  }
  



}