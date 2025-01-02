#include <Preferences.h>
#include <Processes.h>
#include "SpotifyEsp32.h"
#include <SPIFFS.h>
#include <Arduino.h>
#include <array>

/*
const char* SP_CLIENT;
const char* SP_SECRET;

void read_in_keys_spiffs() {
    Serial.begin(115200);
    if(!SPIFFS.begin(true)){
        Serial.println("An Error occured whilst mounting SPIFFS");
        return;
    }

    File file = SPIFFS.open("/spotify_keys.txt");
    if(!file) {
        Serial.println("Failed to open file for reading");
        return;
    }

    int i = 0;
    char buffer[64];
    String client, secret;
    Serial.println("File Content:");
    while(file.available()){
        int l = file.readBytesUntil('\n', buffer, sizeof(buffer));
        buffer[l] = 0;
        if(i == 0){
            client = buffer;
        }
        if(i == 1){
            secret = buffer;
        }
        i++;

        if(i == 2){
            break;
        }
    }
    file.close();

    Serial.println(client);
    Serial.println(secret);
    SP_CLIENT = client.c_str();
    SP_SECRET = secret.c_str();
}

Spotify sp(SP_CLIENT, SP_SECRET); //Make this work with the keys

void authSpotify() {
    Serial.println(SP_CLIENT);
    Serial.println(SP_SECRET);

    sp.begin();
    while(!sp.is_auth()){
        sp.handle_client();
    }
    Serial.println("Spotify Authenticated");
}

std::array<String, 3> getCurrentSongDetails(){
    String artist;
    String title;
    String album_uri;

    artist = sp.current_artist_names();
    title = sp.current_track_name();
    album_uri = sp.get_current_album_image_url(0);

    std::array<String, 3> current;
    current[0] = artist;
    current[1] = title;
    current[2] = album_uri;
    
    return current;
}

bool hasRefreshToken() {
    const char *refresh = sp.get_user_tokens().refresh_token;
    String token = String(refresh);
    if(token == ""){
        return false;
    } else {
        return true;
    }
}

void downloadFile(uint8_t * imgA) {
    WiFiClientSecure wifiClient;
    uint16_t httpPort = 80; 
    const char * host = "http://i.scdn.co";

    Serial.print("connecting to "); 
    Serial.println(host); 
    if (!wifiClient.connect(host, httpPort)) { 
        Serial.println("connection failed"); 
        return; 
    }

    String action = "https://i.scdn.co/image/ab67616d0000b27396fa88fb1789be437d5cb4b6"; 
    Serial.print("Action "); 
    Serial.println(action); 
    wifiClient.print(String("GET ") + action + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP8266\r\n" + "Connection: close\r\n\r\n");

    Serial.println("client print done"); 
    long len = -1; 
    while (wifiClient.connected()) { 
        String line = wifiClient.readStringUntil('\n'); 
        if (line.startsWith("Content-Length:")) { 
            len = line.substring(15).toInt(); 
        } 
        if (line == "\r") { 
            Serial.println("headers received"); 
            break; 
        } 
    }

    Serial.print("Goging to read data with a size of: "); 
    Serial.print(len); 
    Serial.println(" bytes");

    Serial.printf("[HTTP] GET: %s\n",action.c_str());

    uint8_t buff[256];
    int imgAPos = 0;
    while (wifiClient.connected() && (len > 0 || len == -1)) {
        size_t size = wifiClient.available();
        Serial.print("wifiClient.available size: ");
        Serial.print(size);
        if (size>0) {
            int c = 0;
            do{
                c = wifiClient.readBytes(buff, 256);
                for (int i=0; i<c; i++)
                {
                    imgA[imgAPos] = buff[i];
                    imgAPos++;
                }      
                if (len > 0){ 
                    len -= c;
                }
            }while(c >= 256 );
        }
        else { 
            break; 
        }

        Serial.print(" - bytes left (len): ");
        Serial.println(len);
    }
    Serial.print("done. imgAPos: ");
    Serial.println(imgAPos);
} 

*/