# Spotify Mate
## What is SpotifyMate?
Spotify Mate is a device which uses Spotifys WebAPI to give visuals as to what you are currently listening to. Aswell as some functionality to Play, Pause, Skip Forward, Skip Backwards, Add to liked songs and adjust the volume. The device also has one customisable button which can be used for things such as playing a specific playlist for example.
Some Renders of the Hardware side will be added soon, and some images of the fireware GUI will also be added soon.

## How does it work?
SpotifyMate runs a custom C++ based firmware, providing a GUI which gives all necesarry details. The device will enter a 'setup mode' when first powered on, or without an active network connection, which allows the user to connect to the device and add it to their home WiFi network. The device will also need linking to the users Spotify account.

Once this is the done, the device should automatically connect and update whenever the current played track is changed. Should listening become inactive for 5 minutes or longer, the device will enter a sleep mode

More info to come...

## The Software
**Current Version: beta-0.0.1**
Version: beta-0.0.1
- Has a Captive Portal setup to allow connection and setup on device power on

