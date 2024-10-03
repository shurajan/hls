cmake_minimum_required(VERSION 3.29)

# Имя проекта
project(hls)

# Указываем стандарт C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Указываем путь к исходным файлам
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")

# Добавляем исходные файлы
add_executable(hls
        ${SRC_DIR}/main.cpp
        ${SRC_DIR}/PlaylistParserSiteCbr.cpp
        ${SRC_DIR}/NetworkClient.cpp
)

# Включаем заголовочные файлы в компиляцию
target_include_directories(hls
        PRIVATE
        ${SRC_DIR}
)

# Подключаем Boost
find_package(Boost 1.82 REQUIRED COMPONENTS system iostreams)
find_package(OpenSSL REQUIRED)

target_link_libraries(hls PRIVATE Boost::system Boost::iostreams OpenSSL::SSL)

# Сообщаем о успешной сборке
message(STATUS "HLS project configured successfully.")

