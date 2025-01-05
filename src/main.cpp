#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <Graphics.h>
#include <ESPmDNS.h>
#include <SpotifyAuth.h>
#include <esp_heap_caps.h>
#include <Wifi.h>
#include <Preferences.h>


//STILL TO DO --
//Make it restart once its finished provisioning
//Make it not need to be reauthroised every time its restarted
//Add touch screen 
//Add play/pause
//Add add to liked songs
//Make the main screen sprites, so only the bar can be updated and not the whole screen
//Fix the explicit not loading
//Add the logo to the loading pages
//QR Code for spotify auth
//Other than that it 'works'--

//Not Essential but would be VERY nice to have --
//Make it get the next song's image in the queue to allow smooth transition
//Some handling for is the next song is the one expected
//Background the average colour of the album art (Optional)

//IF POSSIBLE --
//Make getting the album image quicker (~11s currently)
//Progress bar about 3s behind (if can update more 'real time' do)



WiFiManager wm;
bool setupCompleted = false; //Only true if on wifi and connected to spotify

void setup() {
    Serial.begin(115200);
    const char* ssid = "SpotifyMate-AP";
    const String hostname = "spotify-mate";
    Preferences prefs;


    initialiseGraphics(); //Initialise the Graphical elements
    startupGraphics("Starting Up...");
    delay(1000);

    //String clientId = "xxx"; 
    //String clientSecret = "xxx";
    //putSpotifyAPIKeys(clientId, clientSecret);

    
    String accessToken = "";
    long tokenExpiryTime = 0;

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
        //Check if connected to spotify
        //  If So then get the track
        //  If not then auth spotify

        //Spotify Get Access Token
        prefs.begin("spotify-data", false);
        accessToken = prefs.getString("accessToken", "");
        tokenExpiryTime = prefs.getLong("expiryTime", 0);
        prefs.end();

        Serial.println("AT: " + accessToken);
        Serial.println("ET: " + String(tokenExpiryTime));

        bool tokenPresent = false;

        //Fix not needing to auth everytime
        if(accessToken == ""){
            tokenPresent = false;
            //There is no access token (assume not authorised)
            startupGraphics("Authing Spotify!");
            authSpotify();
            //Need to do something once connected; Loop? use the bool
        } else {
            //There is a token, best to get a new one.
            loadSpotifyToken(); //Loads from Preferences
            delay(500); //Time to breathe
            refreshAccessToken(); //Check the token is valid -> Refresh always for now
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
        }
        
        
    }
}

void loop() {
    wm.process();


    if(setupCompleted){
        String track = getCurrentlyPlayingTrack();
        String artist = getCurrentlyPlayingArtist();
        String albumUrl = getAlbumUrl();
        int progress = getProgress();
        int duration = getDuration();
        bool explicit_song = getExplicitSong();


        drawCurrentPlaying(track, artist, albumUrl, progress, duration, explicit_song);     
        delay(2000); //Refresh every 2 seconds
    }
}