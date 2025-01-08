#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <Graphics.h>
#include <FS.h>
#include <HTTPClient.h>
#include <SpotifyAuth.h>
#include <TJpg_Decoder.h>
#include <JPEGDecoder.h> //For Average Colour
#include <LittleFS.h>
#include "Web_Fetch.h"
#include "List_LittleFS.h"
#include <Main.h>

TFT_eSPI tft = TFT_eSPI(); //Create the screen

//Sprite for the progress bar
TFT_eSprite progressSprite = TFT_eSprite(&tft);

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

  // Return 1 to decode next block
  return 1;
}

void initialiseGraphics() {
    //Init Screen
    tft.init(); //Initialise the screen
    tft.resetViewport();

    tft.setRotation(0); //Portrait

    if(!LittleFS.begin()){
      Serial.println("LitleFS Init Failed");
      while(1) yield();
    }
    Serial.println("LittleFS Initialised");


    TJpgDec.setJpgScale(4);
    TJpgDec.setSwapBytes(true);
    TJpgDec.setCallback(tft_output);

    progressSprite.createSprite(tft.width(), 50); //Progress bar
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
    TJpgDec.setJpgScale(1);
    TJpgDec.drawFsJpg(70, 113, "/SM_100x100.jpg", LittleFS);
    TJpgDec.setJpgScale(4);

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
  //MAKE WORK
    tft.fillScreen(toInt(18, 18, 18));

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(3.2);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Spotify", 9, 46);


    tft.setTextColor(toInt(30, 215, 96));
    tft.setTextSize(3.2);
    tft.setTextDatum(TR_DATUM);
    tft.drawString("Mate", (tft.width() - 9), 46);
  

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Scan to link with Spotify", (tft.width() / 2)-65, 59);

    String authURL = getAuthURL();
    String QRCodeURL = "https://quickchart.io/qr?text=" + authURL + 
    "&dark=%231ed760&light=%23121212&centerImageUrl=https://raw.githubusercontent.com/Harry-Skerritt/test/refs/heads/main/SM%20100x100.jpg&size=50";

    listLittleFS();
    
    if(LittleFS.exists("/qrcode.jpg") == true){
      Serial.println("Removing File!");
      LittleFS.remove("/qrcode.jpg");
    }

  bool loaded_ok = getFile(QRCodeURL, "/qrcode.jpg");

  //listLittleFS();

  TJpgDec.setJpgScale(2);
  TJpgDec.drawFsJpg(38, 89, "/qrcode.jpg", LittleFS);

    //https://accounts.spotify.com/authorize?response_type=code%26client_id=517041868f9544a0bd757e847ffa3256%26scope=user-read-currently-playing user-read-playback-state user-modify-playback-state%26redirect_uri=http://spotify-mate.local/callback
    

    //QRcode qrcode(&tft);
    //qrcode.init();
    //qrcode.create(getAuthURL());
    //Doesn't scan :/ Fix Later
    


}


String msToFormattedTime(int milliseconds_in){
  int totalSeconds = milliseconds_in / 1000;
  int resultMinutes = totalSeconds / 60;
  int resultSeconds = totalSeconds % 60;

  resultMinutes = floor(resultMinutes);
  resultSeconds = floor(resultSeconds);

  String resultSeconds_s = String(resultSeconds);
  if(resultSeconds < 10){
    //Add leading zeros
    resultSeconds_s = "0" + resultSeconds_s;
  }

  String formattedTime;
  formattedTime = String(resultMinutes) + ":" + resultSeconds_s;

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
  progressSprite.fillRect(x, y, width, height, toInt(24, 24, 24));

  // Draw the filled portion of the progress bar
  progressSprite.fillRect(x, y, filledWidth, height, toInt(30, 215, 96));
  
  // Optionally, draw a border around the progress bar
  progressSprite.drawRect(x, y, width, height, TFT_WHITE);
}

void drawProgressSprite(int progress_ms, int duration_ms, uint8_t avgR, uint8_t avgG, uint8_t avgB){
  //Get duration in terms of minutes and seconds
  Serial.println("P: " + String(progress_ms));
  Serial.println("D: " + String(duration_ms));
  
  String progressTime = msToFormattedTime(progress_ms);
  String durationTime = msToFormattedTime(duration_ms);

  //Check if its empty if its transparent
  progressSprite.fillRect(0, 0, progressSprite.width(), progressSprite.height(), toInt(avgR, avgG, avgB)); //Fill the sprite before drawing <- Will need to change if the avg colour works

  //Draw progress bar
  progressSprite.setTextColor(toInt(255, 255, 255));
  progressSprite.setTextSize(1);

  progressSprite.setTextDatum(TL_DATUM);
  progressSprite.drawString(progressTime, 10, 18); //Progress

  progressSprite.setTextDatum(TR_DATUM);
  progressSprite.drawString(durationTime, (tft.width()-10), 18); //Duration
  
  //Actual Bar
  drawProgressBar(10, 29, 220, 12, progress_ms, duration_ms);

  progressSprite.pushSprite(0, tft.height() - 50);
}

//18 Characters

