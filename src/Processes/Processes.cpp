#include <Preferences.h>
#include <Processes.h>
#include "SpotifyEsp32.h"


String net_ssid;
String net_pass;

bool getPreferences() {
    Preferences prefs;

    prefs.begin("spotify-mate", false);

    net_ssid = prefs.getString("ssid", "null");
    if(net_ssid == "null"){
        net_ssid = "null";
    } else {
        net_ssid = net_ssid;
    }

    
    net_pass = prefs.getString("pass", "null");
    if(net_pass == "null"){
        net_pass = "null";
    } else {
        net_pass = net_pass;
    }

    if(net_ssid != "" && net_pass != ""){
        return true;
    } else {
        return false;
    }

    prefs.end();

}

//DEBUG
void removeNetCreds(){
    Preferences pref;
    pref.begin("spotify-mate", false);
    pref.remove("ssid");
    pref.remove("pass");
    pref.end();
}

String get_ssid() {
    return net_ssid;
}

String get_pass() {
    return net_pass;
}

const char* hc_CLIENT_ID = "517041868f9544a0bd757e847ffa3256";
const char* hc_CLIENT_SECRET = "e37cec185bb249b4ade4d86125840831";

void setSpotifyCreds(){
    Preferences pref;
    pref.begin("spotify-mate", false);
    pref.putString("sp-client", hc_CLIENT_ID);
    pref.putString("sp-secret", hc_CLIENT_SECRET);

    pref.end();

}

const char* SP_CLIENT;
const char* SP_SECRET;

void getSpotifyCreds(){
    Preferences pref;
    pref.begin("spotify-mate", false);

    SP_CLIENT = pref.getString("sp-client", "null").c_str();
    SP_SECRET = pref.getString("sp-secret", "null").c_str();

    pref.end();
}

void authSpotify() {
    Serial.println(SP_CLIENT);
    Serial.println(SP_SECRET);
    Spotify sp(SP_CLIENT, SP_SECRET);

    sp.begin();
    while(!sp.is_auth()){
        sp.handle_client();
    }
    Serial.println("Spotify Authenticated");
}
