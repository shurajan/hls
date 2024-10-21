#include "StreamDownloadManager.h"
#include "StreamDownloader.h"
#include <iostream>
#include <fstream>
#include <csignal>
#include <filesystem>

namespace fs = std::filesystem;
std::atomic<bool> StreamDownloadManager::stop_signal_(false);

StreamDownloadManager::StreamDownloadManager(network::NetworkClient& network_client)
    : network_client_(network_client) {
    // Устанавливаем обработчик сигнала SIGINT для завершения задач при получении сигнала Ctrl-C
    std::signal(SIGINT, StreamDownloadManager::handle_signal);
}

StreamDownloadManager::~StreamDownloadManager() {
    // Останавливаем все задачи при завершении объекта
    stop_all();
}

bool StreamDownloadManager::add_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution) {
    std::lock_guard<std::mutex> lock(tasks_mutex_);
    // Проверяем, существует ли уже задача с таким же выходным файлом
    if (download_tasks_.count(output_file) > 0) {
        std::cerr << "Задача для этого файла уже существует: " << output_file << std::endl;
        return false;
    }

    // Добавляем новую задачу на загрузку в асинхронном потоке
    download_tasks_[output_file] = std::async(std::launch::async, [this, master_playlist_url, output_file, resolution]() {
        try {
            download_stream_task(master_playlist_url, output_file, resolution);
        } catch (const std::exception& e) {
            std::cerr << "Ошибка при загрузке потока " << master_playlist_url << ": " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Неизвестная ошибка при загрузке потока " << master_playlist_url << std::endl;
        }
    });
    return true;
}

void StreamDownloadManager::stop_all() {
    // Устанавливаем флаг остановки
    stop_signal_ = true;

    std::lock_guard<std::mutex> lock(tasks_mutex_);
    // Ожидаем завершения всех задач
    for (auto& task : download_tasks_) {
        if (task.second.valid()) {
            try {
                task.second.get(); // Получаем результат выполнения, чтобы поймать возможные исключения
            } catch (const std::exception& e) {
                std::cerr << "Ошибка в одной из задач: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "Неизвестная ошибка в одной из задач." << std::endl;
            }
        }
    }
    // Очищаем список задач
    download_tasks_.clear();
}

bool StreamDownloadManager::is_stopped() const {
    return stop_signal_;
}

void StreamDownloadManager::download_stream_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution) {
    std::string temp_file = output_file + ".part";
    try {
        // Создаем объекты для загрузки потока
        m3u8::StreamSegments parser(&network_client_, master_playlist_url, resolution);
        m3u8::StreamDownloader downloader(parser, stop_signal_);

        // Выполняем загрузку потока
        downloader.download_stream(master_playlist_url, temp_file);

        // Если загрузка завершена успешно, переименовываем временный файл
        finalize_download(temp_file, output_file);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при загрузке потока: " << e.what() << std::endl;
        // Пытаемся переименовать временный файл даже в случае ошибки
        finalize_download(temp_file, output_file);
    }
}

void StreamDownloadManager::finalize_download(const std::string& temp_file, const std::string& final_file) {
    std::lock_guard<std::mutex> lock(file_mutex_);
    if (fs::exists(temp_file)) {
        // Переименовываем временный файл в конечное имя
        fs::rename(temp_file, final_file);
        std::cout << "Файл " << final_file << " успешно сохранен." << std::endl;
    } else {
        std::cerr << "Временный файл " << temp_file << " не найден для переименования." << std::endl;
    }
}

void StreamDownloadManager::handle_signal(int signal) {
    if (signal == SIGINT) {
        // Устанавливаем флаг остановки при получении SIGINT
        stop_signal_ = true;
        std::cout << "Сигнал завершения получен. Завершаем все задачи..." << std::endl;
    }
}