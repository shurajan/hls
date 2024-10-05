#include "M3U8Parser.h"
#include <iostream>
#include <sstream>

namespace m3u8 {

    // Реализация метода parse для MasterPlaylist
    void MasterPlaylist::parse(const std::string& m3u8_content) {
        std::istringstream stream(m3u8_content);
        std::string line;
        StreamInfo streamInfo;

        while (std::getline(stream, line)) {
            if (line.find("#EXT-X-STREAM-INF:") != std::string::npos) {
                // Парсинг параметров плейлиста
                size_t bandwidthPos = line.find("BANDWIDTH=");
                if (bandwidthPos != std::string::npos) {
                    streamInfo.bandwidth = std::stoi(line.substr(bandwidthPos + 10, line.find(",", bandwidthPos) - bandwidthPos - 10));
                }

                size_t namePos = line.find("NAME=\"");
                if (namePos != std::string::npos) {
                    streamInfo.name = line.substr(namePos + 6, line.find("\"", namePos + 6) - namePos - 6);
                }

                size_t codecsPos = line.find("CODECS=\"");
                if (codecsPos != std::string::npos) {
                    streamInfo.codecs = line.substr(codecsPos + 8, line.find("\"", codecsPos + 8) - codecsPos - 8);
                }

                size_t resolutionPos = line.find("RESOLUTION=");
                if (resolutionPos != std::string::npos) {
                    streamInfo.resolution = line.substr(resolutionPos + 11, line.find(",", resolutionPos) - resolutionPos - 11);
                }
            } else if (line.find(".m3u8") != std::string::npos) {
                streamInfo.uri = line;
                streams.push_back(streamInfo);
            }
        }
    }

    // Реализация метода getStreams для MasterPlaylist
    const std::vector<StreamInfo>& MasterPlaylist::getStreams() const {
        return streams;
    }

    // Реализация метода print для MasterPlaylist
    void MasterPlaylist::print() const {
        for (const auto& stream : streams) {
            std::cout << "Stream Info:\n";
            std::cout << "  Bandwidth: " << stream.bandwidth << "\n";
            std::cout << "  Name: " << stream.name << "\n";
            std::cout << "  Codecs: " << stream.codecs << "\n";
            std::cout << "  Resolution: " << stream.resolution << "\n";
            std::cout << "  URI: " << stream.uri << "\n";
        }
    }

    // Реализация метода parse для MediaPlaylist
    void MediaPlaylist::parse(const std::string& m3u8_content) {
        std::istringstream stream(m3u8_content);
        std::string line;
        SegmentInfo segmentInfo;

        while (std::getline(stream, line)) {
            if (line.find("#EXTINF:") != std::string::npos) {
                // Парсинг длительности сегмента
                size_t durationPos = line.find(":");
                segmentInfo.duration = std::stof(line.substr(durationPos + 1, line.find(",") - durationPos - 1));
            } else if (line.find(".ts") != std::string::npos) {
                // Парсинг URI сегмента
                segmentInfo.uri = line;
                segments.push_back(segmentInfo);
            }
        }
    }

    // Реализация метода getSegments для MediaPlaylist
    const std::vector<SegmentInfo>& MediaPlaylist::getSegments() const {
        return segments;
    }

    // Реализация метода print для MediaPlaylist
    void MediaPlaylist::print() const {
        for (const auto& segment : segments) {
            std::cout << "Segment Info:\n";
            std::cout << "  Duration: " << segment.duration << " seconds\n";
            std::cout << "  URI: " << segment.uri << "\n";
        }
    }

} // namespace m3u8
