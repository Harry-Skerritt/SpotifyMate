![SpotifyMate Logo](https://github.com/Harry-Skerritt/SpotifyMate/blob/main/assets/sm_logo_full.png)

![Dev Badge](https://img.shields.io/badge/IN_DEVELOPMENT-green)


![Release Badge](https://img.shields.io/badge/Release-beta2.0.0-blue)

![Beta Badge](https://img.shields.io/badge/beta-v2.0.0-dark_green)

![PlatformIO Badge](https://img.shields.io/badge/Built_with-PlatformIO-orange)

[![ESP32](https://img.shields.io/badge/ESP-32-000000?labelColor=%23CC101F)](https://www.espressif.com/en/products/socs/esp32)


## What is SpotifyMate?
Spotify Mate is a device which uses Spotifys WebAPI to give visuals as to what you are currently listening to. Aswell as some functionality to Play, Pause and add to liked songs.

[Make Your Own](https://github.com/Harry-Skerritt/SpotifyMate?tab=readme-ov-file#how-to-make)

Some Renders of the Hardware side will be added soon, and some more images of the fireware GUI will also be added soon.

**IMAGES BELOW ARE MOCKUPS**

### The Main Screen
![SpotifyMate Home](https://github.com/Harry-Skerritt/SpotifyMate/blob/main/assets/mock_main_screen.png)

This is what the main screen should look like on the final device.

### WiFi Provisioning
![SpotifyMate Provisioning](https://github.com/Harry-Skerritt/SpotifyMate/blob/main/assets/mock_provisioning.png)

The device also features WiFi provisioning for easy network connectivity

![SpotifyMate Web Provisioning](https://github.com/Harry-Skerritt/SpotifyMate/blob/main/assets/provisioning_web.png)

The device hosts its own webserver allowing for this provisioning to be easy to use. (This is now outdated)


## How does it work?
The SpotifyMate firmware, has a GUI which makes the setup and use of the device pretty simple, but to explain a bit more about how the deice works: 

- The device will enter a 'provisiong mode' when first powered on, or without an active network connection, which allows the user to connect to the device and add it to their home WiFi network. 

- The device will also need linking to the users Spotify account, which can be done through a QR code displayed on screen.

- Once this is the done, the device should automatically connect and update whenever the current played track is changed. Should listening become inactive for 5 minutes or longer, the device will enter a sleep mode

- As the device is intended to have a touch screen, a single tap will play/pause the audio, and a double tap will add the song to the 'Liked Songs' playlist on Spotify.

## Libraries
SpotifyMate is based around and would not function without several libraries. These are:


[WiFiManager](https://github.com/tzapu/WiFiManager) library by tzapu on Github, for WiFi provisioning.

[TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) library by Bodmer on Github, for all of the graphics.

[TJpg_Decoder](https://github.com/Bodmer/TJpg_Decoder) library by Bodmer on Github, for pushing JPG's from a URL to the screen.

## How to make
Code: [Right Here!](https://github.com/Harry-Skerritt/SpotifyMate/tree/main)
STLs: [Right Here!](https://github.com/Harry-Skerritt/SpotifyMate/tree/main)

Alternativly all the above are avalable here:
[Instructables](https://www.instructables.com/preview/EMD055BM5I72W15/)

## The Software
**Current Version: beta-2.1.0**

Version: beta-2.1.0
- Included the loop functionality so the device works with basic functionality
- Fixed some bugs regarding API
- Introduced some more bugs
- Lacking in touch functionality
- Overall in a functional, but pre-release state

Version: beta-2.0.0
- Moved away from the bigger more complex design (Will be developed in the future...) [Code here](https://github.com/Harry-Skerritt/SpotifyMate/tree/landscape)
- Kept all the same code as previously, but amended for a smaller screen
- Added the home page to display song info
- Moved over to using my own API calls rather than those from a library

Version: beta-1.0.0
- Captive Portal is setup and working
- Allows for generation of a refresh token
- Has most of the setup GUI
- API side of the current playing song is there, needs more work however

Version: beta-0.0.1
- Has a Captive Portal setup to allow connection and setup on device power on

