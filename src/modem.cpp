#include "modem.hpp"
#include <KastaarModem.hpp>
esp_modem::command_result modem::connect() 
{
    KastaarModem::getModule()->connect(kastaarModem::getPdpContext());
}