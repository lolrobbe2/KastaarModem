#ifndef _SOCKET_MANAGER_HPP_
#define _SOCKET_MANAGER_HPP_

#include <KastaarModem.hpp>
#include <esp_log.h>
#include "SocketManager.hpp"
#include "Socket.hpp"

namespace kastaarModem::socket
{
    Socket::Socket()
    {
        socketId = SocketManager::getFreeSocket(this);
        if(socketId == 0){
            ESP_LOGD("Socket","could find a free socket");
        }
    }
    
    Socket::~Socket()
    {
        SocketManager::freeSocket(socketId);
    }
    constexpr uint8_t Socket::getSocketId()
    {
        return socketId;
    }

    esp_modem::command_result Socket::config(const uint16_t packetSize, const uint16_t exchangeTimeout, const uint16_t connectionTimeout, const uint16_t transmissionDelay)
    {
        #pragma region INPUT_CHECKING
        if(packetSize > 1500) 
        {
            ESP_LOGD("Socket","packet size was to large: (%hu)", packetSize);
            return esp_modem::command_result::FAIL;
        }

        if((connectionTimeout < 10 || connectionTimeout > 1200) && connectionTimeout != 0)
        {
            ESP_LOGD("Socket", "exchange timout was out of bounds, expected 10-1200 or 0");
            return esp_modem::command_result::FAIL;
        }
        if (transmissionDelay > 255) {
            ESP_LOGD("Socket",
                    "connection timeout was larger then 255");
            return esp_modem::command_result::FAIL;
        }
        #pragma endregion
        
        std::string out;
        ESP_LOGD("Socket","Config");
        return KastaarModem::commandCommon(
            "AT+SQNSCFG=" 
            + std::to_string(socketId) 
            + ",1," 
            + std::to_string(packetSize) 
            + "," 
            + std::to_string(exchangeTimeout) 
            + "," 
            + std::to_string(connectionTimeout) 
            + ","
            + std::to_string(transmissionDelay)
            ,
            500);
    }
}
#endif