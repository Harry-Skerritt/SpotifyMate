#include <WiFi.h>
#ifndef NETWORKING_H
#define NETWORKING_H

void startCaptiveProcess(const char *ssid);

void connectToWifi(String ssid, String pass);

IPAddress getIp();


#endif