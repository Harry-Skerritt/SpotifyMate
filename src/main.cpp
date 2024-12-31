//Check if network creds
//If so attempt connection
//If not lauch captive portal
//Get Creds
//Loop

//On start display splash screen and loading whilst checking network creds and/or attempting connection
//If captive displayed then show how to use on the screen
//Display network connecting
//Go to home (temp for now)

//On Very Very First Launch
//Generate a settings file
//Generate a random 5 digit number - the device identifier
//Put that identifier in the settings in the form of a name
//This is also where the wifi credentials will be stored

#include <Arduino.h>
#include <Graphics.h>
#include <Processes.h>
#include <Networking.h>



void setup() {
    removeNetCreds(); //DEBUG TO RESET

    const char* ssid = "SpotifyMate-AP4562";
    IPAddress local_ip;
    Serial.begin(115200);

    initialiseGraphics(); //Initialise the Graphical elements

    startupGraphics("Starting Up...");
    delay(500);

    startupGraphics("Getting Credentials...");
    getSpotifyCreds();
    delay(500);

    startupGraphics("Getting Preferences...");
    getPreferences();


    String ssid_got = get_ssid();
    String pass_got = get_pass();

    Serial.println(ssid_got + " | " + pass_got);
    delay(1000);

    if(ssid_got == "null" || pass_got == "null"){
        captiveGraphics(ssid);
        startCaptiveProcess(ssid);
        bool wifiConnected = getConnected();
        while(!wifiConnected){
            wifiConnected = getConnected();
            startupGraphics("Connecting...");
        }
        if(wifiConnected){
            startupGraphics("Connecting.");
            delay(50);
            startupGraphics("Connecting..");
            delay(50);
            startupGraphics("Connecting...");
            delay(50);
            
            startupGraphics("Local IP: " + local_ip.toString());
            authSpotify();
        }
        //Add handling for incorrect creds
    } else {
        Serial.println("Not Null");
        startupGraphics("Connecting to Network...");
        connectToWifi(ssid_got, pass_got);
        local_ip = getIp();
        startupGraphics("Local IP: " + local_ip.toString());
    }




}

void loop() {

}