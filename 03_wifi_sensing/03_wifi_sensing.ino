/*
 * 03_wifi_sensing — ensaio 1: validar que a captura de CSI dispara
 *
 * Nao processa CSI ainda. So confirma que:
 *  (1) a ESP32 conecta na rede 2.4GHz
 *  (2) o callback de CSI e chamado a cada pacote recebido
 *  (3) RSSI e tamanho do pacote saem coerentes
 *
 * Criterio de sucesso: contador de pacotes CSI sobe de forma continua
 * enquanto a placa esta associada e ha trafego na rede.
 */

#include <WiFi.h>
extern "C" {
  #include "esp_wifi.h"
}

#include "arduino_secrets.h"
const char* SSID = WIFI_SSID;
const char* PASSWORD = WIFI_PASSWORD;

volatile uint32_t csi_pkt_count = 0;
volatile int8_t   last_rssi = 0;
volatile uint16_t last_len = 0;

void csi_callback(void *ctx, wifi_csi_info_t *data) {
  csi_pkt_count++;
  last_rssi = data->rx_ctrl.rssi;
  last_len = data->len;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ;
  }

  Serial.println("Conectando ao Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado. IP: ");
  Serial.println(WiFi.localIP());

  wifi_csi_config_t csi_config = {
    .lltf_en = true,
    .htltf_en = true,
    .stbc_htltf2_en = true,
    .ltf_merge_en = true,
    .channel_filter_en = true,
    .manu_scale = false,
    .shift = 0,
  };

  esp_err_t err;

  err = esp_wifi_set_csi_config(&csi_config);
  Serial.print("esp_wifi_set_csi_config: ");
  Serial.println(esp_err_to_name(err));

  err = esp_wifi_set_csi_rx_cb(csi_callback, NULL);
  Serial.print("esp_wifi_set_csi_rx_cb: ");
  Serial.println(esp_err_to_name(err));

  err = esp_wifi_set_csi(true);
  Serial.print("esp_wifi_set_csi(true): ");
  Serial.println(esp_err_to_name(err));

  Serial.println("READY");
}

void loop() {
  static uint32_t last_count = 0;
  delay(2000);

  uint32_t count_now = csi_pkt_count;
  uint32_t delta = count_now - last_count;
  last_count = count_now;

  Serial.print("CSI pkts: total=");
  Serial.print(count_now);
  Serial.print(" (+");
  Serial.print(delta);
  Serial.print("/2s) rssi=");
  Serial.print(last_rssi);
  Serial.print(" len=");
  Serial.println(last_len);
}
