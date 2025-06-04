#include "TLSProfile.hpp"
#include "TLSManager.hpp"
#include <esp_log.h>
namespace kastaarModem::tls
{
    TLSProfile::TLSProfile() 
    {
        profileId = TLSManager::getFreeProfile();
        if(profileId == 0){
            ESP_LOGD("Socket","could find a free socket");
        }
    }
}