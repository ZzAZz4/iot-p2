#include <Arduino.h>
#include <WebServer.h>
#include <esp32cam.h>

#include "altitude.hpp"

constexpr const char WIFI_SSID[] = "ssid";
constexpr const char WIFI_PASS[] = "password";

WebServer server(80);

int elapsed_millis = 0;
int last_update = 0;

void handleCamRequest() {
    auto frame = esp32cam::capture();
    if (frame == nullptr) {
        Serial.println("CAPTURE FAIL");
        server.send(503, "", "");
        return;
    }
    Serial.printf("CAPTURE OK %dx%d %db\n",
                  frame->getWidth(), frame->getHeight(),
                  static_cast<int>(frame->size()));

    server.setContentLength(frame->size());
    server.send(200, "image/jpeg");

    WiFiClient client = server.client();
    frame->writeTo(client);
}

void handleAltitudeRequest() {
    server.send(200, "text/plain", String(AltitudeSensor::ReadAltitude()));
}

void setup() {
    Serial.begin(9600);
    AltitudeSensor::Init();
    {
        using namespace esp32cam;
        Config cfg;
        cfg.setPins(pins::AiThinker);
        cfg.setResolution(esp32cam::Resolution::find(800, 600));
        cfg.setBufferCount(2);
        cfg.setJpeg(80);

        bool ok = Camera.begin(cfg);
        Serial.println(ok ? "CAMERA OK" : "CAMERA FAIL");
    }

    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.print("http://");
    Serial.println(WiFi.localIP());
    Serial.println("  /altitude");
    Serial.println("  /cam.jpg");
    
    server.on("/altitude", handleAltitudeRequest);
    server.on("/cam.jpg", handleCamRequest);
    
    server.begin();
}

void loop() {
    elapsed_millis = millis();
    if (elapsed_millis - last_update > 2000) {
        last_update = elapsed_millis;
        Serial.print("Altitude: ");
        Serial.println(AltitudeSensor::ReadAltitude());
    }

    server.handleClient();
}