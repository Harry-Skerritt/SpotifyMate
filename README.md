![SpotifyMate Logo](https://github.com/Harry-Skerritt/SpotifyMate/blob/main/assets/sm_logo_full.png)

![Dev Badge](https://img.shields.io/badge/IN_DEVELOPMENT-green)


![Release Badge](https://img.shields.io/badge/Release-Pending-blue)

![Beta Badge](https://img.shields.io/badge/beta-v1.0.0-dark_green)

![PlatformIO Badge](https://img.shields.io/badge/Built_with-PlatformIO-orange)

[![ESP32](https://img.shields.io/badge/ESP-32-000000?labelColor=%23CC101F)](https://www.espressif.com/en/products/socs/esp32)


## What is SpotifyMate?
Spotify Mate is a device which uses Spotifys WebAPI to give visuals as to what you are currently listening to. Aswell as some functionality to Play, Pause, Skip Forward, Skip Backwards, Add to liked songs and adjust the volume. The device also has one customisable button which can be used for things such as playing a specific playlist for example.

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
SpotifyMate runs a custom C++ based firmware, providing a GUI which gives all necesarry details. 

The device will enter a 'setup mode' when first powered on, or without an active network connection, which allows the user to connect to the device and add it to their home WiFi network. 

The device will also need linking to the users Spotify account.

Once this is the done, the device should automatically connect and update whenever the current played track is changed. Should listening become inactive for 5 minutes or longer, the device will enter a sleep mode

More info to come...

## Libraries
SpotifyMate is based around and would not function without several libraries. These are:

[Spotify-API-Arduino](https://github.com/witnessmenow/spotify-api-arduino) library by witnessmenow on Github

[WiFiManager](https://github.com/tzapu/WiFiManager) library by tzapu on Github




## How to make
Coming Soon!

## The Software
**Current Version: beta-1.0.0**

Version: beta-1.0.0
- Captive Portal is setup and working
- Allows for generation of a refresh token
- Has most of the setup GUI
- API side of the current playing song is there, needs more work however



Version: beta-0.0.1
- Has a Captive Portal setup to allow connection and setup on device power on

