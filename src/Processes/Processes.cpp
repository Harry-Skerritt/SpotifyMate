#include <Preferences.h>
#include <Processes.h>


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


String get_ssid() {
    return net_ssid;
}

String get_pass() {
    return net_pass;
}