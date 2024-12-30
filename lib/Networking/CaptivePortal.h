#ifndef CAPTIVEPORTAL_H
#define CAPTIVEPORTAL_H

void setUpDNSServer();
void startSoftAccessPoint();
void setUpWebserver();
void setupCaptivePortal(const char *ssid);
void captivePortalLoop();


#endif