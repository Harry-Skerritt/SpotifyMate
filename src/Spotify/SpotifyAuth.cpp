#include "SpotifyAuth.h"
#include <Preferences.h>
#include <WiFi.h>
#include <Graphics.h>
#include <WiFiClientSecure.h>

String accessToken = "";
String refreshToken = "";
unsigned long tokenExpiryTime = 0; // Expiry time in seconds since epoch


const char* clientId = "517041868f9544a0bd757e847ffa3256"; //Remove before any commits -> Use the FILE
const char* clientSecret = "8e97936b91944a0cb385534884ef83b4"; //Remove before any commits -> Use the FILE
const char* redirectUri = "http://spotify-mate.local/callback";
const char* scopes = "user-read-currently-playing user-read-playback-state user-modify-playback-state user-library-modify";

WebServer server(80);

String currentTrackId = "";

void convertFromJson(JsonVariantConst src, tm& dst) {
  strptime(src.as<const char*>(), "%FT%TZ", &dst);
}

//Not used currently
void putSpotifyAPIKeys(String clientID, String clientSecret){
  //This only needs to be called once (or if secret changes)
  Preferences prefs;
  prefs.begin("spotify-keys", false);
  prefs.putString("clientId", clientID);
  //prefs.putString("clientSecret", clientSecret);
  prefs.end();

  Serial.println("API Keys Stored Successfully!");
}

void loadSpotifyAPIKeys(){
  Serial.println("Getting API Keys");
  Serial.println(clientId);

  Preferences prefs;
  prefs.begin("spotify-keys", false);
  clientId = prefs.getString("clientId", "clientID").c_str();
  //clientSecret = prefs.getString("clientSecret", "clientSecret").c_str();
  prefs.end();

  Serial.println(clientId);
  Serial.println(clientSecret);
}
 

void loadSpotifyToken() {
  Serial.println("Getting Access Token");
  Preferences prefs;
  prefs.begin("spotify-data", false);
  refreshToken = prefs.getString("refreshToken", "");
  tokenExpiryTime = prefs.getLong("expiryTime", 0);
  accessToken = prefs.getString("accessToken", "");
  prefs.end();
}

void saveSpotifyToken(){
  Serial.println("Saving Access Token");
  Preferences prefs;
  prefs.begin("spotify-data", false);
  prefs.putString("refreshToken", refreshToken);
  prefs.putLong("expiryTime", tokenExpiryTime);
  prefs.putString("accessToken", accessToken);
  prefs.end();
}

void authSpotify() {
  Serial.println("Starting Spotify authentication...");
  Serial.println("DEBUG: Redirect URI: " + String(redirectUri)); 
  
  // Start the web server
  server.on("/callback", HTTP_GET, handleSpotifyRedirect);  // Handle callback URL
  server.begin();
  
  // Redirect user to authenticate and get authorization code
  redirectToSpotifyAuthorization();
  
  // Keep the server running until the user is redirected back with the authorization code
  while (accessToken == "") {
    server.handleClient();
    delay(100);
  }
  //Should be an access and stuff now
  saveSpotifyToken();
}

void redirectToSpotifyAuthorization() {
  Serial.println(clientId);
  String authUrl = "https://accounts.spotify.com/authorize?response_type=code&client_id=" + String(clientId) +
                   "&scope=" + String(scopes) + "&redirect_uri=" + String(redirectUri);
  Serial.println("Visit the following URL to authenticate:");
  Serial.println(authUrl);
}

void handleSpotifyRedirect() {
  if (server.hasArg("code")) {
    String code = server.arg("code");
    Serial.println("Authorization code: " + code);
    
    // Exchange the authorization code for access and refresh tokens
    accessToken = getAccessTokenFromCode(code);
    
    if (accessToken != "") {
      Serial.println("Access token: " + accessToken);
    } else {
      Serial.println("Failed to get access token.");
    }
    server.send(200, "text/plain", "Spotify Authentication successful! You can close this page now.");
  } else {
    server.send(400, "text/plain", "Error: No authorization code found.");
  }
}

