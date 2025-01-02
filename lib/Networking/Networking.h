#include <WiFi.h>
#ifndef NETWORKING_H
#define NETWORKING_H


void connectToWifi(String ssid, String pass);

IPAddress getIp();

bool getConnected();


#endif