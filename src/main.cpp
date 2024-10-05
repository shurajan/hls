#include "LiveStreamSegments.h"
#include "StreamDownloader.h"
#include <iostream>
#include <NetworkClient.h>
#include <string>

int main() {
    // Путь к HLS мастер-плейлисту
    std::string playlist_url = "https://example.com/playlist.m3u8";
    // Выбор разрешения
    m3u8::Resolution resolution = m3u8::Resolution::P720;

    // Файл для сохранения загруженного стрима
    std::string output_file = "output_stream.ts";
    network::NetworkClient network_client = network::NetworkClient();
    // Создаем объект парсера и загрузчика
    m3u8::LiveStreamSegments parser = m3u8::LiveStreamSegments(playlist_url,resolution,&network_client);
    m3u8::StreamDownloader downloader(parser);

    // Запуск загрузки
    downloader.download_stream(playlist_url, resolution, output_file);

    return 0;
}
