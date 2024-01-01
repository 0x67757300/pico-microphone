#include "pico/stdlib.h"
#include "pico/analog_microphone.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"


#define BUFLEN 512


const struct analog_microphone_config config = {
    .gpio = 26,
    .bias_voltage = 1.65,
    .sample_rate = 16000,
    .sample_buffer_size = BUFLEN
};

int16_t sample_buffer[BUFLEN];
volatile int samples_read = 0;


void on_analog_samples_ready() {
    samples_read = analog_microphone_read(sample_buffer, BUFLEN);
}


int main() {
    struct pbuf *p;
    int16_t *payload;
    struct udp_pcb *pcb;
    ip_addr_t server;
    int i;

    if (cyw43_arch_init()) {
        return 1;
    }
    cyw43_arch_enable_sta_mode();
    while (cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
        sleep_ms(1000);
    }

    if (analog_microphone_init(&config) < 0) {
        return 1;
    }
    analog_microphone_set_samples_ready_handler(on_analog_samples_ready);
    if (analog_microphone_start() < 0) {
        return 1;
    }

    pcb = udp_new();
    ipaddr_aton(SERVER, &server);

    while (1) {
        while (samples_read == 0) {
            tight_loop_contents();
        }
        p = pbuf_alloc(PBUF_TRANSPORT, BUFLEN * sizeof (int16_t), PBUF_RAM);
        payload = (int16_t *) p->payload;
        for (i = 0; i < samples_read; i++) {
            payload[i] = sample_buffer[i];
        }
        udp_sendto(pcb, p, &server, PORT);
        pbuf_free(p);
        samples_read = 0;
    }

    return 0;
}
