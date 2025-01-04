#include "SpotifyAuth.h"
#include <WiFi.h>

String accessToken = "";
String refreshToken = "";
unsigned long tokenExpiryTime = 0; // Expiry time in seconds since epoch


const char* clientId = "xxxx"; //Put into SPIFFS
const char* clientSecret = "xxxx"; //Put into SPIFFS
const char* redirectUri = "http://xxx.xxx.xxx.xxxx/callback"; // Update to use mDNS (String("http://"+ WiFi.localIP().toString() + "/callback").c_str();)
const char* scopes = "user-read-currently-playing user-read-playback-state user-modify-playback-state";

WebServer server(80);

void convertFromJson(JsonVariantConst src, tm& dst) {
  strptime(src.as<const char*>(), "%FT%TZ", &dst);
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
}

void redirectToSpotifyAuthorization() {
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
    return accessToken;
  } else {
    Serial.println("Error getting access token: " + String(httpCode));
    return "";
  }
}

String getValidAccessToken() {
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
  
  String body = "grant_type=refresh_token&refresh_token=" + refreshToken;
  String authHeader = base64::encode(clientId + String(":") + clientSecret);
  http.addHeader("Authorization", "Basic " + authHeader);

  int httpCode = http.POST(body);
  String payload = http.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);
    accessToken = doc["access_token"].as<String>();
    String expiresInStr = doc["expires_in"].as<String>(); 
    unsigned long expiresIn = strtol(expiresInStr.c_str(), NULL, 10); // Convert string to unsigned long
    tokenExpiryTime = millis() / 1000 + expiresIn;
    return accessToken;
  } else {
    Serial.println("Error refreshing access token: " + String(httpCode));
    return "";
  }
}

String getCurrentlyPlayingTrack() {
  String token = getValidAccessToken(); // Ensure the token is valid before making API calls
  
  if (token == "") {
    Serial.println("No valid access token.");
    return "";
  }

  HTTPClient http;
  String url = "https://api.spotify.com/v1/me/player/currently-playing";  // Get currently playing track

  http.begin(url);
  http.addHeader("Authorization", "Bearer " + token);

  int httpCode = http.GET();
  String payload = http.getString();

  if (httpCode == 200) {
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, payload);

    const char* trackName = doc["item"]["name"];
    const char* artistName = doc["item"]["artists"][0]["name"];

    Serial.println("Currently playing track: " + String(trackName));
    Serial.println("Artist: " + String(artistName));
    return String(trackName);
  } else {
    Serial.println("Error fetching currently playing track: " + String(httpCode));
    return "";
  }
}

String getAuthURL(){
  return String("https://accounts.spotify.com/authorize?response_type=code&client_id=" + String(clientId) +
                   "&scope=" + String(scopes) + "&redirect_uri=" + String(redirectUri));
}
