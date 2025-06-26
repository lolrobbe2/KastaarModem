#include "CoAPManager.hpp"
#include <esp_log.h>
#include <sdkconfig.h>

#if CONFIG_KASTAAR_ENABLE_COAP
namespace kastaarModem::CoAP {
uint8_t CoAPManager::getFreeCoAPProfile(CoAPProfile *profile) {
    for (uint8_t i = 0; i < 6; ++i) {
        if (profiles[i] == nullptr) {
        profiles[i] = profile;
        return i;
        }
    }
    return 255;
}
void CoAPManager::freeProfile(uint8_t profileId){
    if (profileId < 0 || profileId > 3) {
        ESP_LOGD(COAP_MANAGER_TAG,"profileId was out of range (0-3)");
        return;
    }

    if (profiles[profileId] != nullptr) {
        delete profiles[profileId];
        profiles[profileId] = nullptr;
    }
}
} // namespace kastaarModem::CoAP

#endif