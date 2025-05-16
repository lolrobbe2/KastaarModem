#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <cxx_include/esp_modem_types.hpp>
namespace kastaarModem::socket {
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

    /**
     * @brief this class represents a modem socket
     */
    class Socket {
    public:
    Socket();

    ~Socket();
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

    #pragma region SEND_MINIMAL
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
    #pragma endregion
    /**
     * @brief this returns the Socket Id (1-5) (0 is not initalized)
     */
    constexpr uint8_t getSocketId();

    private:
        bool configured = false;
        bool connected = true;
        uint8_t socketId = 0;
    };

    template <typename T>
    inline esp_modem::command_result Socket::sendMinimal(const T &data, const std::string &ipAddr, const uint16_t port, const releaseAssistanceInformation RAI)
    {
        return sendMinimal((uint8_t*)&data,sizeof(T), ipAddr, port, RAI);
    }
} // namespace kastaarModem::socket

#endif