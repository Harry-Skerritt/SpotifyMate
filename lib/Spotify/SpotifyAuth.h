#ifndef SPOTIFY_AUTH_H
#define SPOTIFY_AUTH_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <base64.h>
#include <ArduinoJson.h>

extern String accessToken;
extern String refreshToken;
extern unsigned long tokenExpiryTime;
extern WebServer server;

// Replace with your actual Spotify app credentials
extern const char* client_Id;
extern const char* clientSecret;
extern const char* redirectUri;
extern const char* scopes;

// Function declarations
void putSpotifyAPIKeys(String clientID, String clientSecret);
void loadSpotifyAPIKeys();
void loadSpotifyToken();

void authSpotify();
bool addCurrentSongToLiked();
String getCurrentlyPlayingTrack();
String getCurrentlyPlayingArtist();
String getAlbumUrl();
int getProgress();
int getDuration();
bool getExplicitSong();

String getNextTrackInQueue();
String getAccessTokenFromCode(const String& code);
String getValidAccessToken();
String refreshAccessToken();
void redirectToSpotifyAuthorization();
void handleSpotifyRedirect();

String getAuthURL();

#endif
