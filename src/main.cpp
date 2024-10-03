#include "PlaylistParserSiteCbr.h"
#include <iostream>

int main() {
    PlaylistParserSiteCbr parser;

    std::string playlist_url = "https://somecdn.com/live-hls/h264/playlist.m3u8";

    try {
        std::vector<std::string> ts_links = parser.parse_m3u8_playlist(playlist_url, Resolution::P720);
        for (const std::string& link : ts_links) {
            std::cout << "Segment URL: " << link << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
