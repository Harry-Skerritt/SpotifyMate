#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Graphics.h>
//#include <TJpg_Decoder.h>
#include <FS.h>
#include <HTTPClient.h>
#include <JPEGDecoder.h>
#include <SpotifyAuth.h>
#include <qrcode.h>
#include <smLogo.h>

TFT_eSPI tft = TFT_eSPI(); //Create the screen

int32_t toInt(int r, int g, int b) {
			
			return (((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
		}


//Spotify Black 18 18 18
//Spotify White 255, 255, 255
//Spotify Green 30 215 96

void initialiseGraphics() {

    tft.init(); //Initialise the screen
    tft.resetViewport();
    //tft.begin();

    //TJpg Decoder
     // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    //TJpgDec.setJpgScale(1);

    // The decoder must be given the exact name of the rendering function above
    //TJpgDec.setCallback(tft_output);

    //Temp
    tft.setRotation(0); //Portrait
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

    tft.pushImage(130, 90, 60, 60, sm_logo);

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

//Convert from 24bit rgb to 16bit (RGB565)
uint16_t convertToRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// Function to render JPEG on the TFT screen
void renderJPEG(int xpos, int ypos) {
   uint16_t *pImage;

    // Loop through each row of the image
    for (uint16_t row = 0; row < JpegDec.height; row++) {
        if (JpegDec.read()) {
            pImage = JpegDec.pImage;

            // Loop through each pixel in the row
            for (int i = 0; i < JpegDec.width; i++) {
                uint8_t r = pImage[i * 3];       // Red
                uint8_t g = pImage[i * 3 + 1];   // Green
                uint8_t b = pImage[i * 3 + 2];   // Blue
                pImage[i] = convertToRGB565(r, g, b);  // Convert and store in RGB565
            }

            // Render each pixel as a 2x2 block for scaling to 128x128
            for (int i = 0; i < JpegDec.width; i++) {
                uint16_t pixel = pImage[i]; // Get the RGB565 pixel
                
                // Create a 2x2 block of the same pixel for horizontal and vertical scaling
                uint16_t doublePixel[9] = {pixel, pixel, pixel, pixel, pixel, pixel, pixel, pixel, pixel}; // Duplicate for 2x2 scaling
                
                // Push the 2x2 block of pixels to the screen (doubled horizontally and vertically)
                tft.pushImage(xpos + i * 3, ypos + row * 3, 3, 3, doublePixel);
            }
        }
    }
}

// Download and decode album art
void displayAlbumArt(String albumArtUrl) {
  Serial.println("Width: " + String(tft.width()));
  Serial.println("Height: " + String(tft.height()));

  HTTPClient http;
  http.begin(albumArtUrl);

  int httpCode = http.GET();
  if (httpCode == 200) {
    int contentLength = http.getSize();
    WiFiClient* stream = http.getStreamPtr();
    uint8_t *imgBuffer = (uint8_t*)malloc(contentLength);

    if (imgBuffer) {
      // Read the stream into the buffer
      int bytesRead = stream->readBytes(imgBuffer, contentLength);

      // Decode the JPEG from the buffer
      if (JpegDec.decodeArray(imgBuffer, bytesRead)) {
        Serial.println("JPEG decoded successfully!");
        renderJPEG(24, 10); // Render the image on the TFT screen
      } else {
        Serial.println("Failed to decode JPEG.");
      }

      free(imgBuffer); // Free the allocated memory
    } else {
      Serial.println("Failed to allocate memory for image buffer.");
    }
  } else {
    Serial.println("Error fetching album art.");
  }
  http.end();
}




int convertMillisSec(String millis_s){
  int millis = millis_s.toInt();
  int seconds = (millis/1000)%60;
  

  seconds = floor(seconds);
  return seconds;
}

int convertMillisMin(String millis_s){
  int millis = millis_s.toInt();
  int minutes = (millis/(1000*60))%60;
  minutes = floor(minutes);

  return minutes;

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

// Function to wrap text within a specified width
void wrapText(const char* text, int x, int y, int maxWidth) {
  char buffer[100];  // Temporary buffer to store a word or line of text
  int lineStart = 0;  // Track the start of each line
  int textLength = strlen(text);  // Length of the input text
  int cursorX = x;  // Current x position for the text

  // Start displaying the text
  tft.setCursor(cursorX, y);

  for (int i = 0; i < textLength; i++) {
    // Check if adding the current character will exceed the width
    int wordWidth = tft.textWidth(text + lineStart, i - lineStart + 1); // width of current word

    // Check if adding this character exceeds maxWidth
    if (wordWidth > maxWidth) {
      // Print the line of text and move to next line
      tft.println(text + lineStart);
      cursorX = x;  // Reset cursor to the start of the next line
      lineStart = i;  // Move the starting index for the next line
      tft.setCursor(cursorX, y + tft.fontHeight()); // Adjust cursor to the next line
    }
  }

  // Print the remaining text
  tft.print(text + lineStart);
}

String currentlyPlayingURL = "";

void drawCurrentPlaying(String title, String artist, String url, String progress_ms, String duration_ms, String account_name, bool playing, bool liked){
  Serial.begin(115200);
  //Get duration in terms of minutes and seconds
  int progress_s = convertMillisSec(progress_ms);
  int progress_m = convertMillisMin(progress_ms);
  int duration_s = convertMillisSec(duration_ms);
  int duration_m = convertMillisMin(duration_ms);

  tft.fillScreen(0x0000); //Set background

  //Draw Album Art
  if(url == currentlyPlayingURL){
    //The song currently playing is the same as last time a call was made
    //No need to redraw the image

    currentlyPlayingURL = url;
  } else {
    //The song currently playing is not what was playing last time the call was made
    //Redraw the image
  
    displayAlbumArt(url);
    currentlyPlayingURL = url;
  }


  //Draw Song Name
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(title.c_str(), tft.width()/2, 206);
  //wrapText(title.c_str(), tft.width()/2, 206, 128);

  tft.setTextColor(toInt(155, 155, 155));
  tft.setTextSize(2);
  tft.setTextDatum(TC_DATUM);
  tft.drawString(artist.c_str(), tft.width()/2, 256);

  //Draw progress bar
  tft.setTextColor(toInt(255, 255, 255));
  tft.setTextSize(1);

  tft.setTextDatum(TL_DATUM);
  tft.drawString(String(progress_m + ":"+ progress_s).c_str(), 10, 280); //Progress

  tft.setTextDatum(TR_DATUM);
  tft.drawString(String(duration_m + ":"+ duration_s).c_str(), (tft.width()-10), 280); //Duration
  
  //Actual Bar
  drawProgressBar(10, 298, 220, 12, progress_ms.toInt(), duration_ms.toInt());



}