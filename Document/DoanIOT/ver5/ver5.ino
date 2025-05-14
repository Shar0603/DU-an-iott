#define BLYNK_TEMPLATE_ID "TMPL6gw-MsB4g"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "rmVngQfsyQAL1amLF4zKjPWjAbCWDMEH"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>

char ssid[] = "iPhone của Dương";
char pass[] = "11111111";

#define ONE_WIRE_BUS 5         // Chân kết nối DS18B20
#define SENSOR_PIN 27           // Chân kêt nối cảm biến mực nước
#define PH_PIN 14               // Chân cảm biến pH

#define SAMPLING_INTERVAL 1000  // Lấy mẫu mỗi giây
#define OFFSET 5             // Điều chỉnh giá trị pH nếu cần

#define MOISTURE_LOW 80         
#define MOISTURE_HIGH 100       

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
BlynkTimer timer;

void sendTemperature() {
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    Blynk.virtualWrite(V0, tempC);  
    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.println("°C");
}

void sendLiquidLevel() {
    int rawValue = analogRead(SENSOR_PIN);
    float percentage = map(rawValue, 0, 4095, 0, 100);
    percentage = constrain(percentage, 0, 100);

    Blynk.virtualWrite(V2, percentage);
    Serial.print(" | Liquid Level: ");
    Serial.print(percentage, 1);
    Serial.println("%");
    
}
void sendPHValue() {
    int sensorValue = analogRead(PH_PIN);
    float voltage = sensorValue * (3.3 / 4095.0); // ESP32 ADC 12-bit (0 - 4095)
    float phValue = 3.5 * voltage - OFFSET; // Công thức chuyển đổi

    Blynk.virtualWrite(V1, phValue); // Gửi giá trị pH lên Blynk (V1)
    Serial.print("Giá trị pH: ");
    Serial.println(phValue);
}

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    sensors.begin();

    timer.setInterval(1000L, sendTemperature);
    timer.setInterval(SAMPLING_INTERVAL, sendLiquidLevel);
    timer.setInterval(1000L, sendPHValue); // Đọc giá trị pH mỗi giây
}

void loop() {
    Blynk.run();
    timer.run();
}
