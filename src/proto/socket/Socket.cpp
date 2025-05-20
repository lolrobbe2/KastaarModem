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
    esp_modem::command_result Socket::sendMinimal(const std::string& payload, const std::string &ipAddr, const uint16_t port, const releaseAssistanceInformation RAI)
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
        std::string command = "AT+SQNSRECV=" + std::to_string(socketId) + "," +
                                std::to_string(maxBytes);
        received = 0;
        bool receiving = false;
        uint32_t expectedLength = 0;
        
        auto callback = [&](uint8_t* raw, size_t len) -> esp_modem::command_result {
            if (raw == nullptr || len == 0)
                return esp_modem::command_result::TIMEOUT;

            std::string_view line(reinterpret_cast<const char*>(raw), len);
            
            if(line.starts_with("\r\n+CME ERROR") || line.starts_with("\r\nERROR"))
                return esp_modem::command_result::FAIL;

            /* TIMEOUT RETURNED DENOTES: no full payload*/
            if (receiving == false && line.starts_with("\r\n+SQNSRECV: ")) {
                unsigned int id;
                unsigned int bytes;
              // Note: line must be null-terminated for sscanf
                size_t headerEnd = line.find("\r\n", 2); // Find end of header
                std::string line_str(line.substr(2, headerEnd - 2));

                if (sscanf(line_str.c_str(), "+SQNSRECV: %u,%u", &id,&bytes) == 2 ) {
                    receiving = true;
                    received = bytes;
                } else {
                    receiving = false;
                    return esp_modem::command_result::TIMEOUT;
                }

               
                

                if (headerEnd != std::string_view::npos) {
                    size_t headerLength = headerEnd + 2; // include \r\n
                    size_t totalLength = headerLength + received + 6; // +6 for "\r\nOK\r\n"
        
                    expectedLength = totalLength;
                } else {
                    receiving = false;
                    return esp_modem::command_result::TIMEOUT;
                }
            }

            if(receiving && len >= expectedLength) {
                size_t headerEnd = line.find("\r\n", 2); // Skip initial \r\n
                size_t payloadOffset = headerEnd + 2; // Start of binary payload
                size_t payloadLength = received;      // already parsed earlier from URC
            
                
                std::memcpy(data.data(), raw + payloadOffset, payloadLength);
                dataAvailable -= received;
                receiving = false;
                KastaarModem::enableURCHandler(true);
                return esp_modem::command_result::OK;
            } 


            return esp_modem::command_result::TIMEOUT;
        };

        KastaarModem::enableURCHandler(false);
        return KastaarModem::commandCallback(command,callback,5000);
    }
    esp_modem::command_result Socket::receiveMinimal(uint8_t* data, size_t size, uint16_t maxBytes, uint32_t & received)
    {
        /* Don't worry about the error here, it is a false positive */
        std::span<uint8_t> buffer {data,size};

        return receiveMinimal(buffer,maxBytes,received);
    }
}
#endif