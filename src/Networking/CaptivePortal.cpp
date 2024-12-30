//Code adpated from https://github.com/CDFER/Captive-Portal-ESP32 

#include <Arduino.h> 

#include <Preferences.h>
#include <CaptivePortal.h>

// Captive Portal
#include <AsyncTCP.h>  //https://github.com/me-no-dev/AsyncTCP using the latest dev version from @me-no-dev
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>	//https://github.com/me-no-dev/ESPAsyncWebServer using the latest dev version from @me-no-dev
#include <esp_wifi.h>			//Used for mpdu_rx_disable android workaround
#include <Preferences.h>

Preferences pref;

// Pre reading on the fundamentals of captive portals https://textslashplain.com/2022/06/24/captive-portals/

//const char *ssid = "SpotifyMate-AP0001";  // FYI The SSID can't have a space in it.
// const char * password = "12345678"; //Atleast 8 chars
const char *password = NULL;  // no password

#define MAX_CLIENTS 4	// ESP32 supports up to 10 but I have not tested it yet
#define WIFI_CHANNEL 6	// 2.4ghz channel 6 https://en.wikipedia.org/wiki/List_of_WLAN_channels#2.4_GHz_(802.11b/g/n/ax)

const IPAddress localIP(4, 3, 2, 1);		   // the IP address the web server, Samsung requires the IP to be in public space
const IPAddress gatewayIP(4, 3, 2, 1);		   // IP address of the network should be the same as the local IP for captive portals
const IPAddress subnetMask(255, 255, 255, 0);  // no need to change: https://avinetworks.com/glossary/subnet-mask/

const String localIPURL = "http://4.3.2.1";	 // a string version of the local IP with http, used for redirecting clients to your webpage

const char* SSID_PARAM = "network-ssid";
const char* PASS_PARAM = "network-password";

const char index_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>

<head>
    <title>SpotifyMate Connect</title>
</head>

<body>

    <h1 class="main_title">Spotify <span class="main_title_1">Mate</span></h1>
    <h2 class="connected_device_h">Currently Connected to: <span class="device_name">SpotifyMate-AP0001</span></h2>

    <div class="network_connect">
        <h1 class="network_title">Connect to your network</h1>

        <form action="/get">
            <p class="ssid_title">SSID</p>
            <input id="ssid" class="network_field" placeholder="SSID" type="text" name="network-ssid"/> 

            <p class="pass_title">Password</p>
            <input id="pass" class="network_field" placeholder="Password" type="password" name="network-password"/> 

            <input type="submit" class="connect_button">Connect!</input>

        </form>



    </div>

    <p class="connected_status"></p>

    <a href="https://www.spotify.com" class="sp_button">
        <div class="connect_spotify_button">
            <img class="spotify_img", src="./data/spotify.png", alt="Spotify Logo", width="40", height="40"/>
            <h1 class="connect_text">Connect to Spotify</h1>
        </div>
    </a>

</body>

<style>
    @font-face {
    font-family: Metropolis-Bold;
    src: url('./data/fonts/Metropolis-Bold.otf');
}

@font-face {
    font-family: Metropolis-Regular;
    src: url('./data/fonts/Metropolis-Regular.otf');
}

@font-face {
    font-family: Metropolis-Light;
    src: url('./data/fonts/Metropolis-Light.otf');
}

body{
    background: #121212;
}

.main_title{
    color: #ffffff;
    font-family: Metropolis-Regular;
    font-size: 48px;
    margin-left: auto;
    margin-right: auto;
    width: 295px;
    margin-bottom: 0;
}

.main_title_1{
    color:#1ed760;
}

.connected_device_h{
    color:#ffffff;
    font-family: Metropolis-Regular;
    font-size: 12px;
    width: 276px;
    margin-left: auto;
    margin-right: auto;
    margin-top: 3%;
}

.device_name{
    color: #1ed760;
    font-family: Consolas,monaco,monospace;
}

.network_connect{
    background-color: #1e1e1e;
    border-radius: 15px;
    width: 340px;
    height: 255px;
    padding-top: 5px;
    padding-left: 25px;
    margin-left: auto;
    margin-right: auto;
    margin-top: 5%;
    display: block;
}

.network_title{
    color: #ffffff;
    font-family: Metropolis-Light;
    font-size: 24px;
    width: 292px; 
    margin: 0;
    padding: auto;
    margin-left: 5%;

    align-content: center;
    margin-top: 10px;
    display: grid;
}

.ssid_title{
    color:#ffffff;
    font-size: 14px;
    font-family: Consolas,monaco,monospace;
    margin: 0px;
    margin-top: 16px;
    margin-bottom: 7px;
}

