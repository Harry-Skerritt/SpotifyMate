#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <Arduino.h>
#include <Graphics.h>
#include <ESPmDNS.h>
#include <SpotifyAuth.h>
#include <esp_heap_caps.h>
#include <Wifi.h>
#include <Preferences.h>


//STILL TO DO --
//HIGH PRIORITY -> Find the cause of the -11 and -1 HTTPCodes and fix it as its causing crashes and issues
//HIGH PRIORITY -> The auto renewal of the token doesn't work, need to add 401 support
//Make it restart once its finished provisioning
//Make it not need to be reauthroised every time its restarted -> Testing (seems to be okay as of 05/01/24)
//Add touch screen 
//Add play/pause
//Add add to liked songs
//Make the main screen sprites, so only the bar can be updated and not the whole screen -> Done#
//Fix the explicit not loading -> Done (typo)#
//Add the logo to the loading pages -> Done#
//QR Code for spotify auth
//Change font size of artist if more than 20 chars -> Done#
//Every 20-40 mins it drops off - Fix
//Look at the check key is valid function -> Testing (I think) (seems to be okay as of 05/01/24)
//Time shows as 0:8 not 0:08 -> Done#
//Remove Unnecessary or DEBUG Serial Prints
//Proper not playing support
//5 min timeout for not playing
//Other than that it 'works'--

//Not Essential but would be VERY nice to have --
//Make it get the next song's image in the queue to allow smooth transition -> Weird Empty API Responses with 200 Code
//Some handling for if the next song is the one expected -> Weird Empty API Responses with 200 Code
//Background the average colour of the album art (Very Optional)

//IF POSSIBLE --
//Make getting the album image quicker (~11s currently)
//Progress bar about 3s behind (if can update more 'real time' do)
//Background the avg colour of album art

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
        delay(2000); //Refresh every 2 seconds (Any quicker brings -11 codes?)
    }
}