String getAccessTokenFromCode(const String& code) {
  HTTPClient http;
  http.begin("https://accounts.spotify.com/api/token");

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String body = "grant_type=authorization_code&code=" + code + "&redirect_uri=" + String(redirectUri);
  
  String authHeader = base64::encode(clientId + String(":") + clientSecret);
  http.addHeader("Authorization", "Basic " + authHeader);

  int httpCode = http.POST(body);
  String payload = http.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    accessToken = doc["access_token"].as<String>();
    refreshToken = doc["refresh_token"].as<String>();
    String expiresInStr = doc["expires_in"].as<String>(); 
    unsigned long expiresIn = strtol(expiresInStr.c_str(), NULL, 10); // Convert string to unsigned long
    tokenExpiryTime = millis() / 1000 + expiresIn;
    Serial.println("Refresh Token: " + String(refreshToken));
    return accessToken;
  } else {
    Serial.println("Error getting access token: " + String(httpCode));
    return "";
  }
}

String getValidAccessToken() {  
  //Sometimes get hungup here
  Serial.println("Checking if Access Token is Valid");
  if (millis() / 1000 >= tokenExpiryTime) {
    Serial.println("Access token expired. Refreshing...");
    return refreshAccessToken();
  } else {
    return accessToken; // Token is still valid
  }
}

String refreshAccessToken() {
  HTTPClient http;
  http.begin("https://accounts.spotify.com/api/token");

  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String body = "grant_type=refresh_token&refresh_token=" + String(refreshToken) + "&client_id=" + String(clientId);
  String authHeader = base64::encode(clientId + String(":") + clientSecret);
  http.addHeader("Authorization", "Basic " + authHeader);

  int httpCode = http.POST(body);
  String payload = http.getString();

  if (httpCode == 200) {
    Serial.println("Successful Refresh Command");
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    accessToken = doc["access_token"].as<String>();
    String expiresInStr = doc["expires_in"].as<String>(); 
    unsigned long expiresIn = strtol(expiresInStr.c_str(), NULL, 10); // Convert string to unsigned long
    tokenExpiryTime = millis() / 1000 + expiresIn;
    saveSpotifyToken();
    return accessToken;
  } else {
    Serial.println("Error refreshing access token: " + String(httpCode));
    return "";
  }
}


bool addCurrentSongToLiked() {
    if (currentTrackId.isEmpty()) {
        Serial.println("Track ID is empty. Cannot add to liked songs.");
        return false;
    }

    String token = getValidAccessToken(); // Ensure the token is valid before making API calls
  
    if (token == "") {
      Serial.println("No valid access token.");
      return "";
    }


    // Spotify API endpoint for adding songs to the library
    String endpoint = "https://api.spotify.com/v1/me/tracks";

    // Create an HTTPS client
    WiFiClientSecure client;
    client.setInsecure(); // Use this if you don't want to verify SSL. Replace with certificate validation in production.

    HTTPClient https;
    if (!https.begin(client, endpoint)) {
        Serial.println("Failed to connect to Spotify API.");
        return false;
    }

    // Set headers
    https.addHeader("Authorization", "Bearer " + token);
    https.addHeader("Content-Type", "application/json");

    // Prepare JSON payload with the track ID
    String payload = "{\"ids\":[\"" + currentTrackId + "\"]}";

    // Send POST request
    int httpResponseCode = https.POST(payload);

    if (httpResponseCode == 200 || httpResponseCode == 204) {
        Serial.println("Song successfully added to liked songs.");
        https.end();
        return true;
    } else {
        Serial.printf("Failed to add song to liked songs. HTTP code: %d\n", httpResponseCode);
        Serial.println("Response: " + https.getString());
    }

    https.end();
    return false;
}

