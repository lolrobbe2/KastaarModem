#include "CoAPProfile.hpp"
#include "CoAPManager.hpp"
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <sdkconfig.h>

#if CONFIG_KASTAAR_ENABLE_COAP
namespace kastaarModem::CoAP
{
    CoAPProfile::CoAPProfile() 
    {
        profileId = CoAPManager::getFreeCoAPProfile(this);

        if (profileId == 255) {
            ESP_LOGD(COAP_PROFILE_TAG, "could not find a free profile");
        }
    }
    
    CoAPProfile::~CoAPProfile()
    {
        CoAPManager::freeProfile(profileId);
    }
    esp_modem::command_result CoAPProfile::config(const std::string &addr, const uint16_t port, const TLSProfile *p_profile)
    {
      return KastaarModem::commandCommon(
          "AT+SQNCOAPCREATE=" + std::to_string(profileId) + ",\"" + addr +
              "\"," + std::to_string(port) + ",," +
              std::to_string(p_profile != nullptr) + ",20,1," +
              std::to_string(p_profile ? p_profile->native() : 0),
          20000);
    }
}

#endif