#include "StreamDownloadManager.h"
#include "LiveStreamSegments.h"
#include "StreamDownloader.h"
#include <iostream>
#include <fstream>
#include <csignal>
#include <chrono>
#include <filesystem>

namespace fs = std::filesystem;

// Инициализация статического члена
std::atomic<bool> StreamDownloadManager::stop_signal_(false);

StreamDownloadManager::StreamDownloadManager(network::NetworkClientBase& network_client)
    : network_client_(network_client) {
    // Обработчик сигнала Ctrl-C
    std::signal(SIGINT, StreamDownloadManager::handle_signal);
}

StreamDownloadManager::~StreamDownloadManager() {
    stop_all();
}

void StreamDownloadManager::add_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    if (download_tasks_.count(output_file) > 0) {
        std::cerr << "Задача для этого файла уже существует: " << output_file << std::endl;
        return;
    }

    // Добавляем новую задачу на загрузку
    download_tasks_[output_file] = std::async(std::launch::async, [this, master_playlist_url, output_file, resolution]() {
        try {
            this->download_stream_task(master_playlist_url, output_file, resolution);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при загрузке потока " << master_playlist_url << ": " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Неизвестная ошибка при загрузке потока " << master_playlist_url << std::endl;
        }
    });
}

void StreamDownloadManager::stop_all() {
    stop_signal_ = true;

    // Ожидаем завершения всех задач
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    for (auto& task : download_tasks_) {
        if (task.second.valid()) {
            try {
                task.second.get(); // Получаем результат выполнения, чтобы поймать исключения, если они были
            } catch (const std::exception& e) {
                std::cerr << "Ошибка в одной из задач: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка в одной из задач." << std::endl;
            }
        }
    }
    download_tasks_.clear();
}

bool StreamDownloadManager::is_stopped() {
    return stop_signal_;
}

void StreamDownloadManager::download_stream_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution) {
    std::string temp_file = output_file + ".part";
    try {
        // Создаем объекты для загрузки потока
        m3u8::LiveStreamSegments parser(&network_client_, master_playlist_url, resolution);
        m3u8::StreamDownloader downloader(parser, stop_signal_);

        // Загружаем поток
        downloader.download_stream(master_playlist_url, temp_file);

        // Если загрузка завершена успешно, переименовываем файл
        finalize_download(temp_file, output_file);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке потока: " << e.what() << std::endl;
        // Пытаемся переименовать файл даже в случае ошибки
        finalize_download(temp_file, output_file);
    }
}

void StreamDownloadManager::finalize_download(const std::string& temp_file, const std::string& final_file) {
    std::lock_guard<std::mutex> lock(file_mutex);
    if (fs::exists(temp_file)) {
        fs::rename(temp_file, final_file);
        std::cout << "Файл " << final_file << " успешно сохранен." << std::endl;
    } else {
        std::cerr << "Временный файл " << temp_file << " не найден для переименования." << std::endl;
    }
}

void StreamDownloadManager::handle_signal(int signal) {
    if (signal == SIGINT) {
        stop_signal_ = true;
        std::cout << "Сигнал завершения получен. Завершаем все задачи..." << std::endl;
    }
}
