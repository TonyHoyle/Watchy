#ifndef Watchy_WiFi_h
#define Watchy_WiFi_h

#include <WiFi.h>

class WiFiExtensionClass : public WiFiClass
{
public:
    void Initialise();
    IPv6Address globalIPv6();
    IPv6Address activeIPv6();
    uint8_t waitForIPv6(unsigned timeout = 5000);
    bool hasGlobalIPv6();

private:
    static void WiFiEvent(WiFiEvent_t event);
};

#define WiFi WatchyWiFi
extern WiFiExtensionClass WatchyWiFi;

#endif
