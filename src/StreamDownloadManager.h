#ifndef STREAM_DOWNLOAD_MANAGER_H
#define STREAM_DOWNLOAD_MANAGER_H

#include <string>
#include <vector>
#include <thread>
#include <future>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include "NetworkClientBase.h" // Базовый класс для NetworkClient

class StreamDownloadManager {
public:
    // Конструктор, принимающий ссылку на NetworkClient
    StreamDownloadManager(network::NetworkClientBase& network_client);
    ~StreamDownloadManager();

    // Метод для добавления задачи на загрузку потока
    void add_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution);

    // Метод для завершения всех задач
    void stop_all();

    // Метод для проверки состояния флага завершения
    static bool is_stopped();

private:
    // Ссылка на NetworkClient для загрузки данных
    network::NetworkClientBase& network_client_;
    static std::atomic<bool> stop_signal_; // Статический флаг завершения работы
    std::mutex tasks_mutex_; // Мьютекс для синхронизации доступа к задачам
    std::mutex file_mutex;   // Мьютекс для синхронизации доступа к файлам

    // Карта для хранения текущих задач
    std::unordered_map<std::string, std::future<void>> download_tasks_;

    // Внутренний метод для загрузки потока
    void download_stream_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution);

    // Метод для обработки временных файлов
    void finalize_download(const std::string& temp_file, const std::string& final_file);

    // Статический метод для обработки сигнала
    static void handle_signal(int signal);
};

#endif // STREAM_DOWNLOAD_MANAGER_H
