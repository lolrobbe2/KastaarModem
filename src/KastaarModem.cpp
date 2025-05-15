#include <KastaarModem.hpp>
#include <cxx_include/esp_modem_api.hpp>
#include <esp_event_loop.h>
#include <esp_modem_config.h>
#include <esp_netif.h>
#include <esp_netif_defaults.h>
#include <esp_netif_ppp.h>
#include <nvs_flash.h>

void KastaarModem::urcHandler(std::string_view line) 
{
  // TODO URC handeling.
}

esp_modem::command_result KastaarModem::connect()
{
  getModule()->connect(pdpContext);
}

esp_modem::PdpContext &KastaarModem::getPdpContext() { return pdpContext; }

esp_modem::command_result KastaarModem::urcCallback(uint8_t *data, size_t len) 
{
    char *mutableData = reinterpret_cast<char *>(data);

    std::string_view view(mutableData, len);

    /* We iterate over the view until no more URC's have been found */
    while (!view.empty()) {
        /* Attempt to find the end of the urc*/
        size_t end = view.find("\r\n");

        /* No complete line found, nothing to do!*/
        if (end == std::string_view::npos) {
        break;
        }

        /*
            Modify the original buffer in-place by replacing \r\n with \0.
            This is so we can use C-style strings from the data function in
        string-view
        */
        mutableData[end] = '\0';
        mutableData[end + 1] = '\0';

        std::string_view line = view.substr(0, end);
        view.remove_prefix(end + 2); // Move past the line and \r\n

        if (urcHandler) {
            urcHandler(line);
        }
    }
    return esp_modem::command_result::OK;
}

bool KastaarModem::init(
  const std::string_view apn,
  const HardwareConfig &config) 
{
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }

  esp_err_t netif_ret = esp_netif_init();
  if (netif_ret != ESP_OK)
    return false;

  esp_err_t netif_event_loop_ret = esp_event_loop_create_default();
  if (netif_event_loop_ret != ESP_OK)
    return false;

  pdpContext.apn = apn;
  pdpContext.context_id = 1;

  esp_modem::dce_config dceConfig = ESP_MODEM_DCE_DEFAULT_CONFIG(apn.data());
  if (!uartDTE) {
    esp_modem::dte_config dteConfig = {
        .dte_buffer_size = 512,
        .task_stack_size = 4096,
        .task_priority = 5,
        .uart_config = {
            .port_num = config.uart_no,
            .data_bits = UART_DATA_8_BITS,
            .stop_bits = UART_STOP_BITS_1,
            .parity = UART_PARITY_DISABLE,
            .flow_control = ESP_MODEM_FLOW_CONTROL_HW,
            .source_clk = ESP_MODEM_DEFAULT_UART_CLK,
            .baud_rate = config.baudRate,
            .tx_io_num = config.pinTX,
            .rx_io_num = config.pinRX,
            .rts_io_num = config.pinRTS,
            .cts_io_num = config.pinCTS,
            .rx_buffer_size = 4096,
            .tx_buffer_size = 512,
            .event_queue_size = 30,
        }};

    uartDTE = esp_modem::create_uart_dte(&dteConfig);
    if (!uartDTE)
      return false;

    esp_netif_inherent_config_t *netif_ppp_inherent_config =
        (esp_netif_inherent_config_t *)ESP_NETIF_BASE_DEFAULT_PPP;
    netif_ppp_inherent_config->route_prio = priority;

    esp_netif_config_t netif_ppp_config = {
        .base = netif_ppp_inherent_config,
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP,
    };

    pppInterface = esp_netif_new(&netif_ppp_config);
    if (!pppInterface)
      return false;

    Driver::priority = priority;
  }

  uartDTE->set_urc_cb(callback);

  gm02sDce = esp_modem::create_SQNGM02S_dce(&dceConfig, uartDTE, pppInterface);
}