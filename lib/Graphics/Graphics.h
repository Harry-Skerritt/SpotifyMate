#ifndef GRAPHICS_H
#define GRAPHICS_H


void initialiseGraphics();

void startupGraphics(String msg);

void captiveGraphics(const char *ssid);

void spotifyConnectScreen();

bool displayOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);

void displayImageFromURL(const char* url);

void drawCurrentPlaying(String title, String artist, String url, int progress_ms, int duration_ms, bool explicit_song);

void isTouch();

#endif