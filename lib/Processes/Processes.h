#ifndef PROCESSES_H
#define PROCESSES_H

void read_in_keys_spiffs();

void authSpotify();

bool hasRefreshToken();

std::array<String, 3> getCurrentSongDetails();

void downloadFile(uint8_t * imgA);

#endif