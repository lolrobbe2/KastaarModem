#include <sdkconfig.h>
#if CONFIG_KASTAAR_ENABLE_TLS
#ifndef _TLS_PROFILE_HPP_
#define _TLS_PROFILE_HPP_
#define BIT(x) (1U << (x))
#include <cxx_include/esp_modem_types.hpp>

namespace kastaarModem::tls 
{
    class TLSProfile
    {
    public:
        
        enum TLSVersion : uint8_t {
            TLS_10,
            TLS_11,
            TLS_12,
            TLS_13,
            TLS_RESET = 255 // THIS is used to reset the profile.
        };

        enum VertificationBits : uint8_t {
            ALL = 0,
            CERTIFICATE_VALIDATION = BIT(0),
            UNUSED = BIT(1),
            URL_VALIDATION = BIT(2),
            RESERVED_1 = BIT(3),
            RESERVED_2 = BIT(4),
            RESERVED_3 = BIT(5),
            RESERVED_4 = BIT(6),
            RESERVED_5 = BIT(7)
        };
        TLSProfile();
        ~TLSProfile();
        esp_modem::command_result config(TLSVersion version, VertificationBits verificationMask, uint8_t caCertificateId,uint8_t clientCertificateId, uint8_t clientPrivKeyId);

      private:
        uint8_t profileId = 0;
    };
}
#endif
#endif