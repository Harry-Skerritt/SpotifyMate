#include <Networking.h>
#include <Wifi.h>

IPAddress device_ip;
bool connected = false;

void connectToWifi(String ssid, String pass){
    const char* ssid_loc = ssid.c_str();
    const char* pass_loc = pass.c_str();
    const char* hostname = "SpotifyMate";

    WiFi.setHostname(hostname);
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid_loc, pass_loc);
    Serial.println("Connecting to WiFI");

    while(WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
        connected = false;
    }

    Serial.println("Connected!");
    connected = true;
    device_ip = WiFi.localIP();
    Serial.println(WiFi.localIP());
}

bool getConnected(){
    return connected;
}

IPAddress getIp(){
    return device_ip;
}

