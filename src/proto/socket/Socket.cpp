#ifndef _SOCKET_MANAGER_HPP_
#define _SOCKET_MANAGER_HPP_

#include "Socket.hpp"
#include "SocketManager.hpp"
#include <KastaarModem.hpp>
#include <cstring>
#include <esp_log.h>
#include <string>

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

    uint32_t Socket::available()
    {
       return getInfo().available;
    }

    esp_modem::command_result Socket::enableRingSize(){
        return KastaarModem::commandCommon(
            "AT+SQNSCFGEXT=" + std::to_string(socketId) + ",1,0,0,0,0,0,0",500);
    }

    esp_modem::command_result Socket::config(const uint16_t packetSize,
                                             const uint16_t exchangeTimeout,
                                             const uint16_t connectionTimeout,
                                             const uint16_t transmissionDelay) {
#pragma region INPUT_CHECKING
        if (packetSize > 1500) {
            ESP_LOGD("Socket", "packet size was to large: (%hu)", packetSize);
            return esp_modem::command_result::FAIL;
        }

        if ((connectionTimeout < 10 || connectionTimeout > 1200) &&
            connectionTimeout != 0) {
            ESP_LOGD("Socket",
                    "exchange timout was out of bounds, expected 10-1200 or 0");
            return esp_modem::command_result::FAIL;
        }
        if (transmissionDelay > 255) {
            ESP_LOGD("Socket", "connection timeout was larger then 255");
            return esp_modem::command_result::FAIL;
        }
        #pragma endregion
        
        std::string out;
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
    esp_modem::command_result Socket::dial(const std::string& addr, const uint16_t port, const protocol proto, const acceptRemote accept)
    {
#pragma region INPUT_CHECKING
        if(addr.empty()) {
            ESP_LOGD("Socket", "address was empty");
            return esp_modem::command_result::FAIL;
        }

        if(proto > protocol::UDP) {
            ESP_LOGD("Socket","protocol was out of bounds");
            return esp_modem::command_result::FAIL;
        }

        if (accept > acceptRemote::RX_TX && proto == protocol::UDP) {
            ESP_LOGD("Socket", "acceptRemote was out of bounds");
            return esp_modem::command_result::FAIL;
        }
#pragma endregion

        if (enableRingSize() != esp_modem::command_result::OK){
            return esp_modem::command_result::FAIL;
        }
        
        return KastaarModem::commandCommon(
            "AT+SQNSD=" + std::to_string(socketId) + "," +
                std::to_string(proto) + "," + std::to_string(port) + ",\"" +
                addr + "\",,,1,,0",
            5000);
    }
    SocketInfo Socket::getInfo()
    {
        std::string command = "AT+SQNSI=" + std::to_string(socketId) + ",0" ;
        SocketInfo info;
        auto callback = [&](uint8_t* raw, size_t len) -> esp_modem::command_result {
            if (raw == nullptr || len == 0)
                return esp_modem::command_result::TIMEOUT;

            std::string_view line(reinterpret_cast<const char*>(raw), len);

            size_t pos = line.find("+SQNSI: ");
            if (pos != std::string_view::npos) {
                // Extract the rest of the line from "+SQNSI: " to end or next \r\n
                size_t end = line.find("\r\n", pos);
                std::string line_str;
                if (end != std::string_view::npos) {
                    line_str = std::string(line.substr(pos, end - pos));
                } else {
                    line_str = std::string(line.substr(pos)); // fallback if no \r\n
                }
                ESP_LOGI("Socket","%s",line_str.c_str());
                sscanf(line_str.c_str(),
                       "+SQNSI: %" SCNu32 ",%" SCNu32 ",%" SCNu32 ",%" SCNu32,
                       &info.socketId, &info.sent, &info.received,
                       &info.available);

               ESP_LOGI("Socket", "socketId: %" PRIu32 ", sent: %" PRIu32 ", received: %" PRIu32 ", available: %" PRIu32,
         info.socketId, info.sent, info.received, info.available);

            }
            return esp_modem::command_result::OK;
        };

        KastaarModem::commandCallback(command, callback, 5000);
        return info;
    }
    esp_modem::command_result Socket::sendMinimal(const std::string &payload, const std::string &ipAddr, const uint16_t port, const releaseAssistanceInformation RAI)
    {
        if(payload.size() > 1500){
            ESP_LOGD("Socket","payload was to large");
            return esp_modem::command_result::FAIL;
        }
        const auto pass = std::list<std::string_view>({"> "});
        const auto fail = std::list<std::string_view>({"ERROR", "NO CARRIER", "+CME ERROR"});
        if (RAI == SINGLE_DOWNLINK_ONLY) 
        {
            if(KastaarModem::command("AT+SQNSSEND="
                + std::to_string(socketId)
                + ",\""
                + ipAddr
                + "\","
                + std::to_string(port)
                + ","
                + std::to_string(RAI),
                pass,
                fail,
                20000
            ) != esp_modem::command_result::OK){
                return esp_modem::command_result::FAIL;
            }
        } else if(KastaarModem::command("AT+SQNSSEND=" 
            + std::to_string(socketId),
            pass,
            fail,
            20000
        ) != esp_modem::command_result::OK) {
            return esp_modem::command_result::FAIL;
        }
        std::string terminator = "";
        terminator.push_back((char)0x1a);
        return KastaarModem::commandPayload(
            payload,
            terminator,
            20000
        );
}

    esp_modem::command_result Socket::sendMinimal(const uint8_t *data, uint8_t len, const std::string &ipAddr, const uint16_t port, const releaseAssistanceInformation RAI)
    {
        std::string payload(reinterpret_cast<const char*>(data), len);
        return sendMinimal(payload,ipAddr,port,RAI);
    }

    esp_modem::command_result Socket::receiveMinimal(std::span<uint8_t>& data, uint32_t & received)
    {
        return receiveMinimal(data,1500,received);
    }

    esp_modem::command_result Socket::receiveMinimal(uint8_t * data, size_t size, uint32_t & received)
    {
        return receiveMinimal(data,size,1500,received);
    }

    esp_modem::command_result Socket::receiveMinimal(std::span<uint8_t>& data, uint16_t maxBytes, uint32_t & received)
    {
        ESP_LOGD("Socket", "Max bytes allowed to receive: %u", static_cast<unsigned int>(maxBytes));

        std::string command = "AT+SQNSRECV=" + std::to_string(socketId) + "," +
                                std::to_string(maxBytes);
        received = 0;
        bool receiving = false;
        uint32_t expectedLength = 0;
        ESP_LOGI("Socket", "Available: %" PRIu32, available());

        size_t headerStart = 0;
        size_t headerEnd = 0;
        size_t payloadOffset = 0; // Start of binary payload
        auto callback = [&](uint8_t* raw, size_t len) -> esp_modem::command_result {
            if (raw == nullptr || len == 0)
                return esp_modem::command_result::TIMEOUT;

            std::string_view line(reinterpret_cast<const char*>(raw), len);
            
            if(line.starts_with("\r\n+CME ERROR") || line.starts_with("\r\nERROR"))
                return esp_modem::command_result::FAIL;

            ESP_LOGI("Socket", "line mc line face: %.*s", line.length(), line.data());
            headerStart = line.find("+SQNSRECV:");
            /* TIMEOUT RETURNED DENOTES: no full payload*/
            if (receiving == false && headerStart != std::string_view::npos) {
                unsigned int id;
                unsigned int bytes;
              // Note: line must be null-terminated for sscanf
                
                
                headerEnd = line.find("\r\n", headerStart); // Find end of header
                headerStart--;
                ESP_LOGI("Socket", "headerStart: %" PRIu32 ", headerEnd: %" PRIu32,
                    static_cast<uint32_t>(headerStart),
                    static_cast<uint32_t>(headerEnd));
                payloadOffset = headerEnd + headerStart;
                std::string line_str(line.substr(headerStart, headerEnd + 2));
                ESP_LOGI("Socket", "line_str: %s", line_str.c_str());

                if (sscanf(line_str.c_str(), "\r\n+SQNSRECV: %u,%u", &id,&bytes) == 2 ) {
                    receiving = true;
                    received = bytes;
                    ESP_LOGI("Socket", "receiving: %u", bytes);
                } else {
                    receiving = false;
                    return esp_modem::command_result::TIMEOUT;
                }

                if (headerEnd != std::string_view::npos) {
                    size_t headerLength = headerEnd + 2; // include \r\n
                    size_t totalLength = headerLength + received + 8; // +6 for "\r\nOK\r\n"
                    ESP_LOGI("Soket","Hello");
                    expectedLength = totalLength;
                } else {
                    receiving = false;
                    return esp_modem::command_result::TIMEOUT;
                }
            }
            ESP_LOGI("Socket",
                     "len (%" PRIu32 ") >= expectedLength (%" PRIu32 "): %s",
                     (uint32_t)len, (uint32_t)expectedLength,
                     (len >= expectedLength) ? "true" : "false");

            if(receiving && len >= expectedLength) {
                std::memcpy(data.data(), raw + payloadOffset, received);
                receiving = false;
                ESP_LOGI("Socket", "finished receiving");
                //KastaarModem::enableURCHandler(true);
                return esp_modem::command_result::OK;
            } 


            return esp_modem::command_result::TIMEOUT;
        };

        //KastaarModem::enableURCHandler(false);
        return KastaarModem::commandCallback(command,callback,5000);
    }
    esp_modem::command_result Socket::receiveMinimal(uint8_t* data, size_t size, uint16_t maxBytes, uint32_t & received)
    {
        /* Don't worry about the error here, it is a false positive */
        std::span<uint8_t> buffer {data,size};

        return receiveMinimal(buffer,maxBytes,received);
    }

    esp_modem::command_result Socket::receive(std::span<uint8_t> &data, uint32_t &received)
    {
        return receive(data,data.size(),received);
    }

    esp_modem::command_result Socket::receive(std::span<uint8_t> &data, uint32_t maxBytes, uint32_t &received)
    {
        return receive(data.data(),data.size(), maxBytes,received);
    }

    esp_modem::command_result Socket::receive(uint8_t *data, size_t size, uint32_t &received)
    {
        return receive(data,size,size,received);
    }

    esp_modem::command_result Socket::receive(uint8_t *data, size_t size, uint32_t maxBytes, uint32_t &received)
    {
        size_t bufferOffset = 0;
        uint32_t tempReceived = 0;
        received = 0;
        ESP_LOGI("Socket", "Available: %" PRIu32, available());

        if (available() > size && maxBytes > size) {
            ESP_LOGD("Socket", "Buffer was too small");
            return esp_modem::command_result::FAIL;
        }

        while (available() && received < maxBytes) {
            uint32_t remaining = static_cast<uint16_t>(maxBytes - received);
            uint32_t chunkSize = (remaining < 1500) ? remaining : 1500;

            esp_modem::command_result res = receiveMinimal(data + bufferOffset, size - bufferOffset, chunkSize, tempReceived);
            if (res != esp_modem::command_result::OK)
                return res;

            received += tempReceived;
            bufferOffset += tempReceived;
        }

        return esp_modem::command_result::OK;
    }

}
#endif