#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_SOCKETS
#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <cxx_include/esp_modem_types.hpp>
#include <span>

namespace kastaarModem::socket {
    #pragma region ENUMS
    enum protocol {
        TCP,
        UDP,
    };

    enum acceptRemote {
        DISABLED = 0, /* disabled */
        RX_ONLY = 1,  /* modem accepts data from any host besides ipaddr:port */
        RX_TX = 2 /* modem receives from any host besides ipaddr:port and can send to any besides ipaddr:port */
    };

    enum releaseAssistanceInformation {
        NO_INFORMATION = 0,
        NO_MORE_TRANSMISSIONS = 1,
        SINGLE_DOWNLINK_ONLY = 2
    };
    #pragma endregion
    
    struct SocketInfo
    {
        uint32_t socketId = 0;
        uint32_t sent = 0;
        uint32_t received = 0;
        uint32_t available = 0;
    };
    /**
     * @brief this class represents a modem socket
     */
    class Socket {
    public:
    Socket();

    ~Socket();
    #pragma region PUBLIC_METHODS
    /**
     * @brief this function configures the socket
     */
    esp_modem::command_result config(const uint16_t packetSize = 300,
                                    const uint16_t exchangeTimeout = 90,
                                    const uint16_t connectionTimeout = 600,
                                    const uint16_t transmissionDelay = 50);

    esp_modem::command_result dial(
        const std::string& addr,
        const uint16_t port,
        const protocol proto = protocol::TCP,
        const acceptRemote accept = acceptRemote::DISABLED
    );

    SocketInfo getInfo();
    
    #pragma region SEND_MINIMAL
#if KASTAAR_ENABLE_SEND_MINIMAL
    /**
     * @brief This function will send a minimal amount of bytes over the socket (max 1500)
     * 
     * @param [in] payload The payload to send
     * @param [in] ipAddr The address of the remote host (only enabled when RAI = SINGLE_DOWNLINK_ONLY)
     * @param [in] port The port of the remote host (only enabled when RAI = SINGLE_DOWNLINK_ONLY)
     * @param [in] RAI The release assitance information.
     */
    esp_modem::command_result sendMinimal(const std::string& payload,const std::string& ipAddr = "",const uint16_t port = 0,const releaseAssistanceInformation RAI = NO_INFORMATION);

    /**
     * @brief This function will send a minimal amount of bytes over the socket
     * (max 1500)
     * 
     * @param [in] data The pointer to the data Array.
     * @param [in] ipAddr The address of the remote host (only enabled when RAI = SINGLE_DOWNLINK_ONLY)
     * @param [in] port The port of the remote host (only enabled when RAI = SINGLE_DOWNLINK_ONLY)
     * @param [in] RAI The release assitance information.
     */
    esp_modem::command_result sendMinimal(const uint8_t* data, uint8_t len,const std::string& ipAddr = "",const uint16_t port = 0,const releaseAssistanceInformation RAI = NO_INFORMATION);

    /**
     * @brief This function will send a minimal amount of bytes over the socket
     * (max 1500)
     */
    template<typename T>
    esp_modem::command_result sendMinimal(const T& object, const std::string& ipAddr = "",const uint16_t port = 0,const releaseAssistanceInformation RAI = NO_INFORMATION);
    #endif
    #pragma endregion
#pragma region RECEIVE
    #pragma region MINIMAL
#if CONFIG_KASTAAR_ENABLE_RECEIVE_MINIMAL
    /**
     * @brief This function attempts to receive 1500 bytes and write it to the data span.
     * 
     * @param [in] data the data span to write the data to.
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receiveMinimal(std::span<uint8_t>& data,uint32_t& received);

    /**
     * @brief This function attempts to receive 1500 bytes and write it to the
     * data buffer.
     *
     * @param [in] data pointer to the data buffer to write to.
     * @param [in] size size of the data buffer
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receiveMinimal(uint8_t* data, size_t size,uint32_t& received);

    /**
     * @brief This function attempts to receive maxBytes bytes and write it to the the data span
     */
    esp_modem::command_result receiveMinimal(std::span<uint8_t>& data,uint16_t maxBytes,uint32_t& received);

    /**
     * @brief This function attempts to receive a maximum of 1500 bytes and
     * writes them to the array.
     *
     * @param [in] data The array to copy the data into.
     * @param [out] received the amount of data that was acctualy received.
     */
    template <std::size_t N>
    esp_modem::command_result receiveMinimal(std::array<uint8_t, N>& data, uint32_t & received);

