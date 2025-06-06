#include "TLSProfile.hpp"
#include "TLSManager.hpp"
#include <KastaarModem.hpp>
#include <esp_log.h>
#include <string>

namespace kastaarModem::tls
{
    TLSProfile::TLSProfile() 
    {
        profileId = TLSManager::getFreeProfile(this);
        if(profileId == 0){
            ESP_LOGD("Socket","could find a free socket");
        }
    }

    TLSProfile::~TLSProfile() 
    {
        TLSManager::freeProfile(profileId);
    }
    esp_modem::command_result TLSProfile::config(
        TLSVersion version, VertificationBits verificationMask,
        uint8_t caCertificateId, uint8_t clientCertificateId,
        uint8_t clientPrivKeyId) 
    {
        std::string command = "AT+SQNSPCFG=" 
            + std::to_string(profileId) 
            + "," + std::to_string(version) + ",\"\","
            + std::to_string(verificationMask)
            + ",";

        if (caCertificateId != 0xff) 
            command += std::to_string(caCertificateId);
        command += ",";
        
        if(clientCertificateId != 0xff)
            command += std::to_string(clientCertificateId);
        command += ",";

        if(clientPrivKeyId != 0xff)
            command += std::to_string(clientPrivKeyId);
        
        command += ",\"\",\"\",0,0,0";
        return KastaarModem::commandCommon(
            command
        ,1200);
    }
}