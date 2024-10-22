cmake_minimum_required(VERSION 3.15)

# Имя проекта
project(hls)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Включаем поддержку нового механизма поиска Python в pybind11
set(PYBIND11_FINDPYTHON ON)

# Найти Python перед подключением pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development)

# Подключаем pybind11
find_package(pybind11 REQUIRED)

# Указываем пути к исходным файлам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(NETWORK_SRC_DIR "${SRC_DIR}/network")
set(HLS_SRC_DIR "${SRC_DIR}/m3u8")

# Главные исходные файлы, включая файлы для сети
set(MODULE_SOURCES
        ${SRC_DIR}/StreamDownloadManager.cpp
        ${NETWORK_SRC_DIR}/NetworkClient.cpp
        ${HLS_SRC_DIR}/M3U8Parser.cpp
        ${HLS_SRC_DIR}/StreamSegments.cpp
)

# Создаем модуль Python
pybind11_add_module(hls_module
        ${MODULE_SOURCES}
)

# Включаем заголовочные файлы в компиляцию для hls_module
target_include_directories(hls_module PRIVATE ${SRC_DIR} ${NETWORK_SRC_DIR} ${HLS_SRC_DIR})

# Подключаем libcurl, zlib, nlohmann_json
find_package(CURL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(nlohmann_json 3.10.5 REQUIRED)

# Линкуем необходимые библиотеки к модулю hls_module
target_link_libraries(hls_module PRIVATE CURL::libcurl ZLIB::ZLIB nlohmann_json::nlohmann_json)

# Дополнительные настройки для macOS
if(APPLE)
    message(STATUS "Configuring for macOS...")
    target_compile_options(hls_module PRIVATE "-stdlib=libc++")
    target_link_options(hls_module PRIVATE "-stdlib=libc++")
endif()

# Сообщение о завершении настройки
message(STATUS "HLS Python module configured successfully.")