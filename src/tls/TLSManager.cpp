#include "TLSManager.hpp"
#include <esp_log.h>
#include <sdkconfig.h>

namespace kastaarModem::tls 
{
    uint8_t TLSManager::getFreeProfile(TLSProfile *profile)
    {
        for (uint8_t i = 0; i < 5; ++i) {
            if (profiles[i] == nullptr) {
                profiles[i] = profile;
                return i + 1;
            }
        }

        // No free slot found
        ESP_LOGD(TLS_TAG,"NO free SSL/TLS profile was found");
        return 0; // or another invalid marker
    }

    void TLSManager::freeProfile(uint8_t profileId)
    {
        if (profileId == 0 || profileId > 6) {
            ESP_LOGD("SocketManager","socketId was out of range (1-6)");
            return;
        }

        uint8_t index = toIndex(profileId);

        if (profiles[index] != nullptr) {
            delete profiles[index];
            profiles[index] = nullptr;
        }
    }

    constexpr uint8_t TLSManager::toIndex(uint8_t profileId) {
        return profileId - 1;
    }
}
