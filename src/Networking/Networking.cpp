#include <Networking.h>
#include <CaptivePortal.h>
#include <Wifi.h>

void startCaptiveProcess(const char *ssid) {


    setupCaptivePortal(ssid);

    int i = 0;
    while(i < 10000){
        i++;
        captivePortalLoop();
    }
}


IPAddress device_ip;

void connectToWifi(String ssid, String pass){
    const char* ssid_loc = ssid.c_str();
    const char* pass_loc = pass.c_str();
    const char* hostname = "SpotifyMate";

    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_loc, pass_loc);
    Serial.println("Connecting to WiFI");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }

    Serial.println("Connected!");
    device_ip = WiFi.localIP();
    Serial.println(WiFi.localIP());
}

IPAddress getIp(){
    return device_ip;
}