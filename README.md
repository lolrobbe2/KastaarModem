# KastaarModem

This library is meant as an alternative to WalterModem wich uses ESP-MODEM internally.


there is a change required for the modem component in esp_modem_dte.cpp:
see [here](https://github.com/espressif/esp-protocols/pull/810) for more info.

```cpp
if (urc_handler) {
    if (urc_handler(data, consumed + len) == command_result::OK) {
        return true;
    }
}
```