#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <Graphics.h>
#include <array>
#include <ESPmDNS.h>
#include <Spotify.h>

//Spotify Related
#include <Wifi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>

#include <SpotifyArduino.h> //https://github.com/witnessmenow/spotify-api-arduino/tree/main
#include <SpotifyArduinoCert.h> //https://github.com/witnessmenow/spotify-api-arduino/tree/main
#include <ArduinoJson.h>

WiFiManager wm;

void setup() {
    Serial.begin(115200);

    //startSpiffs();

    //Debug
    pinMode(23, INPUT_PULLUP);

    if(digitalRead(23) == LOW) {
       // wm.resetSettings();
        sp_getCurrentSong();
    }
    //Not Debug

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

        //Spotify Get Access Token
        refreshAccessToken();

        //sp_getCurrentSong();

        
        if(true){
            //No refresh token
            spotifyConnectScreen(hostname + ".local");
            //authSpotify();
        } else {
            //Spotify Authed
            startupGraphics("Spotify Connected!");
        }

        
        
    }
    else {
        Serial.println("Configportal running");
        captiveGraphics(ssid);
        //Not Connected to Wifi
    }
}

void loop() {
    wm.process();
    ratLoop();
    //Serial.println("SS: "+wm.getWLStatusString());
    // put your main code here, to run repeatedly:
}