.pass_title{
    color:#ffffff;
    font-size: 14px;
    font-family: Consolas,monaco,monospace;
    margin: 0px;
    margin-top: 16px;
    margin-bottom: 7px;
}

.network_field{
    background-color: #424242;
    border-radius: 50px;
    width: 299px;
    height: 20px;
    margin: 0;
    padding-left: 10px;
    font-family: monospace;
    color:#ffffff;
}

.connect_button{
    background-color: #1ed760;
    width: 150px;
    height: 40px;
    border-radius: 50px;
    border: none;
    color:#ffffff;
    font-size: 22px;
    font-family: Metropolis-Light;
    margin: 0;
    margin-top: 7%;
    margin-left: 23%;
    cursor: pointer;
}

.connected_status{
    color: #1ed760;
    font-family: Metropolis-Regular;
    font-size: 20px;
    width: 89px;
    text-align: center;
    margin: 0;
    margin-top: 25px;
    margin-bottom: 25px;
    margin-left: auto;
    margin-right: auto;

}

.sp_button{
    background-color: #121212;
    border: none;
    text-decoration: none;
}

.connect_spotify_button{
    margin-top: 20px;
    margin-left: 15px;
    width: 250px;
    height: 50px;
    background-color: #1ed760;
    border-radius: 50px;
    display: flex;
    margin: 0;
    margin-left: auto;
    margin-right: auto;
    margin-top: 20px;
}

.spotify_img{
    margin-left: 6px;
    margin-top: 5px;

}

.connect_text{
    font-family: Metropolis-Regular;
    color: #121212;
    font-size: 20px;
    margin-left: 10px;
    margin-top: 0px;
    margin-top: auto;
    margin-bottom: auto;
}
</style>

</html> 
)=====";

DNSServer dnsServer;
AsyncWebServer server(80);

void setUpDNSServer(DNSServer &dnsServer, const IPAddress &localIP) {
// Define the DNS interval in milliseconds between processing DNS requests
#define DNS_INTERVAL 30

	// Set the TTL for DNS response and start the DNS server
	dnsServer.setTTL(3600);
	dnsServer.start(53, "*", localIP);
}

void startSoftAccessPoint(const char *ssid, const char *password, const IPAddress &localIP, const IPAddress &gatewayIP) {
// Define the maximum number of clients that can connect to the server
#define MAX_CLIENTS 4
// Define the WiFi channel to be used (channel 6 in this case)
#define WIFI_CHANNEL 6

	// Set the WiFi mode to access point and station
	WiFi.mode(WIFI_MODE_AP);

	// Define the subnet mask for the WiFi network
	const IPAddress subnetMask(255, 255, 255, 0);

	// Configure the soft access point with a specific IP and subnet mask
	WiFi.softAPConfig(localIP, gatewayIP, subnetMask);

	// Start the soft access point with the given ssid, password, channel, max number of clients
	WiFi.softAP(ssid, password, WIFI_CHANNEL, 0, MAX_CLIENTS);

	// Disable AMPDU RX on the ESP32 WiFi to fix a bug on Android
	esp_wifi_stop();
	esp_wifi_deinit();
	wifi_init_config_t my_config = WIFI_INIT_CONFIG_DEFAULT();
	my_config.ampdu_rx_enable = false;
	esp_wifi_init(&my_config);
	esp_wifi_start();
	vTaskDelay(100 / portTICK_PERIOD_MS);  // Add a small delay
}

