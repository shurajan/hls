cmake_minimum_required(VERSION 3.15)

# Имя проекта
project(hls)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем пути к исходным файлам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(NETWORK_SRC_DIR "${SRC_DIR}/network")
set(HLS_SRC_DIR "${SRC_DIR}/m3u8")

# Главные исходные файлы, включая файлы для сети
set(SOURCES
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/AppConfig.cpp
        ${SRC_DIR}/AppConfig.h
        ${SRC_DIR}/App.cpp
        ${SRC_DIR}/App.h
        ${SRC_DIR}/RoomInfo.h
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
find_package(fmt REQUIRED)

# Настройки для nlohmann-json в зависимости от операционной системы
if(APPLE)
    # macOS: используем стандартный способ через find_package
    find_package(nlohmann_json 3.11.3 REQUIRED)
    target_link_libraries(hls PRIVATE nlohmann_json::nlohmann_json)
    message(STATUS "Using system-installed nlohmann-json on macOS.")
else()
    # Linux: используем FetchContent для загрузки библиотеки
    include(FetchContent)
    FetchContent_Declare(
            nlohmann_json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG        v3.11.3 # Указываем нужную версию
    )
    FetchContent_MakeAvailable(nlohmann_json)
    target_link_libraries(hls PRIVATE nlohmann_json::nlohmann_json)
    message(STATUS "Using FetchContent to load nlohmann-json on Linux.")
endif()

# Линкуем необходимые библиотеки к исполняемому файлу
target_link_libraries(hls PRIVATE
        CURL::libcurl
        ZLIB::ZLIB
        fmt::fmt
)

# Дополнительные настройки для macOS
if(APPLE)
    message(STATUS "Configuring for macOS...")
    target_compile_options(hls PRIVATE "-stdlib=libc++")
    target_link_options(hls PRIVATE "-stdlib=libc++")
endif()

# Сообщение о завершении настройки
message(STATUS "HLS project configured successfully.")
