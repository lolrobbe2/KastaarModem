#include <sdkconfig.h>

#include "cxx17_include/esp_modem_command_library_17.hpp"
#include <KastaarModem.hpp>
#include <cxx_include/esp_modem_api.hpp>
#include <cxx_include/esp_modem_command_library_utils.hpp>
#include <driver/gpio.h>
#include <esp_event.h>
#include <esp_modem_config.h>
#include <esp_netif.h>
#include <esp_netif_defaults.h>
#include <esp_netif_ppp.h>
#include <nvs_flash.h>
#include <proto/socket/SocketManager.hpp>
#include <proto/coap/CoAPManager.hpp>

bool startsWithAny(std::string_view line,
                   const std::initializer_list<std::string_view> &prefixes) {
  for (const auto &prefix : prefixes) {
    if (line.starts_with(prefix)) {
      return true;
    }
  }
  return false;
}

void KastaarModem::urcHandler(std::string_view line) 
{
    /* this function handles the routing of the URC's*/
    ESP_LOGI("KastaarModem", "URC: %.*s", static_cast<int>(line.length()),line.data());
    // Find the first colon
    size_t colonPos = line.find(':');
    if (colonPos == std::string_view::npos) return;

    // Look for the first space after the colon
    size_t spacePos = line.find(' ', colonPos);
    if (spacePos == std::string_view::npos) return;

    // Find the comma after the space
    size_t commaPos = line.find(',', spacePos);
    if (commaPos == std::string_view::npos) return;

    // Extract the substring containing the number
    std::string_view numberStr = line.substr(spacePos + 1, commaPos - spacePos - 1);

    // Convert to integer
    uint8_t profileId = std::profile(std::string(numberStr).c_str());
    if (startsWithAny(line,{"+SQNSRING: "}))
    {
      /* SOCKETS */
      kastaarModem::socket::SocketManager::urcHandler(line, profileId);
    } else if(startsWithAny(line,{"+SQNCOAP"})) {
      /* COAP */
      kastaarModem::CoAP::CoAPManager::urcHandler(line, profileId);
    }
}

esp_modem::command_result KastaarModem::connect()
{
    if(gm02sDce) {
        esp_modem::command_result res = getModule()->connect(pdpContext);
        enableURCHandler(true);
        while (res == esp_modem::command_result::TIMEOUT)
        {
          res = waitForConnection();
        }
    }
    return esp_modem::command_result::FAIL;
}

const esp_modem::PdpContext &KastaarModem::getPdpContext() { return pdpContext; }

esp_modem::command_result KastaarModem::waitForConnection()
{
    const auto pass = std::list<std::string_view>({"+CEREG: 1,1", "+CEREG: 1,5"});
    const auto fail = std::list<std::string_view>({"ERROR","+CEREG: 1,2","+CEREG: 1,4"});
    return KastaarModem::command("AT+CEREG?", pass, fail, 1200000);
}

esp_modem::command_result KastaarModem::urcCallback(uint8_t *data, size_t len) {
  char *mutableData = reinterpret_cast<char *>(data);
  std::string_view view(mutableData, len);
  ESP_LOGI("Socket", "View content: %.*s", view.size(), view.data());
  if (!view.ends_with("\r\n"))
    return esp_modem::command_result::FAIL;
  /* We iterate over the view until no more URC's have been found */
  while (!view.empty()) {

    /* Attempt to find the end of the urc*/
    size_t start = view.find("\r\n");

    if (start == std::string_view::npos) {
      break;
    }

    // Remove everything before and including first \r\n
    view.remove_prefix(start + 2);

    // Step 2: Look for the next \r\n (end of URC)
    size_t end = view.find("\r\n");

    if (end == std::string_view::npos) {
      return esp_modem::command_result::FAIL;
    }

    std::string_view line = view.substr(0, end);
    if (line.starts_with("+")) {
      urcHandler(line);
    }

    view.remove_prefix(end + 2); // Move past the line and \r\n
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

  gpio_set_direction((gpio_num_t)config.pinReset, GPIO_MODE_OUTPUT);
  gpio_set_pull_mode((gpio_num_t)config.pinReset, GPIO_FLOATING);
  gpio_deep_sleep_hold_en();

  gpio_hold_dis((gpio_num_t)config.pinReset);
  gpio_set_level((gpio_num_t)config.pinReset, 0);
  vTaskDelay(pdMS_TO_TICKS(10));
  gpio_set_level((gpio_num_t)config.pinReset, 1);
  gpio_hold_en((gpio_num_t)config.pinReset);
  vTaskDelay(pdMS_TO_TICKS(5000));

  esp_modem::dce_config dceConfig = ESP_MODEM_DCE_DEFAULT_CONFIG(apn.data());
  if (!uartDTE) {

    esp_modem::dte_config dteConfig = {
        .dte_buffer_size = 4096 / 2,
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
            .rx_buffer_size = 2048,
            .tx_buffer_size = 2048,
            .event_queue_size = 30,
        }};

    uartDTE = esp_modem::create_uart_dte(&dteConfig);
    
    if (!uartDTE)
      return false;

    esp_netif_inherent_config_t *netif_ppp_inherent_config =
        (esp_netif_inherent_config_t *)ESP_NETIF_BASE_DEFAULT_PPP;
    netif_ppp_inherent_config->route_prio = 10;

    esp_netif_config_t netif_ppp_config = {
        .base = netif_ppp_inherent_config,
        .driver = NULL,
        .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP,
    };

    pppInterface = esp_netif_new(&netif_ppp_config);
    if (!pppInterface)
      return false;
  }

  gm02sDce = esp_modem::create_SQNGM02S_dce(&dceConfig, uartDTE, pppInterface);

  if(sync(3) != esp_modem::command_result::OK){
    ESP_LOGD("KastaarModem","could not sync the modem");
    return false;
  }
  getModule()->config_mobile_termination_error(2);
  return gm02sDce.get();
}