void setUpWebserver(AsyncWebServer &server, const IPAddress &localIP) {
	//======================== Webserver ========================
	// WARNING IOS (and maybe macos) WILL NOT POP UP IF IT CONTAINS THE WORD "Success" https://www.esp8266.com/viewtopic.php?f=34&t=4398
	// SAFARI (IOS) IS STUPID, G-ZIPPED FILES CAN'T END IN .GZ https://github.com/homieiot/homie-esp8266/issues/476 this is fixed by the webserver serve static function.
	// SAFARI (IOS) there is a 128KB limit to the size of the HTML. The HTML can reference external resources/images that bring the total over 128KB
	// SAFARI (IOS) popup browser has some severe limitations (javascript disabled, cookies disabled)

	// Required
	server.on("/connecttest.txt", [](AsyncWebServerRequest *request) { request->redirect("http://logout.net"); });	// windows 11 captive portal workaround
	server.on("/wpad.dat", [](AsyncWebServerRequest *request) { request->send(404); });								// Honestly don't understand what this is but a 404 stops win 10 keep calling this repeatedly and panicking the esp32 :)

	// Background responses: Probably not all are Required, but some are. Others might speed things up?
	// A Tier (commonly used by modern systems)
	server.on("/generate_204", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });		   // android captive portal redirect
	server.on("/redirect", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // microsoft redirect
	server.on("/hotspot-detect.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });  // apple call home
	server.on("/canonical.html", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });	   // firefox captive portal call home
	server.on("/success.txt", [](AsyncWebServerRequest *request) { request->send(200); });					   // firefox captive portal call home
	server.on("/ncsi.txt", [](AsyncWebServerRequest *request) { request->redirect(localIPURL); });			   // windows call home

	// B Tier (uncommon)
	//  server.on("/chrome-variations/seed",[](AsyncWebServerRequest *request){request->send(200);}); //chrome captive portal call home
	//  server.on("/service/update2/json",[](AsyncWebServerRequest *request){request->send(200);}); //firefox?
	//  server.on("/chat",[](AsyncWebServerRequest *request){request->send(404);}); //No stop asking Whatsapp, there is no internet connection
	//  server.on("/startpage",[](AsyncWebServerRequest *request){request->redirect(localIPURL);});

	// return 404 to webpage icon
	server.on("/favicon.ico", [](AsyncWebServerRequest *request) { request->send(404); });	// webpage icon

	// Serve Basic HTML Page
	server.on("/", HTTP_ANY, [](AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
		response->addHeader("Cache-Control", "public,max-age=31536000");  // save this file to cache for 1 year (unless you refresh)
		request->send(response);
		Serial.println("Served Basic HTML Page");
	});

  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request) {
    String network_ssid_msg;
    String network_ssid_param;

    String network_pass_msg;
    String network_pass_param;

    bool ssid_got = false;
    bool pass_got = false;

    //Get SSID
    if(request->hasParam(SSID_PARAM)) {
      network_ssid_msg = request->getParam(SSID_PARAM)->value();
      network_ssid_param = SSID_PARAM;
      ssid_got = true;
    } else {
      network_ssid_msg = "No message sent";
      network_ssid_param = "none";
    }

    //Get Pass
    if(request->hasParam(PASS_PARAM)) {
      network_pass_msg = request->getParam(PASS_PARAM)->value();
      network_pass_param = PASS_PARAM;
      pass_got = true;
    } else {
      network_pass_msg = "No message sent";
      network_pass_param = "none";
    }

    Serial.println("### SSID:" + network_ssid_msg);
    Serial.println("### PASS:" + network_pass_msg);

    pref.begin("spotify-mate", false);
    pref.putString("ssid", network_ssid_msg);
    pref.putString("pass", network_pass_msg);
    pref.end();

    if(ssid_got && pass_got){
      //Both the SSID and Password are gotten
      //Connect to network
      Serial.println("Attempting Network Connection");
    } else {
      //Do not connect to network
      //Display error message
      Serial.println("Could not attempt network Connection");
    }

    //ToDo: Change this to send the HTML to connect to spotify
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + network_pass_param + ") with value: " + network_pass_msg +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });

	// the catch all
	server.onNotFound([](AsyncWebServerRequest *request) {
		request->redirect(localIPURL);
		Serial.print("onnotfound ");
		Serial.print(request->host());	// This gives some insight into whatever was being requested on the serial monitor
		Serial.print(" ");
		Serial.print(request->url());
		Serial.print(" sent redirect to " + localIPURL + "\n");
	});
}

void setupCaptivePortal(const char *ssid_parse) {
	// Set the transmit buffer size for the Serial object and start it with a baud rate of 115200.
	Serial.setTxBufferSize(1024);
	Serial.begin(115200);

	// Wait for the Serial object to become available.
	while (!Serial)
		;

	// Print a welcome message to the Serial port.
	Serial.println("\n\nCaptive Test, V0.5.0 compiled " __DATE__ " " __TIME__ " by CD_FER");  //__DATE__ is provided by the platformio ide
	Serial.printf("%s-%d\n\r", ESP.getChipModel(), ESP.getChipRevision());

	startSoftAccessPoint(ssid_parse, password, localIP, gatewayIP);

	setUpDNSServer(dnsServer, localIP);

	setUpWebserver(server, localIP);
	server.begin();

	Serial.print("\n");
	Serial.print("Startup Time:");	// should be somewhere between 270-350 for Generic ESP32 (D0WDQ6 chip, can have a higher startup time on first boot)
	Serial.println(millis());
	Serial.print("\n");
}

void captivePortalLoop() {
	dnsServer.processNextRequest();	 // I call this atleast every 10ms in my other projects (can be higher but I haven't tested it for stability)
	delay(DNS_INTERVAL);			 // seems to help with stability, if you are doing other things in the loop this may not be needed
}