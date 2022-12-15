#include <Arduino.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>

#include "Sensor.hpp"
#include "Update.h"  // Somehow fixes failure to find "Update.h" in AsyncElegantOTA
#include "credentials.h"
#include "index_html.h"

const char *TAG = "main";

const char *WIFI_AP_SSID = "imager";
const char *WIFI_AP_PASSWORD = "slug stupor retool";

AsyncWebServer server(80);

struct {
  uint8_t target_status[64];
  int16_t distance[64];
} sensor_data;

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  WiFi.mode(WIFI_STA);
  ESP_LOGI(TAG, "Trying to connect to %s...", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    ESP_LOGW(TAG, "WiFi connection failed!");
    ESP_LOGI(TAG, "Starting AP %s", WIFI_AP_SSID);
    WiFi.softAP(WIFI_AP_SSID, WIFI_AP_PASSWORD);
  }

  MDNS.begin("imager");

  ESP_LOGI(TAG, "IP: %s", WiFi.localIP().toString());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/sensor", HTTP_GET, [](AsyncWebServerRequest *request) {
    char buf[1024];
    uint16_t len = 0;

    buf[len++] = '[';

    for (uint8_t i = 0; i < 64; i++) {
      len += snprintf(buf + len, sizeof(buf) - len, "[%d,%d],", sensor_data.target_status[i], sensor_data.distance[i]);
    }

    // Replace last , with ]
    buf[len - 1] = ']';

    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", buf);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
  });

  server.onNotFound(notFound);

  AsyncElegantOTA.begin(&server);
  server.begin();

  Wire.begin(19, 18, 400000);

  Sensor::initialize();
}

void loop() {
  Sensor::read(sensor_data.target_status, sensor_data.distance);
}
