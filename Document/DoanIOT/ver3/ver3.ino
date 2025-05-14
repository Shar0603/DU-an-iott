#define BLYNK_TEMPLATE_ID "TMPL6B9hIXIxS"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "sjeqP6-0ZSpyeEqgRPBdfYYKAojLgaym"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[] = "88888888";
char pass[] = "tamcontam";

#define ONE_WIRE_BUS 32        // Chân kết nối DS18B20 (D32)
#define SENSOR_PIN 34         // Chân analog đọc tín hiệu từ cảm biến mức chất lỏng
#define SAMPLING_INTERVAL 1000 // Lấy mẫu mỗi giây

#define ADC_MIN 0             // Giá trị ADC khi mức chất lỏng thấp nhất
#define ADC_MAX 4095          // Giá trị ADC khi mức chất lỏng cao nhất

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

void sendTemperature() {
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    Blynk.virtualWrite(V0, tempC);  // Gửi dữ liệu nhiệt độ lên Blynk (V0)
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
}

void sendLiquidLevel() {
    int rawValue = analogRead(SENSOR_PIN);
    float percentage = ((rawValue - ADC_MIN) * 100.0) / (ADC_MAX - ADC_MIN);
    percentage = constrain(percentage, 0, 100);

    Blynk.virtualWrite(V2, percentage);  // Gửi dữ liệu mức chất lỏng lên Blynk (V2)
    Serial.print("Raw ADC: ");
    Serial.print(rawValue);
    Serial.print(" | Liquid Level: ");
    Serial.print(percentage, 1);
    Serial.println("%");
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    sensors.begin();
    timer.setInterval(1000L, sendTemperature);     // Gửi nhiệt độ mỗi giây
    timer.setInterval(SAMPLING_INTERVAL, sendLiquidLevel); // Gửi mức chất lỏng mỗi giây
}

void loop() {
    Blynk.run();
    timer.run();
}
