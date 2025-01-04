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
#include <Buttons.h>           //Button Images

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
    tft.resetViewport();
    //tft.begin();

    //TJpg Decoder
     // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
    //TJpgDec.setJpgScale(1);

    // The decoder must be given the exact name of the rendering function above
    //TJpgDec.setCallback(tft_output);

    //Temp
    tft.setRotation(1);
    tft.fillScreen(toInt(30, 215, 96));
    delay(200);
    tft.fillScreen(TFT_BLACK);
    delay(200);
    tft.fillScreen(TFT_RED);
    delay(200);
    tft.fillScreen(TFT_BLUE);
    delay(200);
    tft.fillScreen(TFT_GREEN);
    delay(200);
    tft.fillScreen(TFT_WHITE);
    delay(200);
    tft.fillScreen(TFT_BLACK);
    delay(200);
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
    tft.drawString("to your network. The device will restart!", (tft.width() / 2), 280);

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
        renderJPEG(8, 8); // Render the image on the TFT screen
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

//'Helper' Functions
void mirrorImageHorizontally(const uint16_t* src, uint16_t* dest, int width, int height) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      // Reverse the horizontal position of each pixel
      dest[y * width + (width - 1 - x)] = src[y * width + x];
    }
  }
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

void drawCurrentPlaying(String title, String artist, String url, String duration_ms, String progress_ms, String account_name, bool playing, bool liked){
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


  //Draw account name
 // tft.pushImage(218, 11, 20, 20, spotify_logo);
  //tft.setTextColor(0xffff);
  //tft.setTextSize(3);
  //tft.drawString(account_name.c_str(), 248, 11);

  //Draw volume knob decal

  //Draw Song Name
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(3);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(title.c_str(), 218, 160);
  wrapText(title.c_str(), 218, 140, 168);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setTextDatum(BL_DATUM);
  tft.drawString(artist.c_str(), 218, 200);

  //Draw progress bar




  //Draw buttons
  int button_width = 32;
  int button_height = 32;

  //Playlist

  //Skip Backwards
  uint16_t mirroredNav[button_width * button_height];
  mirrorImageHorizontally(button_nav, mirroredNav, button_width, button_height);
  tft.pushImage((tft.width()/2) - 113, 270, button_width, button_height, mirroredNav);

  //Play Pause
  if(playing){
    //Music is Playing (Display Pause)
    tft.pushImage((tft.width()/2) - 16, 270, button_width, button_height, button_pause);
  } else {
    //Music is not playing (Display Play)
    tft.pushImage((tft.width()/2) - 16, 270, button_width, button_height, button_play);
  }

  //Skip Forward
  tft.pushImage((tft.width()/2) + 97, 270, button_width, button_height, button_nav);

  //Like/Add Song
  if(liked){
    //Song is liked (Green Tick)
    //size_t pixelCount = sizeof(button_liked_song) / sizeof(button_liked_song[0]);
    //swapImageEndianness(button_liked_song, pixelCount);

    tft.pushImage((tft.width()/2) + 191 , 270, button_width, button_height, button_liked_song);
  } else {
    //Song is unliked (Plus)
    tft.pushImage((tft.width()/2) + 191, 270, button_width, button_height, button_liked_song);
  }

  //Draw button arrows (ButtonIdentifiers.h)
  int imageWidth = 40;
  int imageHeight = 15;
  int pixelCount = imageWidth * imageHeight;

  tft.pushImage(17, 303, imageWidth, imageHeight, button_identifier_arrow); //PL
  tft.pushImage((tft.width()/2) - 20, 303, imageWidth, imageHeight, button_identifier_arrow); //PP
  tft.pushImage((tft.width()/2) - 117, 303, imageWidth, imageHeight, button_identifier_arrow); //SB
  tft.pushImage((tft.width()/2) + 93, 303, imageWidth, imageHeight, button_identifier_arrow); //SF
  tft.pushImage((tft.width()/2) + 187, 303, imageWidth, imageHeight, button_identifier_arrow); //AS
}