    /**
     * @brief This function attempts to receive a maximum of 1500 bytes and writes them to the data buffer
     * 
     * @param [in] data The pointer to the data buffer
     */
    esp_modem::command_result receiveMinimal(uint8_t *data, size_t size, uint16_t maxBytes,uint32_t &received);
    #endif
#pragma endregion

#pragma region FULL
#if CONFIG_KASTAAR_ENABLE_RECEIVE_FULL
    /**
     * @brief This function attempts to receive all the available bytes and write it to the data span.
     * 
     * @param [in] data the data span to write the data to.
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receive(std::span<uint8_t>& data,uint32_t& received);

       /**
     * @brief This function attempts to receive all the available bytes and write it to the data span.
     * 
     * @param [in] data the data span to write the data to.
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receive(std::span<uint8_t>& data, uint32_t maxBytes, uint32_t& received);

    /**
     * @brief This function attempts to receive all the available bytes
     * and write it to the data buffer.
     *
     * @param [in] data pointer to the data buffer to write to.
     * @param [in] size size of the data buffer
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receive(uint8_t* data, size_t size,uint32_t& received);

    /**
     * @brief This function attempts to receive all the available bytes
     * and write it to the data buffer.
     *
     * @param [in] data pointer to the data buffer to write to.
     * @param [in] size size of the data buffer
     * @param [out] received the amount of data that was acctualy received.
     */
    esp_modem::command_result receive(uint8_t* data, size_t size, uint32_t maxBytes,uint32_t& received);

    /**
     * @brief This function attempts to receive a maximum of 1500 bytes and
     * writes them to the array.
     *
     * @param [in] data The array to copy the data into.
     * @param [out] received the amount of data that was acctualy received.
     */
    template <std::size_t N>
    esp_modem::command_result receive(std::array<uint8_t, N> &data, uint32_t &received);

    /**
     * @brief This function attempts to receive a maximum of 1500 bytes and
     * writes them to the array.
     *
     * @param [in] data The array to copy the data into.
     * @param [out] received the amount of data that was acctualy received.
     */
    template <std::size_t N>
    esp_modem::command_result receive(std::array<uint8_t, N> &data, uint32_t maxBytes, uint32_t &received);
    #endif
#pragma endregion
#pragma endregion
    /**
     * @brief this returns the Socket Id (1-5) (0 is not initalized)
     */
    constexpr uint8_t getSocketId();

    /**
     * @brief This function returns the amount of bytes available
     */
    uint32_t available();

    /**
     * @brief The urc handler for socket events
     */
    void setUrcHandler(std::function<void(std::string_view)> handler) {
        urcCallback = handler;
    }
#pragma endregion
    
    private:
        /** 
         * @brief This function enables the data portion of the ring message
         */
        esp_modem::command_result enableRingSize();
    private:
        bool configured = false;
        bool connected = true;
        uint8_t socketId = 0;

    protected:
        friend class SocketManager;
        std::function<void(std::string_view)> urcCallback;
    };

    #pragma region TEMPLATED_FUNCTIONS
    template <typename T>
    inline esp_modem::command_result Socket::sendMinimal(const T &data, const std::string &ipAddr, const uint16_t port, const releaseAssistanceInformation RAI)
    {
        static_assert(sizeof(T) <= 1500, "Maximum allowed array size is 1500 bytes");

        return sendMinimal((uint8_t*)&data,sizeof(T), ipAddr, port, RAI);
    }

    template <std::size_t N>
    inline esp_modem::command_result Socket::receiveMinimal(std::array<uint8_t, N> & data, uint32_t & received)
    {
        static_assert(N <= 1500, "Maximum allowed array size is 1500 bytes");

        return receiveMinimal(data.data(),data.size(), 1500, received);
    }

    template <std::size_t N>
    inline esp_modem::command_result Socket::receive(std::array<uint8_t, N> &data, uint32_t &received)
    {
        return receive(data.data(), data.size(), received);
    }

    template <std::size_t N>
    inline esp_modem::command_result
    Socket::receive(std::array<uint8_t, N> &data, uint32_t maxBytes, uint32_t &received) {
      return receive(data.data(), data.size(), received);
    }
#pragma endregion
} // namespace kastaarModem::socket

using Socket = kastaarModem::socket::Socket;

#endif
#endif