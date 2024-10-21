#ifndef STREAM_DOWNLOAD_MANAGER_H
#define STREAM_DOWNLOAD_MANAGER_H

#include <string>
#include <map>
#include <future>
#include <atomic>
#include <mutex>
#include "NetworkClient.h"

class StreamDownloadManager {
public:
    StreamDownloadManager(network::NetworkClient& network_client);
    ~StreamDownloadManager();

    bool add_task(const std::string& name, const std::string& master_playlist_url, const std::string& resolution);
    void stop_all();
    bool is_stopped() const;

private:
    network::NetworkClient& network_client_;
    static std::atomic<bool> stop_signal_;
    std::map<std::string, std::future<void>> download_tasks_;
    mutable std::mutex tasks_mutex_;
    std::mutex file_mutex_;

    void download_stream_task(const std::string& master_playlist_url, const std::string& output_file, const std::string& resolution);
    void finalize_download(const std::string& temp_file, const std::string& final_file);
    static void handle_signal(int signal);
    std::string generate_output_filename(const std::string& name);
};

#endif // STREAM_DOWNLOAD_MANAGER_H