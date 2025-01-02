#include <SpotifyArduino.h>

#ifndef SPOTIFY_H
#define SPOTIFY_H


void startSpiffs();

void readInKeys();

int displayImage(char *albumArtUrl);

void printCurrentlyPlayingToSerial(CurrentlyPlaying currentlyPlaying);

void sp_getCurrentSong();

char getClient();

char getSecret();


//====REFRESH ACCESS TOKEN====
void handleRoot();
void handleCallBack();
void handleNotFound();
void refreshAccessToken();
void ratLoop();


#endif