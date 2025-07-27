//
// Created by alexander on 6/29/25.
//

#include "wifi.h"

//#define MULTICAST_IP "192.168.1.128"
#define MULTICAST_IP "239.255.10.10"
#define IP_PORT 3838

// Global variables
static struct udp_pcb *udp_pcb;
static ip_addr_t dest_ip;

// Function to initialize UDP multicast (simplified - no IGMP library needed)
bool udp_multicast_init() {
    // Create UDP PCB
    udp_pcb = udp_new();
    if (!udp_pcb) {
        printf("Failed to create UDP PCB\n");
        return false;
    }
    ipaddr_aton(MULTICAST_IP, &dest_ip);

    return true;
}

// Function to send DHT11 data over UDP multicast
bool send_dht11_data(float humidity, float temperature) {
    if (!udp_pcb) {
        printf("UDP not initialized\n");
        return false;
    }

    // Create a simple data structure to send
    struct {
        float humidity;
        float temperature;
    } sensor_data = {humidity, temperature};

    // Create pbuf for the data
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, sizeof(sensor_data), PBUF_RAM);
    if (!p) {
        printf("Failed to allocate pbuf\n");
        return false;
    }

    // Copy data to pbuf
    memcpy(p->payload, &sensor_data, sizeof(sensor_data));

    // Send the packet
    err_t err = udp_sendto(udp_pcb, p, &dest_ip, IP_PORT);

    // Free the pbuf
    pbuf_free(p);

    if (err != ERR_OK) {
        printf("Failed to send UDP packet: %d\n", err);
        return false;
    }

    printf("Sent DHT11 data - Humidity: %.2f%%, Temperature: %.2f°C\n",
           humidity, temperature);
    return true;
}


// Callback function for WiFi scan
static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    // Print the result immediately
    printf("SSID: %-32s  RSSI: %4d  Channel: %3d  MAC: %02x:%02x:%02x:%02x:%02x:%02x  Auth: %d\n",
           result->ssid,
           result->rssi,
           result->channel,
           result->bssid[0], result->bssid[1], result->bssid[2],
           result->bssid[3], result->bssid[4], result->bssid[5],
           result->auth_mode);

    return 0;
}

void print_network_status() {
    // Get the current IP configuration
    ip4_addr_t ip_addr;
    ip4_addr_t netmask;
    ip4_addr_t gateway;

    // Get network interface details from cyw43_state
    ip_addr = cyw43_state.netif[CYW43_ITF_STA].ip_addr;
    netmask = cyw43_state.netif[CYW43_ITF_STA].netmask;
    gateway = cyw43_state.netif[CYW43_ITF_STA].gw;

    // Print network status
    printf("\nNetwork Status:\n");
    printf("---------------\n");
    printf("Link Status: %s\n", cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP ? "UP" : "DOWN");
    printf("IP Address: %s\n", ip4addr_ntoa(&ip_addr));
    printf("Subnet Mask: %s\n", ip4addr_ntoa(&netmask));
    printf("Gateway: %s\n", ip4addr_ntoa(&gateway));

    // Get signal strength (RSSI)
    int32_t rssi;
    if (cyw43_wifi_get_rssi(&cyw43_state, &rssi) == 0) {
        printf("Signal Strength (RSSI): %d dBm\n", rssi);
    }
}

bool connect_wifi() {
    printf("Connecting to WiFi: %s\n", WIFI_SSID);
    // Connect to WiFi network
    // Replace WIFI_SSID and WIFI_PASSWORD with your actual WiFi credentials
    int status = cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000);
    if (status != 0) {
        printf("Failed to connect to WiFi. Error code: %d\n", status);

        // Interpret the error code
        switch (status) {
            case CYW43_LINK_DOWN:
                printf("Link is down - no carrier detected\n");
                break;
            case CYW43_LINK_JOIN:
                printf("Attempting to join network\n");
                break;
            case CYW43_LINK_NOIP:
                printf("Connected to network but no IP address\n");
                break;
            case CYW43_LINK_UP:
                printf("Link is up (this shouldn't happen as an error)\n");
                break;
            case CYW43_LINK_FAIL:
                printf("Connection failed - wrong password or authentication method\n");
                break;
            case CYW43_LINK_NONET:
                printf("No matching SSID found\n");
                break;
            default:
                printf("Unknown error\n");
                break;
        }

        return false;

    }

    printf("Connected to WiFi!\n");

    // Wait a moment for DHCP to complete
    sleep_ms(2000);

    // Print network status
    print_network_status();

    return true;
}

bool scan_for_ssids() {
    printf("\nStarting WiFi scan...\n");

    cyw43_wifi_scan_options_t scan_options = {.scan_type=1}; // 0 is active, 1 is passive

    // Wait for scan to complete (maximum 10 seconds)
    absolute_time_t scan_timeout = make_timeout_time_ms(7000);
    while (absolute_time_diff_us(get_absolute_time(), scan_timeout) > 0) {
        // Process any pending events (this calls our callback)
        cyw43_arch_poll();

        // Check if scan is still running
        if (cyw43_wifi_scan_active(&cyw43_state)) {
            sleep_ms(100);
            continue;
            //break;
        } else {
            sleep_ms(1000);  // Don't burn CPU cycles
            printf("\n");
            int scan_code = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
            //printf("WiFi scan code: %d\n", scan_code);
        }
    }

    // Print summary

    // Print a divider
    printf("\n----------------------------------------\n");

    return true;
}
