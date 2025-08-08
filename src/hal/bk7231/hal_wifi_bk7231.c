#include "../hal_wifi.h"

#define LOG_FEATURE LOG_FEATURE_MAIN
#include "../../new_common.h"
#include "wlan_ui_pub.h"
#include "ethernet_intf.h"
#include "../../new_common.h"
#include "net.h"
#include "../../logging/logging.h"
#include "../../beken378/app/config/param_config.h"
#include "lwip/netdb.h"
#include "../../new_pins.h"
#include "../src/new_cfg.h"

#ifdef PLATFORM_BEKEN_NEW

#define SOFT_AP                     BK_SOFT_AP
#define STATION                     BK_STATION
#define SECURITY_TYPE_NONE          BK_SECURITY_TYPE_NONE
#define SECURITY_TYPE_WEP           BK_SECURITY_TYPE_WEP
#define SECURITY_TYPE_WPA_TKIP      BK_SECURITY_TYPE_WPA_TKIP
#define SECURITY_TYPE_WPA_AES       BK_SECURITY_TYPE_WPA_AES
#define SECURITY_TYPE_WPA2_TKIP     BK_SECURITY_TYPE_WPA2_TKIP
#define SECURITY_TYPE_WPA2_AES      BK_SECURITY_TYPE_WPA2_AES
#define SECURITY_TYPE_WPA2_MIXED    BK_SECURITY_TYPE_WPA2_MIXED
#define SECURITY_TYPE_AUTO          BK_SECURITY_TYPE_AUTO
#define RW_EVT_STA_CONNECT_FAILED   RW_EVT_STA_AUTH_FAILED
#endif

// Diğer kodlarınız aynı kalacak...

int HAL_SetupWiFiOpenAccessPoint(const char* ssid)
{
#define APP_DRONE_DEF_NET_IP        "192.168.4.1"
#define APP_DRONE_DEF_NET_MASK      "255.255.255.0"
#define APP_DRONE_DEF_NET_GW        "192.168.4.1"
#define APP_DRONE_DEF_CHANNEL       1

    general_param_t general;
    ap_param_t ap_info;
    network_InitTypeDef_st wNetConfig;
    unsigned char* mac;

    memset(&general, 0, sizeof(general_param_t));
    memset(&ap_info, 0, sizeof(ap_param_t));
    memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));

    general.role = 1;
    general.dhcp_enable = 1;

    strcpy((char*)wNetConfig.local_ip_addr, APP_DRONE_DEF_NET_IP);
    strcpy((char*)wNetConfig.net_mask, APP_DRONE_DEF_NET_MASK);
    strcpy((char*)wNetConfig.dns_server_ip_addr, APP_DRONE_DEF_NET_GW);

    ADDLOGF_INFO("no flash configuration, use default\r\n");
    mac = (unsigned char*)&ap_info.bssid.array;
    // Access Point MAC adresi alınıyor
    wifi_get_mac_address((char*)mac, CONFIG_ROLE_AP);

    ap_info.chann = APP_DRONE_DEF_CHANNEL;
    ap_info.cipher_suite = 4; // WPA2-PSK CCMP (AES) - Güvenli şifreleme

    // Sabit SSID ve parola ataması (SSID: "OKUL_ZIL", parola: "9988776655")
    const char* ap_ssid = "OKUL_ZIL";
    const char* ap_key = "9988776655";

    memset(ap_info.ssid.array, 0, sizeof(ap_info.ssid.array));
    memcpy(ap_info.ssid.array, ap_ssid, strlen(ap_ssid));

    ap_info.key_len = strlen(ap_key);
    memset(ap_info.key, 0, sizeof(ap_info.key));
    memcpy(ap_info.key, ap_key, ap_info.key_len);

    bk_wlan_ap_set_default_channel(ap_info.chann);

    os_strncpy((char*)wNetConfig.wifi_ssid, (char*)ap_info.ssid.array, sizeof(wNetConfig.wifi_ssid));
    os_strncpy((char*)wNetConfig.wifi_key, (char*)ap_info.key, sizeof(wNetConfig.wifi_key));

    wNetConfig.wifi_mode = SOFT_AP;
    wNetConfig.dhcp_mode = DHCP_SERVER;

    os_strncpy((char*)wNetConfig.gateway_ip_addr, (char*)APP_DRONE_DEF_NET_GW, sizeof(wNetConfig.gateway_ip_addr));
    os_strncpy((char*)wNetConfig.dns_server_ip_addr, (char*)APP_DRONE_DEF_NET_GW, sizeof(wNetConfig.dns_server_ip_addr));

    wNetConfig.wifi_retry_interval = 100;

    ADDLOGF_INFO("set ip info: %s,%s,%s\r\n",
        wNetConfig.local_ip_addr,
        wNetConfig.net_mask,
        wNetConfig.dns_server_ip_addr);

    ADDLOGF_INFO("ssid:%s  key:%s mode:%d\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key, wNetConfig.wifi_mode);

    bk_wlan_start(&wNetConfig);
    g_bOpenAccessPointMode = 1;

    return 0;
}
