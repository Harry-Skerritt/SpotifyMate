//|------------------------------------------------------------------------------------------------------------------------|
//| Made using the following examples from witenessmenow's Spotify-API-Arduino library:                                    |
//|                     https://github.com/witnessmenow/spotify-api-arduino/tree/main                                      |
//|                                                                                                                        |
//| https://github.com/witnessmenow/spotify-api-arduino/blob/main/examples/displayAlbumArt/tftAlbumArt/tftDisplayImage.ino |
//|       https://github.com/witnessmenow/spotify-api-arduino/blob/main/examples/getRefreshToken/getRefreshToken.ino       |
//|                                                                                                                        |
//|------------------------------------------------------------------------------------------------------------------------|


#include <Spotify.h>
#include <Wifi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include <FS.h>
#include <TFT_eSPI.h>
#include <TJpg_Decoder.h>
#include <Graphics.h>

#include <SpotifyArduino.h> //https://github.com/witnessmenow/spotify-api-arduino/tree/main
//#include <SpotifyArduinoCert.h>
#include <ArduinoJson.h>


//Keys
const char* SP_CLIENT;
const char* SP_SECRET;
const char* SP_REFRESH = "xxxxxxxxxxxxxxx";

//SPotify Related
#define SPOTIFY_MARKET "GB"
#define ALBUM_ART "/album.jpg"

String lastAlbumArtUrl;
SpotifyImage smallestImage;
char *songName;
char *songArtist;

//Webserver
WiFiClientSecure client;
unsigned long delayBetweenRequests = 30000; // Time between requests (30 seconds)
unsigned long requestDueTime;               // time when request due



void startSpiffs(){
    Serial.begin(115200);
    if(!SPIFFS.begin()){
        Serial.println("An Error occured whilst mounting SPIFFS");
        return;
    } else {
        Serial.println("Spiffs Init - Complete");
    }
    
}

