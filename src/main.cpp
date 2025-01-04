#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <Graphics.h>
#include <array>
#include <ESPmDNS.h>
#include <SpotifyAuth.h>

#include <esp_heap_caps.h>

//Spotify Related
#include <Wifi.h>
//#include <WebServer.h>
//#include <WiFiClient.h>
//#include <WiFiClientSecure.h>

//#include <SpotifyArduino.h> //https://github.com/witnessmenow/spotify-api-arduino/tree/main
//#include <SpotifyArduinoCert.h> //https://github.com/witnessmenow/spotify-api-arduino/tree/main
//#include <ArduinoJson.h>

WiFiManager wm;

void setup() {
    Serial.begin(115200);
    //Serial.begin(esp_get_free_heap_size());

    //startSpiffs();

    const char* ssid = "SpotifyMate-AP";
    const String hostname = "spotify-mate";
    initialiseGraphics(); //Initialise the Graphical elements
    startupGraphics("Starting Up...");
    delay(1000);

    
    

    startupGraphics("Getting Credentials...");
    //read_in_keys_spiffs();
    delay(500);

    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP    
    
    
    //Start Wifi Manager (for Provisioning)
    wm.setHostname("SpotifyMate");
    wm.setConnectRetries(4);

    wm.setConfigPortalBlocking(false);
    wm.setConfigPortalTimeout(240);
    wm.setConnectTimeout(20);

    startupGraphics("Connecting to WiFi...");
    delay(500);

    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
    if(wm.autoConnect(ssid)){
        //Connected to Wifi
        Serial.println("Connected to Wifi!!");
        startupGraphics("WiFi Connected!");


        //Need to handle refresh token.

        //mDNS Setup
        if(!MDNS.begin(hostname)) {
            Serial.println("Error setting up MDNS responder");
            while(1){
                delay(1000);
            }
        }
        Serial.println("mDNS responder started");

        drawCurrentPlaying("The Number of the Beast", "Iron Maiden", "https://i.scdn.co/image/ab67616d000048515c29a88ba5341ca428f0c322", "27000", "12500", "harry_skerritt");
        //delay(2000);

        //spotifyConnectScreen();
        //Now on WiFi
        //Check if connected to spotify
        //  If So then get the track
        //  If not then auth spotify

        //Spotify Get Access Token
        //authSpotify();
  
        //If this works, then make it get the artist aswell and then start on the display side, and possibly the image
        //Also add the redirect to a QR code that can be scanned

        // Now get the currently playing track
       // String track = getCurrentlyPlayingTrack();
       // if (track != "") {
       //     Serial.println("Successfully fetched track: " + track);
       // } else {
       //     Serial.println("Failed to fetch track.");
       // };

        
        
    }
    else {
        Serial.println("Configportal running");
        captiveGraphics(ssid);
        //Once Done, Restart
        //Not Connected to Wifi
    }
}

void loop() {
    wm.process();
    //Serial.println("SS: "+wm.getWLStatusString());
    // put your main code here, to run repeatedly:
}