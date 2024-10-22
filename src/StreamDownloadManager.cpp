#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <queue>
#include <thread>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <condition_variable>
#include <string>
#include <optional>
#include "m3u8/StreamDownloader.h"
#include "m3u8/ResolutionWrapper.h"
#include "network/NetworkClient.h"

namespace py = pybind11;

class Task {
public:
    std::string name;
    std::string master_playlist_url;
    m3u8::Resolution resolution;

    Task(const std::string& name, const std::string& url, const std::string& res_str)
        : name(name), master_playlist_url(url), resolution(m3u8::ResolutionWrapper::fromString(res_str)) {}
};

class DownloadManager {
public:
    DownloadManager() : stop_signal_(false) {}

    ~DownloadManager() {
        stop_all_tasks();
    }

    // Добавление новой задачи
    bool add_task(const std::string& task_name, const std::string& master_playlist_url, const std::string& resolution_str) {
        std::lock_guard lock(queue_mutex_);
        if (task_names_.contains(task_name)) {
            std::cerr << "Задача с именем \"" << task_name << "\" уже существует." << std::endl;
            return false; // Задача с таким именем уже существует
        }
        tasks_queue_.emplace(task_name, master_playlist_url, resolution_str);
        task_names_.insert(task_name);
        queue_condition_.notify_one(); // Уведомляем поток о добавлении новой задачи
        std::cout << "Добавлена задача: " << task_name << std::endl;
        return true;
    }

    // Запуск обработки задач
    void start_processing() {
        stop_signal_ = false; // Сбрасываем сигнал остановки
        processing_thread_ = std::thread(&DownloadManager::process_tasks, this);
    }

    // Остановка всех задач
    void stop_all_tasks() {
        stop_signal_ = true;
        queue_condition_.notify_all(); // Уведомляем поток для выхода из ожидания
        if (processing_thread_.joinable()) {
            processing_thread_.join();
        }
    }

    // Проверка, запущен ли менеджер
    bool is_running() const {
        return !stop_signal_;
    }

private:
    std::queue<Task> tasks_queue_;
    std::unordered_set<std::string> task_names_;
    std::mutex queue_mutex_;
    std::condition_variable queue_condition_;
    std::thread processing_thread_;
    std::atomic<bool> stop_signal_;

    // Метод обработки задач
    void process_tasks() {
        while (true) {
            std::optional<Task> current_task;
            {
                std::unique_lock lock(queue_mutex_);
                // Ожидание новых задач или сигнала остановки
                queue_condition_.wait(lock, [this] { return !tasks_queue_.empty() || stop_signal_; });

                // Проверка на сигнал остановки
                if (stop_signal_ && tasks_queue_.empty()) {
                    break; // Завершаем работу, если установлен сигнал остановки и очередь пуста
                }

                if (!tasks_queue_.empty()) {
                    // Извлекаем задачу из очереди
                    current_task = tasks_queue_.front();
                    task_names_.erase(current_task->name); // Удаляем имя задачи из множества
                    tasks_queue_.pop();
                }
            }

            if (current_task) {
                // Инициализируем компоненты для загрузки
                network::NetworkClient network_client;
                m3u8::StreamSegments parser(&network_client, current_task->master_playlist_url, current_task->resolution);
                m3u8::StreamDownloader downloader(parser, stop_signal_);

                // Выполняем загрузку потока
                std::string temp_file = current_task->name + ".ts";
                std::cout << "Запуск загрузки: " << current_task->name << std::endl;
                downloader.download_stream(current_task->master_playlist_url, temp_file);
            }
        }
    }
};

PYBIND11_MODULE(myextension, m) {
    py::class_<DownloadManager>(m, "DownloadManager")
        .def(py::init<>())
        .def("add_task", &DownloadManager::add_task)
        .def("start_processing", &DownloadManager::start_processing)
        .def("stop_all_tasks", &DownloadManager::stop_all_tasks)
        .def("is_running", &DownloadManager::is_running);
}