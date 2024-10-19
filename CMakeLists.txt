cmake_minimum_required(VERSION 3.22.1)

# Имя проекта
project(hls)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем пути к исходным файлам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(NETWORK_SRC_DIR "${SRC_DIR}/network")
set(HLS_SRC_DIR "${SRC_DIR}/m3u8")

# Главные исходные файлы
set(MAIN_SOURCES
        ${SRC_DIR}/main.cpp
        ${HLS_SRC_DIR}/M3U8Parser.cpp
        ${HLS_SRC_DIR}/StreamSegments.cpp
        ${HLS_SRC_DIR}/LiveStreamSegments.cpp
        ${HLS_SRC_DIR}/FixedStreamSegments.cpp
        ${HLS_SRC_DIR}/StreamDownloader.h
        src/m3u8/ResolutionWrapper.h
)

# Исходники библиотеки NetworkClientLib
set(NETWORK_SOURCES
        ${NETWORK_SRC_DIR}/NetworkClient.cpp
        ${NETWORK_SRC_DIR}/NetworkClientSocks5.cpp
        ${NETWORK_SRC_DIR}/NetworkClientBase.cpp
)

# Создаем статическую библиотеку NetworkClientLib
add_library(NetworkClientLib STATIC
        ${NETWORK_SOURCES}
)

# Добавляем исполняемый файл hls
add_executable(hls
        ${MAIN_SOURCES}
)

# Включаем заголовочные файлы в компиляцию для hls
target_include_directories(hls PRIVATE ${SRC_DIR} ${NETWORK_SRC_DIR} ${HLS_SRC_DIR})

# Включаем заголовочные файлы в компиляцию для библиотеки NetworkClientLib
target_include_directories(NetworkClientLib PUBLIC ${SRC_DIR} ${NETWORK_SRC_DIR})

# Подключаем libcurl, zlib, nlohmann_json
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(nlohmann_json REQUIRED)

# Линкуем NetworkClientLib, libcurl и zlib к исполняемому файлу hls
target_link_libraries(hls PRIVATE NetworkClientLib CURL::libcurl ZLIB::ZLIB nlohmann_json::nlohmann_json)

# Сообщение о завершении настройки
message(STATUS "HLS project configured successfully.")
