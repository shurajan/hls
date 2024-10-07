#include "LiveStreamSegments.h"
#include "StreamDownloader.h"
#include <iostream>
#include <NetworkClient.h>
#include <NetworkClientSocks5.h>
#include <string>

int main() {
    // Путь к HLS мастер-плейлисту
    std::string playlist_url = "https://example.com/playlist.m3u8";


    // Выбор разрешения
    const std::string resolution = "Max";

    // Файл для сохранения загруженного стрима
    std::string output_file = "output_stream.ts";
    network::NetworkClientSocks5 network_client = network::NetworkClientSocks5("127.0.0.1", 1080);
    // Создаем объект парсера и загрузчика
    m3u8::LiveStreamSegments parser = m3u8::LiveStreamSegments(&network_client, playlist_url, resolution);
    m3u8::StreamDownloader downloader(parser);

    // Запуск загрузки
    downloader.download_stream(playlist_url,  output_file);

    return 0;
}
