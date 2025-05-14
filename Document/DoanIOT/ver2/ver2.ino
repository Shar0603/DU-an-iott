#define BLYNK_TEMPLATE_ID "TMPL6B9hIXIxS"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "sjeqP6-0ZSpyeEqgRPBdfYYKAojLgaym"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[] = "88888888";
char pass[] = "tamcontam";

#define ONE_WIRE_BUS 4  // Chân kết nối DS18B20 (D4)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

void sendTemperature() {
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    Blynk.virtualWrite(V0, tempC);  // Gửi dữ liệu lên Blynk (chọn V0 trên app)
    Serial.println(tempC);
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    sensors.begin();
    timer.setInterval(1000L, sendTemperature);
}

void loop() {
    Blynk.run();
    timer.run();
}
