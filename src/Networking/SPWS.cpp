#include <WebServer.h>
#include <Arduino.h>
#include <WiFi.h>

void createWebServer(){
    WiFiServer server(80);
    String header;

    Serial.begin(115200);

    unsigned long currentTime = millis();
    unsigned long previousTime = 0; 
    const long timeoutTime = 2000;


    WiFiClient client = server.available();

    if(client) {
        currentTime = millis();
        previousTime = currentTime;
        Serial.println("New Client");
        String currentLine = "";

        while(client.connected() && currentTime - previousTime <= timeoutTime) {
            currentTime = millis();
            if(client.available()){
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                header += c;
                if (c == '\n') {                    // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();
                
                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><title>SpotifyMate Connect</title></head>");
                        // CSS to style the on/off buttons 
                        // Feel free to change the background-color and font-size attributes to fit your preferences
                        client.println("<body><form action=\"/get\">");
                        client.println("<input type=\"submit\" class=\"connect_spotify_button\" value=\"Connect to Spotify\" name=\"spotify_connect\"/>");
                        client.println("</form></body>");
                        client.println("<style>body{ background: #121212; }");
                        client.println(".connect_spotify_button{margin-top: 20px; margin-left: 15px;");
                        client.println("width: 250px; height: 50px;");
                        client.println("background-color: #1ed760; border-radius: 50px;");
                        client.println("display: flex; margin: 0;");
                        client.println("margin-left: auto; margin-right: auto; margin-top: 20px;");
                        client.println("color: #121212; font-size: 22px; font-family: monospace, sans-serif; }</style></html>");
            
                        // The HTTP response ends with another blank line
                        client.println();
                        // Break out of the while loop
                        break;
                    } else { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
  }
}