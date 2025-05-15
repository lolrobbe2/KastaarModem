#include <cxx_include/esp_modem_dce_module.hpp>
#include <cxx_include/esp_modem_types.hpp>
#include <hal/uart_types.h>

/**
 * @brief This is the main interface for KastaarModem
 */
class KastaarModem {
public:
    struct HardwareConfig {
        uart_port_t uart_no;
        uint8_t pinRX;
        uint8_t pinTX;
        uint8_t pinRTS;
        uint8_t pinCTS;
        uint8_t pinReset;
        int baudRate;
    };
    /**
     * @brief This function connects the modem to the LTE/NB-IOT network
     */
    static esp_modem::command_result connect();

    /**
     * @brief returns the modems PDP context
     */
    static esp_modem::PdpContext getPdpContext();
    
    /**
     * @brief This is the URC handler function meant for internal use.
     * 
     * @param [in] line the URC line.
     */
    static void urcHandler(std::string_view line);

    /**
     * @brief This is the urcHandler for ESP_MODEM
     * 
     * This function can have URC's comin togheter see the urcHandler for individual URC's
     * 
     * @param [in] data The incomming data buffer
     * @param [in] len The lenght of the incomming data buffer.
     */
    static esp_modem::command_result urcCallback(uint8_t *data, size_t len);

    /**
     * @brief this function initalizes the esp_modem DCE and DTE.
     * 
     * @param [in] apn The apn to use for PDP connection
     * @param [in] config The configuration for the modem (pins etc).
     */
    static bool init(const std::string_view apn, const HardwareConfig &config);

    /**
     * @brief This functions returns the internal modem module for internal use.
     */
    static esp_modem::SQNGM02S *getModule() {
        return static_cast<esp_modem::SQNGM02S *>(gm02sDce->get_module());
    }

    private:
    inline static esp_modem::PdpContext pdpContext{""};
    inline static esp_modem::esp_netif_t *pppInterface;
    inline static std::shared_ptr<esp_modem::DTE>
        uartDTE; /* DTE can be reused accross configs */
    inline static std::unique_ptr<esp_modem::DCE> gm02sDce;
    inline static esp_netif_inherent_config_t netifPPPInherentConfig;
    inline static esp_modem::got_line_cb callback =
        [](uint8_t *data, size_t len) { return urcCallback(data, len); };
};
