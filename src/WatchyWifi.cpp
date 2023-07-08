#include <esp_wifi.h>
#include <nvs_flash.h>

#include "WatchyWifi.h"

WiFiExtensionClass WatchyWiFi;

esp_netif_t* get_esp_interface_netif(esp_interface_t interface);
void WifiExtension_WiFiEvent(WiFiEvent_t event);

IPv6Address WiFiExtensionClass::globalIPv6()
{
	esp_ip6_addr_t addr;
    if(WiFiGenericClass::getMode() == WIFI_MODE_NULL){
        return IPv6Address();
    }
    if(esp_netif_get_ip6_global(get_esp_interface_netif(ESP_IF_WIFI_STA), &addr)) {
        return IPv6Address();
    }
    return IPv6Address(addr.addr);
}

// return link local if global not available yet
IPv6Address WiFiExtensionClass::activeIPv6()
{
    IPv6Address address = globalIPv6();
    if(address[0] == 0)
        return localIPv6();
    else
        return address;
}

bool WiFiExtensionClass::hasGlobalIPv6()
{
    return globalIPv6()[0]!=0;
}

// It can take 2-3 seconds for SLAAC to give us an address.. this is longer than I'd expect
// so I suspect there's something wrong in the libraries somewhere.
//
// Because watchy aggressively shuts down wifi between calls, this has to be called each time, which sucks..
//
uint8_t WiFiExtensionClass::waitForIPv6(unsigned timeout) 
{
    if((WiFiGenericClass::getMode() & WIFI_MODE_STA) == 0) {
        return WL_DISCONNECTED;
    }
    unsigned long start = millis();
    while(!hasGlobalIPv6() && (millis() - start) < timeout) {
        delay(100);
    }
    return WL_CONNECTED;
}

void WiFiExtensionClass::Initialise()
{
    WiFi.onEvent(WiFiEvent);
}

void WiFiExtensionClass::WiFiEvent(WiFiEvent_t event)
{
    switch(event) {
        case ARDUINO_EVENT_WIFI_AP_START:
            WiFi.softAPenableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_START:
            break;
        case ARDUINO_EVENT_WIFI_STA_CONNECTED:
            delay(10); // Slight delay before enabling ipv6 otherwise it doesn't work
            WiFi.enableIpV6();
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP6:
            break;
        case ARDUINO_EVENT_WIFI_AP_GOT_IP6:
            break;
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            break;
        default:
            break;
    }
}
