//
// Created by sasha on 07.10.2024.
//

#ifndef RESOLUTIONWRAPPER_H
#define RESOLUTIONWRAPPER_H

#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <regex>

namespace m3u8 {
    enum class Resolution {
        Max,
        Min,
        P144,
        P240,
        P360,
        P480,
        P540,
        P720,
        P1080,
        P1440,
        P2160
    };

    class ResolutionWrapper {
    public:
        // Метод для преобразования значения enum в строку
        static std::string toString(Resolution res) {
            switch (res) {
                case Resolution::Max: return "7680x4320";
                case Resolution::Min: return "256x144";
                case Resolution::P144: return "256x144";
                case Resolution::P240: return "426x240";
                case Resolution::P360: return "640x360";
                case Resolution::P480: return "854x480";
                case Resolution::P540: return "960x540";
                case Resolution::P720: return "1280x720";
                case Resolution::P1080: return "1920x1080";
                case Resolution::P1440: return "2560x1440";
                case Resolution::P2160: return "3840x2160";
                default: return "Unknown";
            }
        }

        // Метод для преобразования строки в значение enum
        static Resolution fromString(const std::string &str) {
            // Приведение строки к единому формату: убираем пробелы и приводим символы разделителя к 'x'
            std::string normalized_str = str;

            // Удаляем пробелы
            normalized_str.erase(remove_if(normalized_str.begin(), normalized_str.end(), ::isspace),
                                 normalized_str.end());

            // Приводим все 'X' в нижний регистр
            std::replace(normalized_str.begin(), normalized_str.end(), 'X', 'x');

            // Проверяем на конкретные разрешения
            if (normalized_str == "Max") return Resolution::Max;
            else if (normalized_str == "Min") return Resolution::Min;
            else if (normalized_str == "144p") return Resolution::P144;
            else if (normalized_str == "240p") return Resolution::P240;
            else if (normalized_str == "360p") return Resolution::P360;
            else if (normalized_str == "480p") return Resolution::P480;
            else if (normalized_str == "540p") return Resolution::P540;
            else if (normalized_str == "720p") return Resolution::P720;
            else if (normalized_str == "1080p") return Resolution::P1080;
            else if (normalized_str == "1440p") return Resolution::P1440;
            else if (normalized_str == "2160p") return Resolution::P2160;

            // Простая обработка форматов вида "1280x720" или "1280X720" без регулярных выражений
            size_t x_pos = normalized_str.find('x');
            if (x_pos != std::string::npos) {
                std::string width = normalized_str.substr(0, x_pos);
                std::string height = normalized_str.substr(x_pos + 1);

                // Проверка всех возможных разрешений
                if (width == "256" && height == "144") {
                    return Resolution::P144;
                } else if (width == "426" && height == "240") {
                    return Resolution::P240;
                } else if (width == "640" && height == "360") {
                    return Resolution::P360;
                } else if (width == "854" && height == "480") {
                    return Resolution::P480;
                } else if (width == "960" && height == "540") {
                    return Resolution::P540;
                } else if (width == "1280" && height == "720") {
                    return Resolution::P720;
                } else if (width == "1920" && height == "1080") {
                    return Resolution::P1080;
                } else if (width == "2560" && height == "1440") {
                    return Resolution::P1440;
                } else if (width == "3840" && height == "2160") {
                    return Resolution::P2160;
                } else if (width == "7680" && height == "4320") {
                    return Resolution::Max; // Можно использовать Max как 8K разрешение, если хотите.
                }
                // Можете добавить другие разрешения по необходимости
            }

            // Если ничего не подходит, выбрасываем исключение
            throw std::invalid_argument("Unknown resolution string: " + str);
        }

        // Метод для получения минимального значения разрешения
        static Resolution minResolution() {
            return Resolution::Min;
        }

        // Метод для получения максимального значения разрешения
        static Resolution maxResolution() {
            return Resolution::Max;
        }

        // Метод для проверки валидности значения enum
        static bool isValidResolution(Resolution res) {
            switch (res) {
                case Resolution::Max:
                case Resolution::Min:
                case Resolution::P144:
                case Resolution::P240:
                case Resolution::P360:
                case Resolution::P480:
                case Resolution::P540:
                case Resolution::P720:
                case Resolution::P1080:
                case Resolution::P1440:
                case Resolution::P2160:
                    return true;
                default:
                    return false;
            }
        }
    };
}
#endif //RESOLUTIONWRAPPER_H
