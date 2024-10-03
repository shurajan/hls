#include "NetworkClient.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>             // Подключаем Boost.Beast для работы с SSL
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/ssl/context.hpp>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ssl = asio::ssl;
using tcp = asio::ip::tcp;

// Конструктор
NetworkClient::NetworkClient() {}

// Функция для распаковки gzip-данных с использованием Boost.Iostreams (теперь публичная)
std::string NetworkClient::decompress_gzip(const std::string& compressed_data) {
    std::string decompressed_data;
    boost::iostreams::filtering_ostream out;
    boost::iostreams::gzip_decompressor decompressor;

    out.push(decompressor);
    out.push(boost::iostreams::back_inserter(decompressed_data));

    boost::iostreams::write(out, compressed_data.data(), compressed_data.size());
    boost::iostreams::close(out);

    return decompressed_data;
}

// Шаблонная функция для отправки HTTP запроса и чтения ответа (работает как с SSL, так и без SSL)
template <typename StreamType>
std::string send_http_request(StreamType& stream, const std::string& host, const std::string& target, NetworkClient& client) {
    // Создаем HTTP GET запрос
    http::request<http::string_body> req{http::verb::get, target, 11};
    req.set(http::field::host, host);
    req.set(http::field::accept_encoding, "gzip");

    // Отправляем запрос
    http::write(stream, req);

    // Буфер для хранения данных ответа
    beast::flat_buffer buffer;
    // Объект для хранения ответа
    http::response<http::dynamic_body> res;

    // Читаем ответ
    http::read(stream, buffer, res);

    // Возвращаем строку данных тела ответа
    if (res[http::field::content_encoding] == "gzip") {
        return client.decompress_gzip(beast::buffers_to_string(res.body().data()));
    }
    return beast::buffers_to_string(res.body().data());
}

// Функция для выполнения HTTP(S) запроса с использованием Boost.Beast и Boost.Asio
std::string NetworkClient::fetch(const std::string& url) {
    try {
        asio::io_context ioc;
        ssl::context ssl_ctx{ssl::context::sslv23_client};  // Инициализация SSL контекста

        // Разбираем URL
        std::string protocol, host, target;
        unsigned short port;
        if (url.find("https://") == 0) {
            protocol = "https";
            port = 443;
        } else if (url.find("http://") == 0) {
            protocol = "http";
            port = 80;
        } else {
            throw std::runtime_error("Unsupported URL protocol");
        }

        std::size_t host_start = url.find("://") + 3;
        std::size_t host_end = url.find("/", host_start);
        host = url.substr(host_start, host_end - host_start);
        target = url.substr(host_end);

        // Резолвер для получения конечных точек
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, std::to_string(port));

        if (protocol == "https") {
            // Для HTTPS используем ssl_stream
            beast::ssl_stream<beast::tcp_stream> stream{ioc, ssl_ctx};
            beast::get_lowest_layer(stream).connect(results);
            stream.handshake(ssl::stream_base::client);
            return send_http_request(stream, host, target, *this);  // Передаем клиент для распаковки gzip
        } else {
            // Для HTTP используем обычный stream
            beast::tcp_stream stream(ioc);
            stream.connect(results);
            return send_http_request(stream, host, target, *this);  // Передаем клиент для распаковки gzip
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to fetch data: " + std::string(e.what()));
    }
}
