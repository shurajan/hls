#include "NetworkClientBase.h"
#include <zlib.h>
#include <stdexcept>

namespace network {

    NetworkClientBase::NetworkClientBase() {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    NetworkClientBase::~NetworkClientBase() {
        curl_global_cleanup();
    }

    size_t NetworkClientBase::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    size_t NetworkClientBase::HeaderCallback(void* contents, size_t size, size_t nmemb, void* userp) {
        std::string header((char*)contents, size * nmemb);
        std::string* headers = static_cast<std::string*>(userp);
        headers->append(header);
        return size * nmemb;
    }

    CURL* NetworkClientBase::initCurl() {
        return curl_easy_init();
    }

    std::string NetworkClientBase::decompress_gzip(const std::string& compressed_data) {
        const size_t bufferSize = 32768;
        char buffer[bufferSize];
        std::string decompressed_data;

        z_stream zs;
        zs.zalloc = Z_NULL;
        zs.zfree = Z_NULL;
        zs.opaque = Z_NULL;
        zs.avail_in = static_cast<uInt>(compressed_data.size());
        zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed_data.data()));

        if (inflateInit2(&zs, 16 + MAX_WBITS) != Z_OK) {
            throw std::runtime_error("Failed to initialize zlib for gzip decompression.");
        }

        int ret;
        do {
            zs.avail_out = bufferSize;
            zs.next_out = reinterpret_cast<Bytef*>(buffer);

            ret = inflate(&zs, Z_NO_FLUSH);
            if (ret != Z_OK && ret != Z_STREAM_END) {
                inflateEnd(&zs);
                throw std::runtime_error("Error during gzip decompression.");
            }

            decompressed_data.append(buffer, bufferSize - zs.avail_out);
        } while (ret != Z_STREAM_END);

        inflateEnd(&zs);
        return decompressed_data;
    }

} // namespace network
