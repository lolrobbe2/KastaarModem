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
    static esp_modem::PdpContext getPdpContext();
    static void urcHandler(std::string_view line);
    static esp_modem::command_result urcCallback(uint8_t *data, size_t len);
    static bool init(const std::string_view apn, const HardwareConfig &config);
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
