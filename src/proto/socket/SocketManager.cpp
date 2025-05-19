#include "SocketManager.hpp"
#include <esp_log.h>
#include <charconv>
namespace kastaarModem::socket
{
    void SocketManager::urcHandler(std::string_view line) 
    {
        std::string temp { line }; /* this will be cached */
        uint8_t id = 0;
        if(temp.starts_with("+SQNSRING: ")){
            uint16_t payloadSize = 0;
            if (sscanf(temp.c_str(), "+SQNSRING: %hhu,%hu", &id, &payloadSize) == 2) {
                sockets[id - 1]->dataAvailable += payloadSize;
            }
        }

        sockets[id - 1]->urcCallback(line);
    }
    constexpr uint8_t SocketManager::toIndex(uint8_t socketId) {
        return socketId - 1;
    }
    uint8_t SocketManager::getFreeSocket(Socket *socket) {
        for (uint8_t i = 0; i < 6; ++i) {
            if (sockets[i] == nullptr) {
                sockets[i] = socket;
                return i + 1;
            }
        }

      // No free slot found
      return 0; // or another invalid marker
    }
    void SocketManager::freeSocket(uint8_t socketId)
    {
        if (socketId == 0 || socketId > 6) {
            ESP_LOGD("SocketManager","socketId was out of range (1-6)");
            return;
        }

        uint8_t index = toIndex(socketId);

        if (sockets[index] != nullptr) {
            delete sockets[index];
            sockets[index] = nullptr;
        }
    }
}