esp_modem::command_result KastaarModem::sync(uint8_t count)
{
    for (int i = 0; i < count; i++) {
        getModule()->sync();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    return esp_modem::command_result::OK;
}

esp_modem::command_result KastaarModem::reset()
{
    std::string out;
    return getModule()->at_raw("AT^RESET\r", out, "+SYSSTART","ERROR",2000);
}

esp_modem::command_result KastaarModem::commandPayload(const std::string& payload,const std::string& terminator, uint32_t timeout_ms)
{
    const auto pass = std::list<std::string_view>({"OK"});
    const auto fail = std::list<std::string_view>({"ERROR", "NO CARRIER", "+CME ERROR"});
    
    return gm02sDce->command(
        payload + terminator,
        [&](uint8_t *data, size_t len) {
            std::string_view response((char *)data, len);
            if (data == nullptr || len == 0 || response.empty()) {
            return esp_modem::command_result::TIMEOUT;
            }
            ESP_LOGD("KastaarModem", "Response: %.*s\n", (int)response.length(),
                    response.data());
            for (auto &it : pass)
            if (response.find(it) != std::string::npos) {
                return esp_modem::command_result::OK;
            }
            for (auto &it : fail)
            if (response.find(it) != std::string::npos) {
                return esp_modem::command_result::FAIL;
            }
            return esp_modem::command_result::TIMEOUT;
        },
        timeout_ms);
}
esp_modem::command_result
KastaarModem::command(const std::string &command,
                      const std::list<std::string_view> &pass_phrase,
                      const std::list<std::string_view> &fail_phrase, uint32_t timeout_ms) {
  // TODO implement or remove
  return gm02sDce->command(
      command + "\r",
      [&](uint8_t *data, size_t len) {
        std::string_view response((char *)data, len);
        if (data == nullptr || len == 0 || response.empty()) {
          return esp_modem::command_result::TIMEOUT;
        }
        ESP_LOGD("KastaarModem", "Response: %.*s\n", (int)response.length(),
                 response.data());
        for (auto &it : pass_phrase)
          if (response.find(it) != std::string::npos) {
            return esp_modem::command_result::OK;
          }
        for (auto &it : fail_phrase)
          if (response.find(it) != std::string::npos) {
            return esp_modem::command_result::FAIL;
          }
        return esp_modem::command_result::TIMEOUT;
      },
      timeout_ms);
}

esp_modem::command_result KastaarModem::commandCommon(const std::string &command, uint32_t timeout_ms)
{
    std::string out = "";
    esp_modem::command_result res =
        KastaarModem::getModule()->at(command, out, timeout_ms);
    if (res == esp_modem::command_result::FAIL && out.size() > 2) {
        ESP_LOGD("KastaarModem"," and error occured: %s", out.c_str());
        return esp_modem::command_result::FAIL;
    }
    
    return esp_modem::command_result::OK;
}

esp_modem::command_result KastaarModem::at(const std::string &command, uint32_t timeout_ms) 
{
    std::string out;
    return KastaarModem::getModule()->at(command,out,timeout_ms);
}

esp_modem::command_result KastaarModem::commandCallback(const std::string& command, esp_modem::got_line_cb got_line, uint32_t time_ms)
{
  return gm02sDce->command(command + "\r", got_line, time_ms);
}

void KastaarModem::enableURCHandler(bool enabled) 
{
  uartDTE->set_urc_cb(enabled ? callback : nullptr);
}
