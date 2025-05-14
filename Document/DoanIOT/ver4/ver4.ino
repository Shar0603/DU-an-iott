#define BLYNK_TEMPLATE_ID "TMPL6gw-MsB4g"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "rmVngQfsyQAL1amLF4zKjPWjAbCWDMEH"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[] = "88888888";
char pass[] = "tamcontam";

#define ONE_WIRE_BUS 32         // Chân kết nối DS18B20
#define SENSOR_PIN 34          // Chân analog đọc tín hiệu từ cảm biến mức chất lỏng
#define CONTROL_PIN 26         // Chân GPIO điều khiển thiết bị
#define SAMPLING_INTERVAL 1000 // Lấy mẫu mỗi giây

#define MOISTURE_LOW 80        // Ngưỡng độ ẩm thấp
#define MOISTURE_HIGH 100      // Ngưỡng độ ẩm cao 

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
    float percentage = map(rawValue, 0, 4095, 0, 100);
    percentage = constrain(percentage, 0, 100);

    Blynk.virtualWrite(V2, percentage);  // Gửi dữ liệu mức chất lỏng lên Blynk (V2)
    Serial.print(" | Liquid Level: ");
    Serial.print(percentage, 1);
    Serial.println("%");

    if (percentage < MOISTURE_LOW) {
        digitalWrite(CONTROL_PIN, HIGH); // Bật thiết bị khi độ ẩm 
        Serial.println("Thiết bị đã được BẬT do độ ẩm thấp.");
    } else if (percentage >= MOISTURE_HIGH) {
        digitalWrite(CONTROL_PIN, LOW); // 
        Serial.println("Thiết bị đã được TẮT do độ ẩm cao.");
    }
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    sensors.begin();
    pinMode(CONTROL_PIN, OUTPUT);
    digitalWrite(CONTROL_PIN, LOW); // Khởi tạo trạng thái tắt
    timer.setInterval(1000L, sendTemperature);     // Gửi nhiệt độ mỗi giây
    timer.setInterval(SAMPLING_INTERVAL, sendLiquidLevel); // Gửi mức chất lỏng mỗi giây
}

void loop() {
    Blynk.run();
    timer.run();
}