String track_title = "";
String track_artist = "";
String album_url = ""; //Needs a default value
int progress = 0 ;
int duration = 0 ;
bool explicit_song = false;
bool playing; //Implement this

String getCurrentlyPlayingTrack() {
  if(WiFi.status() != WL_CONNECTED){
    //If wifi is not connected
    startupGraphics("Device is Offline! - Restart");
    return "";
  } else {
    //Device is online
    Serial.println("Getting Currently Playing Track");
    String token = getValidAccessToken(); // Ensure the token is valid before making API calls
  
    if (token == "") {
      Serial.println("No valid access token.");
      return "";
    }

    Serial.println("###Just before the http is send");

    HTTPClient http;
    WiFiClientSecure wifiClient;
    wifiClient.setInsecure();
    wifiClient.setTimeout(10000); //10 Second Timeout
    String url = "https://api.spotify.com/v1/me/player/currently-playing";  // Get currently playing track


    if(http.begin(wifiClient, url)) {
      http.addHeader("Authorization", "Bearer " + token);


      int httpCode = http.GET();
      String payload = http.getString();
      yield();

      Serial.print("HCB: ");
      Serial.println(httpCode);

      if (httpCode == 401){
        //Bad Token
        startupGraphics("Refreshing Credentials...");
        refreshAccessToken(); //Renew the token
        delay(200); //Allow process time
        getCurrentlyPlayingTrack(); //Try again
        yield();
        return "";
      } else if (httpCode == 200) {
        //Music is Playing
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, payload);

        //Serial.println("DEBUG 5");
        const char* trackName = doc["item"]["name"];
        //Serial.println(trackName);

        const char* trackID = doc["item"]["id"];

        const char* artistName = doc["item"]["artists"][0]["name"];
        //Serial.println(artistName);

        const char* albumURL = doc["item"]["album"]["images"][0]["url"];
        //Serial.println(albumURL);

        const int i_progress = doc["progress_ms"];
        //Serial.println("P: " + String(i_progress));

        const int i_duration = doc["item"]["duration_ms"];
        //Serial.println("D: " + String(i_duration));

        explicit_song = doc["item"]["explicit"];
        Serial.println("Explicit:" + String(explicit_song));

        //Serial.println("DEBUG 6");
        //delay(20);

        track_title = String(trackName);
        currentTrackId = String(trackID);
        track_artist = String(artistName);
        album_url = String(albumURL);
        progress = i_progress;
        duration = i_duration;

        //Serial.println("DEBUG 7");

        //doc.clear();
        Serial.println("Currently playing track: " + String(trackName));
        Serial.println("Artist: " + String(artistName));
        return String(trackName);
      } else {
        //Music is not playing?
        if(httpCode == -11){
          Serial.println("-11 Recieved.");
          //getCurrentlyPlayingTrack();
          //yield();
        } else if (httpCode == -1){
          Serial.println("-1 Recieved.");
          //getCurrentlyPlayingTrack();
          //yield();
        } else {
          track_title = "Nothing is Playing";
          track_artist = "-";
          album_url = "https://raw.githubusercontent.com/Harry-Skerritt/test/refs/heads/main/not_playing_album.jpg";
          progress = 0;
          duration = 100;
    
          Serial.println(httpCode);

          Serial.println("Error fetching currently playing track: " + String(httpCode));
          return String(track_title); //""
        }
      }
    } else {
      Serial.println("HTTP connection failed");
    }
  }
}

String getCurrentlyPlayingArtist(){
  return track_artist;
}

String getAlbumUrl(){
  return album_url;
}

int getProgress(){
  return progress;
}

int getDuration(){
  return duration;
}

bool getExplicitSong(){
  return explicit_song;
}

String getAuthURL(){
  return String("https://accounts.spotify.com/authorize?response_type=code&client_id=" + String(clientId) +
                   "&scope=" + String(scopes) + "&redirect_uri=" + String(redirectUri));
}