//Could be the cause of errors -> Not sure
bool getAverageColorFromFile(const char* filePath, uint8_t& avgR, uint8_t& avgG, uint8_t& avgB) {
    // Initialize LittleFS only once
    static bool fsInitialized = false;
    if (!fsInitialized) {
        if (!LittleFS.begin()) {
            Serial.println("Failed to initialize LittleFS");
            return false;
        }
        fsInitialized = true;
    }

    // Open the file
    File file = LittleFS.open(filePath, "r");
    if (!file) {
        Serial.printf("Failed to open file: %s\n", filePath);
        return false;
    }

    // Decode the JPEG file
    if (!JpegDec.decodeFsFile(file)) {
        Serial.println("Failed to decode JPEG image");
        file.close();
        return false;
    }

    uint32_t sumR = 0, sumG = 0, sumB = 0;
    uint32_t pixelCount = 0;

    // Process the image
    while (JpegDec.read()) { // Read each MCU block
        uint16_t* pImg = JpegDec.pImage;
        for (uint16_t y = 0; y < JpegDec.MCUHeight; y++) {
            uint16_t offsetY = JpegDec.MCUy * JpegDec.MCUHeight + y;
            if (offsetY >= JpegDec.height) break;

            for (uint16_t x = 0; x < JpegDec.MCUWidth; x++) {
                uint16_t offsetX = JpegDec.MCUx * JpegDec.MCUWidth + x;
                if (offsetX >= JpegDec.width) break;

                uint16_t color = pImg[y * JpegDec.MCUWidth + x];
                uint8_t r = (color & 0xF800) >> 8;
                uint8_t g = (color & 0x07E0) >> 3;
                uint8_t b = (color & 0x001F) << 3;

                sumR += r;
                sumG += g;
                sumB += b;
                pixelCount++;
            }
        }
        yield();
    }

    // Clean up
    file.close();
    JpegDec.abort(); // Properly release decoder resources

    if (pixelCount == 0) {
        Serial.println("No pixels were processed");
        return false;
    }

    // Calculate average color
    avgR = sumR / pixelCount;
    avgG = sumG / pixelCount;
    avgB = sumB / pixelCount;

    // Compute brightness using a common formula
    float brightness = 0.2126f * avgR + 0.7152f * avgG + 0.0722f * avgB;

    // Threshold for white or near-white (255 is full white)
    const float whiteThreshold = 220.0f; // Adjust as needed
    if (brightness > whiteThreshold) {
        // Make the color darker
        avgR *= 0.8; // Reduce by 20%
        avgG *= 0.8;
        avgB *= 0.8;
        Serial.println("Color was near white, adjusted to a darker equivalent.");
    } else {
        // Make it 10% darker as usual
        avgR *= 0.9;
        avgG *= 0.9;
        avgB *= 0.9;
    }

    return true;
}

const char* img_path = "/album.jpg";
void getAlbumArt(String url, uint8_t &avgR, uint8_t &avgG, uint8_t &avgB){
  //listLittleFS();

  if(LittleFS.exists(img_path) == true){
    Serial.println("Removing File!");
    LittleFS.remove(img_path);
    yield();
  }

  bool loaded_ok = getFile(url, img_path);
  yield();

  //listLittleFS();

  //Get Average Colour
  if(getAverageColorFromFile(img_path, avgR, avgG, avgB)){
    //Gets the average colour
    tft.fillScreen(toInt(avgR, avgG, avgB)); //Average Colour
  } else {
    //Doesn't get the average colour
    tft.fillScreen(0x0000); //Black
  }


  TJpgDec.setJpgScale(4);
  TJpgDec.drawFsJpg(43, 16, img_path, LittleFS);
  yield();
}


String currentlyPlayingURL = "";
String nextSongURL = "";
String currentSongTitle = "";
uint8_t avgR = 0, avgG = 0, avgB = 0;

void drawCurrentPlaying(String title, String artist, String url, int progress_ms, int duration_ms, bool explicit_song){
  Serial.begin(115200);
  

  if(url == currentlyPlayingURL && currentSongTitle == title){
    //The same song is playing as before
    //Dont update anything but the sprite
    drawProgressSprite(progress_ms, duration_ms, avgR, avgG, avgB);
  } else {
    //The song has changed - change everything!
    tft.fillScreen(0x0000); //Set background

    //Album Art
    if(url == nextSongURL){
      //The already cached song is playing
      //Load the cached image then continue
      TJpgDec.setJpgScale(4);
      TJpgDec.drawFsJpg(43, 16, img_path, LittleFS);
      yield();
      currentlyPlayingURL = url;
    } else {
      // A different song is playing
      getAlbumArt(url, avgR, avgG, avgB);
      yield();
      currentlyPlayingURL = url;
    }

    //Draw Song Name
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);
    tft.setTextDatum(TC_DATUM);

    currentSongTitle = title;


    if(TRUNCTUATE_REMASTER_TITLES){
      //If to remove '- XXXX Remaster' from titles
      std::string temp_title = title.c_str();
      int substring_length;
      if((substring_length = temp_title.find("-")) != std::string::npos){
        //The Substring is present
        temp_title = temp_title.substr(0, substring_length);
        title = temp_title.c_str();
      } else{
        //Not present -> Continue with string as it
        title = title;
      }
    }

    if(title.length() > 20){
      //More than 20 chars
      //Wrap Around
      tft.setTextWrap(true, false);
      tft.setCursor(0, 196);
      tft.print(title.c_str());
      yield();
    } else {
      //Less than 20
      tft.drawString(title.c_str(), tft.width()/2, 196);
    }  
  
    tft.setTextColor(0xffff); //toInt(155, 155, 155)
    if(artist.length() > 20){
      tft.setTextSize(1);
      tft.setTextDatum(TC_DATUM);
      tft.drawString(artist.c_str(), tft.width()/2, 246);
    } else {
      tft.setTextSize(2);
      tft.setTextDatum(TC_DATUM);
      tft.drawString(artist.c_str(), tft.width()/2, 246);
    }

    //If Explicit
    if(explicit_song == true){
      TJpgDec.setJpgScale(1);
      TJpgDec.drawFsJpg(211, 156, "/explicit.jpg", LittleFS);
    }

    //Progress bar
    drawProgressSprite(progress_ms, duration_ms, avgR, avgG, avgB);
    //All drawn at this point
  }
}