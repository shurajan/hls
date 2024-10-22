cmake_minimum_required(VERSION 3.15)

# Имя проекта
project(hls)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Устанавливаем путь к TgBot (один из способов)
set(TgBot_DIR "/usr/local/lib/cmake/TgBot")
# или
# set(CMAKE_PREFIX_PATH "/usr/local/lib/cmake/TgBot" ${CMAKE_PREFIX_PATH})

# Указываем пути к исходным файлам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(NETWORK_SRC_DIR "${SRC_DIR}/network")
set(HLS_SRC_DIR "${SRC_DIR}/m3u8")

# Главные исходные файлы, включая файлы для сети
set(SOURCES
        ${SRC_DIR}/main.cpp
        ${NETWORK_SRC_DIR}/NetworkClient.cpp
        ${HLS_SRC_DIR}/M3U8Parser.cpp
        ${HLS_SRC_DIR}/StreamSegments.cpp
)

# Создаем исполняемый файл
add_executable(hls
        ${SOURCES}
)

# Включаем заголовочные файлы в компиляцию для hls
target_include_directories(hls PRIVATE
        ${SRC_DIR}
        ${NETWORK_SRC_DIR}
        ${HLS_SRC_DIR}
)

# Подключаем необходимые библиотеки
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(nlohmann_json 3.10.5 REQUIRED)

# Линкуем необходимые библиотеки к исполняемому файлу
target_link_libraries(hls PRIVATE
        CURL::libcurl
        ZLIB::ZLIB
        nlohmann_json::nlohmann_json
)

# Дополнительные настройки для macOS
if(APPLE)
    message(STATUS "Configuring for macOS...")
    target_compile_options(hls PRIVATE "-stdlib=libc++")
    target_link_options(hls PRIVATE "-stdlib=libc++")
endif()

# Сообщение о завершении настройки
message(STATUS "HLS project configured successfully.")