void readInKeys() {
    startSpiffs();
    Serial.begin(115200);
    File file = SPIFFS.open("/spotify_keys.txt");
    if(!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    int i = 0;
    char buffer[64];
    String client, secret;
    Serial.println("File Content:");
    while(file.available()){
        int l = file.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[l] = 0;
        if(i == 0){
            client = buffer;
        }
        if(i == 1){
            secret = buffer;
        }
        i++;

        if(i == 2){
            break;
        }
    }
    file.close();

    Serial.println(client);
    Serial.println(secret);
    SP_CLIENT = client.c_str();
    SP_SECRET = secret.c_str();
}


SpotifyArduino initSpotify(){
    readInKeys();
    SpotifyArduino spotify(client, SP_CLIENT, SP_SECRET, SP_REFRESH);

    //JPeg Decoder
    TJpgDec.setJpgScale(4);
    TJpgDec.setCallback(displayOutput);
    TJpgDec.setSwapBytes(true);

    //Client
    //client.setCACert(spotify_server_cert);
    client.setInsecure();

    return spotify;
}

SpotifyArduino spotify = initSpotify(); //Should be inited



int displayImage(char *albumArtUrl)
{

    uint8_t *imageFile; // pointer that the library will store the image at (uses malloc)
    int imageSize;      // library will update the size of the image
    bool gotImage = spotify.getImage(albumArtUrl, &imageFile, &imageSize);

    if (gotImage)
    {
        Serial.print("Got Image");
        delay(1);
        int jpegStatus = TJpgDec.drawJpg(28, 40, imageFile, imageSize);
        free(imageFile); // Make sure to free the memory!
        return jpegStatus;
    }
    else
    {
        return -2;
    }
}

void printCurrentlyPlayingToSerial(CurrentlyPlaying currentlyPlaying)
{
    Serial.begin(115200);
    // Use the details in this method or if you want to store them
    // make sure you copy them (using something like strncpy)
    // const char* artist =

    // Clear the Text every time a new song is created
    //tft.fillRect(0, 120, 128, 130, TFT_BLACK);
    Serial.println("--------- Currently Playing ---------");

    Serial.print("Is Playing: ");
    if (currentlyPlaying.isPlaying)
    {
        Serial.println("Yes");
    }
    else
    {
        Serial.println("No");
    }

    Serial.print("Track: ");
    Serial.println(currentlyPlaying.trackName);
    // Save the song name to a variable
    songName = const_cast<char *>(currentlyPlaying.trackName);
    //drawMessage(0, 120, songName);
    Serial.print("Track URI: ");
    Serial.println(currentlyPlaying.trackUri);
    Serial.println();

    Serial.println("Artists: ");
    for (int i = 0; i < currentlyPlaying.numArtists; i++)
    {
        Serial.print("Name: ");
        // Save the song artist name to a variable
        Serial.println(currentlyPlaying.artists[i].artistName);
        songArtist = const_cast<char *>(currentlyPlaying.artists[0].artistName);
        //drawMessage(0, 130, songArtist);
        Serial.print("Artist URI: ");
        Serial.println(currentlyPlaying.artists[i].artistUri);
        Serial.println();
    }

    Serial.print("Album: ");
    Serial.println(currentlyPlaying.albumName);
    Serial.print("Album URI: ");
    Serial.println(currentlyPlaying.albumUri);
    Serial.println();

    long progress = currentlyPlaying.progressMs; // duration passed in the song
    long duration = currentlyPlaying.durationMs; // Length of Song
    Serial.print("Elapsed time of song (ms): ");
    Serial.print(progress);
    Serial.print(" of ");
    Serial.println(duration);
    Serial.println();

    float percentage = ((float)progress / (float)duration) * 100;
    int clampedPercentage = (int)percentage;
    Serial.print("<");
    for (int j = 0; j < 50; j++)
    {
        if (clampedPercentage >= (j * 2))
        {
            Serial.print("=");
        }
        else
        {
            Serial.print("-");
        }
    }
    Serial.println(">");
    Serial.println();

    // will be in order of widest to narrowest
    // currentlyPlaying.numImages is the number of images that
    // are stored

    for (int i = 0; i < currentlyPlaying.numImages; i++)
    {
        // Save the second album image into the smallestImage Variable above.
        smallestImage = currentlyPlaying.albumImages[1];
        Serial.println("------------------------");
        Serial.print("Album Image: ");
        Serial.println(currentlyPlaying.albumImages[i].url);
        Serial.print("Dimensions: ");
        Serial.print(currentlyPlaying.albumImages[i].width);
        Serial.print(" x ");
        Serial.print(currentlyPlaying.albumImages[i].height);
        Serial.println();
    }
    Serial.println("------------------------");
}

void sp_getCurrentSong() {
    int status = spotify.getCurrentlyPlaying(printCurrentlyPlayingToSerial, SPOTIFY_MARKET);
    if (status == 200)
    {
        Serial.println("Successfully got currently playing");
        String newAlbum = String(smallestImage.url);
        if (newAlbum != lastAlbumArtUrl)
        {
            Serial.println("Updating Art");
            char *my_url = const_cast<char *>(smallestImage.url);
            int displayImageResult = displayImage(my_url);

            if (displayImageResult == 0)
            {
                lastAlbumArtUrl = newAlbum;
            }
            else
            {
                Serial.print("failed to display image: ");
                Serial.println(displayImageResult);
            }
        }
        else if (status == 204)
        {
            Serial.println("Doesn't seem to be anything playing");
        }
        else
        {
            Serial.print("Error: ");
            Serial.println(status);
        }
    }
}

char getSPClient(){
    return *SP_CLIENT;
}

char getSPSecret(){
    return *SP_SECRET;
}

//====REFRESH ACCESS TOKEN====
WebServer server(80); //RAT
char clientId = getSPClient();
char clientSecret = getSPSecret();
char scope[] = "user-read-playback-state%20user-modify-playback-state";
char callbackURI[] = "http%3A%2F%2Fspotify-mate.local%2Fcallback%2F";

const char *webpageTemplate = R"(
<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
  </head>
  <body>

     <a href="https://accounts.spotify.com/authorize?client_id=%s&response_type=code&redirect_uri=%s&scope=%s">
         <div class="connect-button">
            <img class="spotify_img", src="https://storage.googleapis.com/pr-newsroom-wp/1/2023/05/Spotify_Primary_Logo_RGB_Black.png", alt="Spotify Logo", width="40", height="40"/>
            <h1 class="connect_text">Connect Spotify</h1>
        </div>
    </a>
    
  </body>
  <style>
      body{
          background-color: #fbfbfb;
      }
      
      .connect-button {
         margin-top: 20px;
        margin-left: 15px;
        width: 400px;
        height: 70px;
        background-color: #1ed760;
        border-radius: 50px;
        display: flex;
        margin: 0;
        margin-left: auto;
        margin-right: auto;
        margin-top: 20px;
          
      }
      
      img {
         width: 60px;
         height: 60px;
         margin-left: 6px;
        margin-top: 5px;
      }
      
      a{
         background-color: #121212;
         border: none;
         text-decoration: none;
      }
      
      h1 {
        font-family: arial;
        color: #121212;
        font-size: 38px;
        margin-left: 15px;
        margin-top: 0px;
        margin-top: auto;
        margin-bottom: auto;
        }
  </style>
 </html>
)";

void handleRoot()
{
  char webpage[800];
  sprintf(webpage, webpageTemplate, clientId, callbackURI, scope);
  server.send(200, "text/html", webpage);
}

void handleCallback()
{
  String code = "";
  const char *refreshToken = NULL;
  for (uint8_t i = 0; i < server.args(); i++)
  {
    if (server.argName(i) == "code")
    {
      code = server.arg(i);
      refreshToken = spotify.requestAccessTokens(code.c_str(), callbackURI);
    }
  }

  if (refreshToken != NULL)
  {
    server.send(200, "text/plain", refreshToken);
  }
  else
  {
    server.send(404, "text/plain", "Failed to load token, check serial monitor");
  }
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++)
  {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  Serial.print(message);
  server.send(404, "text/plain", message);
}

void refreshAccessToken() {
    Serial.begin(115200);

    //DEBUG
    Serial.println("");
    Serial.print("Connected");
    Serial.print("IP address: ");
    IPAddress ipAddress = WiFi.localIP();
    Serial.println(ipAddress);

    client.setInsecure();

    server.on("/", handleRoot);
    server.on("/callback/", handleCallback);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("HTTP server started");
    
}

void ratLoop(){
    server.handleClient();
}