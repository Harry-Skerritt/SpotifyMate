#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <Graphics.h>
#include <ESPmDNS.h>
#include <SpotifyAuth.h>
#include <esp_heap_caps.h>
#include <Wifi.h>
#include <Preferences.h>


WiFiManager wm;
bool setupCompleted = false; //Only true if on wifi and connected to spotify

//Like Song Functionality
#define LIKE_SONG_PIN 12
int like_song_state ;

void setup() {
    Serial.begin(115200);
    const char* ssid = "SpotifyMate-AP";
    const String hostname = "spotify-mate";
    Preferences prefs;

    pinMode(LIKE_SONG_PIN, INPUT_PULLUP);



    initialiseGraphics(); //Initialise the Graphical elements
    startupGraphics("Starting Up...");
    delay(1000);

    //String clientId = "xxx"; 
    //String clientSecret = "xxx";
    //putSpotifyAPIKeys(clientId, clientSecret);

    
    String l_refreshToken = "";
    String l_accessToken = "";
    long l_tokenExpiryTime = 0;

    startupGraphics("Getting Credentials...");
    //loadSpotifyAPIKeys(); //Check this is the right position
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

        //mDNS Setup
        if(!MDNS.begin(hostname)) {
            Serial.println("Error setting up MDNS responder");
            while(1){
                delay(1000);
            }
        }
        Serial.println("mDNS responder started");

        //Now on WiFi

        //Spotify Get Access Token
        prefs.begin("spotify-data", false);
        l_refreshToken = prefs.getString("refreshToken", "");
        l_tokenExpiryTime = prefs.getLong("expiryTime", 0);
        l_accessToken = prefs.getString("accessToken", "");
        prefs.end();

        Serial.println("RT: " + l_refreshToken);
        Serial.println("AT: " + l_accessToken);
        Serial.println("ET: " + String(tokenExpiryTime));

        //authSpotify();

        //Fix not needing to auth everytime
        if(l_refreshToken == "" || l_accessToken == ""){
            //There is no access token (assume not authorised)
            startupGraphics("Authing Spotify!");
            authSpotify();
            setupCompleted = true;
        } else {
            //There is a token, best to get a new one.
            loadSpotifyToken(); //Loads from Preferences
            delay(500); //Time to breathe
            getValidAccessToken(); //Check the token is valid -> Refresh always for now
            startupGraphics("Spotify Connected!");
            setupCompleted = true;
        }
    }
    else {
        //Not Connected to Wifi
        Serial.println("Configportal running");
        captiveGraphics(ssid);

        if(wm.getWiFiIsSaved() == true){
            //Once Done, Restart
            Serial.println("Wifi Saved is now true");
        }
    }
}

unsigned long lastScreenUpdate = 0; //Tracks when the screen was last updated
const unsigned long updateInterval = 2000; //Time in ms for the screen update delay

void loop() {
    wm.process();

    //Checked Constantly
    //Like Songs
    like_song_state = digitalRead(LIKE_SONG_PIN);
    if(like_song_state == LOW){
        Serial.println("Like Song Button Clicked");
        if(addCurrentSongToLiked()) {
            //Song added Successfully do something graphically
            Serial.println("Song Added Successfully");
        } else {
            //Song wasn't added successfully do something graphically
            Serial.println("Failed to add Song");
        }

        like_song_state = !like_song_state;  //Move to the function which adds the liked song
    }

    //Performed every 2 seconsd
    //The App has correctly initialised
    if(setupCompleted && (millis() - lastScreenUpdate >= updateInterval)){
        lastScreenUpdate = millis(); //Update the timestamp

        //Handle The Screen
        String track = getCurrentlyPlayingTrack();
        String artist = getCurrentlyPlayingArtist();
        String albumUrl = getAlbumUrl();
        int progress = getProgress();
        int duration = getDuration();
        bool explicit_song = getExplicitSong();


        drawCurrentPlaying(track, artist, albumUrl, progress, duration, explicit_song);     
        //delay(2000); //Refresh every 2 seconds (Any quicker brings -11 codes?)
    }
}