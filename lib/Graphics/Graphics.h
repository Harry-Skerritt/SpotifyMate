#ifndef GRAPHICS_H
#define GRAPHICS_H

void initialiseGraphics();

void startupGraphics(String msg);

void captiveGraphics(const char *ssid);

void spotifyConnectScreen(String hostname);

bool displayOutput(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);

#endif