#include "PlaylistParserSiteCbr.h"
#include "NetworkClient.h"
#include <iostream>
#include <NetworkClientSocks5.h>

int main() {
    // Инициализируем клиент через указатель базового класса
    //network::NetworkClient client;
    network::NetworkClientSocks5 client = network::NetworkClientSocks5(
        "127.0.0.1",
        1080);

    PlaylistParserSiteCbr parser(&client);  // Передаем указатель на базовый класс

    std::string playlist_url = "https://somcdn.com/live-hls/st:some11234567_trns_h264/playlist.m3u8";

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
