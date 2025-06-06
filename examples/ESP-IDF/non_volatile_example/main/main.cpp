#include <stdio.h>
#include <esp_log.h>
#include <KastaarModem.hpp>
#include <tls/NonVolatile.hpp>
#include <tls/TLSProfile.hpp>
#include <esp_mac.h>

const char* TAG = "Non volatile example";
KastaarModem modem;
NonVolatileMemory memory;
TLSProfile profile;
/**
 * @brief CA root certificate for DTLS (chain with intermediate: bandwidth!)
 */
const char *caCert = "-----BEGIN CERTIFICATE-----\r\n\
MIIBlTCCATqgAwIBAgICEAAwCgYIKoZIzj0EAwMwGjELMAkGA1UEBhMCQkUxCzAJ\r\n\
BgNVBAMMAmNhMB4XDTI0MDMyNDEzMzM1NFoXDTQ0MDQwODEzMzM1NFowJDELMAkG\r\n\
A1UEBhMCQkUxFTATBgNVBAMMDGludGVybWVkaWF0ZTBZMBMGByqGSM49AgEGCCqG\r\n\
SM49AwEHA0IABJGFt28UrHlbPZEjzf4CbkvRaIjxDRGoeHIy5ynfbOHJ5xgBl4XX\r\n\
hp/r8zOBLqSbu6iXGwgjp+wZJe1GCDi6D1KjZjBkMB0GA1UdDgQWBBR/rtuEomoy\r\n\
49ovMAnj5Hpmk2gTGjAfBgNVHSMEGDAWgBR3Vw0Y1sUvMhkX7xySsX55tvsu8TAS\r\n\
BgNVHRMBAf8ECDAGAQH/AgEAMA4GA1UdDwEB/wQEAwIBhjAKBggqhkjOPQQDAwNJ\r\n\
ADBGAiEApN7DmuufC/aqyt6g2Y8qOWg6AXFUyTcub8/Y28XY3KgCIQCs2VUXCPwn\r\n\
k8jR22wsqNvZfbndpHthtnPqI5+yFXrY4A==\r\n\
-----END CERTIFICATE-----\r\n\
-----BEGIN CERTIFICATE-----\r\n\
MIIBmDCCAT+gAwIBAgIUDjfXeosg0fphnshZoXgQez0vO5UwCgYIKoZIzj0EAwMw\r\n\
GjELMAkGA1UEBhMCQkUxCzAJBgNVBAMMAmNhMB4XDTI0MDMyMzE3MzU1MloXDTQ0\r\n\
MDQwNzE3MzU1MlowGjELMAkGA1UEBhMCQkUxCzAJBgNVBAMMAmNhMFkwEwYHKoZI\r\n\
zj0CAQYIKoZIzj0DAQcDQgAEB00rHNthOOYyKj80cd/DHQRBGSbJmIRW7rZBNA6g\r\n\
fbEUrY9NbuhGS6zKo3K59zYc5R1U4oBM3bj6Q7LJfTu7JqNjMGEwHQYDVR0OBBYE\r\n\
FHdXDRjWxS8yGRfvHJKxfnm2+y7xMB8GA1UdIwQYMBaAFHdXDRjWxS8yGRfvHJKx\r\n\
fnm2+y7xMA8GA1UdEwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgGGMAoGCCqGSM49\r\n\
BAMDA0cAMEQCID7AcgACnXWzZDLYEainxVDxEJTUJFBhcItO77gcHPZUAiAu/ZMO\r\n\
VYg4UI2D74WfVxn+NyVd2/aXTvSBp8VgyV3odA==\r\n\
-----END CERTIFICATE-----\r\n";

/**
 * @brief Walter client certificate for DTLS
 */
const char *walterClientCert = "-----BEGIN CERTIFICATE-----\r\n\
MIIBNTCB3AICEAEwCgYIKoZIzj0EAwMwJDELMAkGA1UEBhMCQkUxFTATBgNVBAMM\r\n\
DGludGVybWVkaWF0ZTAeFw0yNDAzMjUxMDU5MzRaFw00NDA0MDkxMDU5MzRaMCkx\r\n\
CzAJBgNVBAYTAkJFMRowGAYDVQQDDBFsaXRlMDAwMS4xMTExMTExMTBZMBMGByqG\r\n\
SM49AgEGCCqGSM49AwEHA0IABPnA7m6yDd0w6iNuKWJ5T3eMB38Upk1yfM+fUUth\r\n\
AY/qh/BM8JYqG0KFpbR0ymNe+KU0m2cUCPR1QIUVvp3sIYYwCgYIKoZIzj0EAwMD\r\n\
SAAwRQIgDkAa7P78ieIamFqj8el2zL0oL/VHBYcTQL9/ZzsJBSkCIQCRFMsbIHc/\r\n\
AiKVsr/pbTYtxbyz0UJKUlVoM2S7CjeAKg==\r\n\
-----END CERTIFICATE-----\r\n";

/**
 * @brief Walter client private key for DTLS
 */
const char *walterClientKey = "-----BEGIN EC PRIVATE KEY-----\r\n\
MHcCAQEEIHsCxTfyp5l7OA0RbKTKkfbTOeZ26WtpfduUvXD6Ly0YoAoGCCqGSM49\r\n\
AwEHoUQDQgAE+cDubrIN3TDqI24pYnlPd4wHfxSmTXJ8z59RS2EBj+qH8Ezwliob\r\n\
QoWltHTKY174pTSbZxQI9HVAhRW+newhhg==\r\n\
-----END EC PRIVATE KEY-----\r\n";

extern "C" void app_main(void)
{
    modem.init("soracom.io",DEFAULT_CONFIG);

    memory.write(caCert,5,NonVolatileMemory::CERTIFICATE);
    memory.write(walterClientCert, 6, NonVolatileMemory::CERTIFICATE);
    memory.write(walterClientKey, 11, NonVolatileMemory::PRIVATE_KEY);

    profile.config(TLSProfile::TLS_12,(TLSProfile::VertificationBits)(TLSProfile::URL_VALIDATION | TLSProfile::CERTIFICATE_VALIDATION),5,6,11);
    
}