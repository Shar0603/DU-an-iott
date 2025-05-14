#define BLYNK_TEMPLATE_ID "TMPL6xahK8SKs"
#define BLYNK_TEMPLATE_NAME "Becathongminh1"
#define BLYNK_AUTH_TOKEN "17HSZAw0DeUKc985EQvAlrKDcUhBAm2V"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1X15.h>

#define ONE_WIRE_BUS 5
#define SENSOR_PIN 34
#define OFFSET 8
#define RELAY1_PIN 25    // Relay 1 (máy bơm 1 - rút nước)
#define RELAY2_PIN 26    // Relay 2 (máy bơm 2 - bơm nước vào)

char ssid[] = "Lyngh";
char pass[] = "888888888";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADS1115 ads;

// Trạng thái thông báo
bool notifiedLowTemp = false;
bool notifiedHighTemp = false;
bool notifiedLowWater = false;
bool notifiedLowPH = false;
bool notifiedHighPH = false;

// Thời gian lần cuối gửi thông báo
unsigned long lastNotifyTimeTemp = 0;
unsigned long lastNotifyTimeWater = 0;
unsigned long lastNotifyTimePH = 0;
const unsigned long notifyInterval = 20000;

// Trạng thái máy bơm
bool pump1Active = false;
bool pump2Active = false;

// Hàm gửi cảnh báo với kiểm tra tránh spam
void sendAlert(const char* eventCode, const char* message, bool& notified, unsigned long& lastNotifyTime) {
  unsigned long currentMillis = millis();
  if (!notified || (currentMillis - lastNotifyTime >= notifyInterval)) {
    Blynk.logEvent(eventCode, message);
    notified = true;
    lastNotifyTime = currentMillis;
  }
}

float readTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Blynk.virtualWrite(V0, tempC);
  Serial.print("Nhiet do: ");
  Serial.println(tempC);

  if (tempC < 20) {
    sendAlert("canh_bao_nhiet_do_thap", "Nhiệt độ thấp hơn 20°C!", notifiedLowTemp, lastNotifyTimeTemp);
    notifiedHighTemp = false;
  } else if (tempC > 32) {
    sendAlert("canh_bao_nhiet_do_cao", "Nhiệt độ cao hơn 32°C!", notifiedHighTemp, lastNotifyTimeTemp);
    notifiedLowTemp = false;
  } else {
    notifiedLowTemp = false;
    notifiedHighTemp = false;
  }

  return tempC;
}

float readWaterLevel() {
  int rawValue = analogRead(SENSOR_PIN);
  float percentage = map(rawValue, 0, 4095, 0, 100);
  percentage = constrain(percentage, 0, 100);
  Blynk.virtualWrite(V2, percentage);
  Serial.print("Muc nuoc: ");
  Serial.println(percentage);

  if (percentage < 50) {
    sendAlert("canh_bao_muc_nuoc", "Mực nước thấp hơn 50%!", notifiedLowWater, lastNotifyTimeWater);
  } else {
    notifiedLowWater = false;
  }

  return percentage;
}

float readPH() {
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float voltage = adc0 * 0.1875 / 1000.0;
  float phValue = 3.5 * voltage - OFFSET;
  Blynk.virtualWrite(V1, phValue);
  Serial.print(" | pH: ");
  Serial.println(phValue);

  if (phValue < 4) {
    sendAlert("canh_bao_ph_thap", "pH thấp hơn 4!", notifiedLowPH, lastNotifyTimePH);
    notifiedHighPH = false;
  } else if (phValue > 8) {
    sendAlert("canh_bao_ph_cao", "pH cao hơn 8!", notifiedHighPH, lastNotifyTimePH);
    notifiedLowPH = false;
  } else {
    notifiedLowPH = false;
    notifiedHighPH = false;
  }

  return phValue;
}

void updateLCD(float tempC, float waterLevel, float phValue) {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(tempC, 1);
  lcd.print((char)223);
  lcd.print(" N:");
  lcd.print(waterLevel, 0);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("pH:");
  lcd.print(phValue, 2);
  lcd.print("       ");
}

// Xử lý nút nhấn ảo V3
BLYNK_WRITE(V3) {
  int value = param.asInt(); // Lấy giá trị từ nút V3 (0 hoặc 1)
  if (value == 1 && !pump1Active && !pump2Active) {
    pump1Active = true;
    digitalWrite(RELAY1_PIN, LOW); // Bật máy bơm 1
    Serial.println("Pump 1 ON");
  }
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Wire.begin(21, 22);

  // Cấu hình chân relay
  pinMode(RELAY1_PIN, OUTPUT);
  pinMode(RELAY2_PIN, OUTPUT);
  digitalWrite(RELAY1_PIN, HIGH); // Tắt relay 1
  digitalWrite(RELAY2_PIN, HIGH); // Tắt relay 2

  if (!ads.begin()) {
    Serial.println("Khong tim thay ADS1115!");
    while (1);
  }

  sensors.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Khoi dong...");
  delay(2000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  float temp = readTemperature();
  float water = readWaterLevel();
  float pH = readPH();
  updateLCD(temp, water, pH);

  // Điều khiển máy bơm
  if (pump1Active) {
    if (water <= 60) { // Khi mức nước còn 60%
      digitalWrite(RELAY1_PIN, HIGH); // Tắt máy bơm 1
      pump1Active = false;
      Serial.println("Pump 1 OFF");

      digitalWrite(RELAY2_PIN, LOW); // Bật máy bơm 2
      pump2Active = true;
      Serial.println("Pump 2 ON");
    }
  }

  if (pump2Active) {
    if (water >= 100) { // Khi bể đầy
      digitalWrite(RELAY2_PIN, HIGH); // Tắt máy bơm 2
      pump2Active = false;
      Serial.println("Pump 2 OFF");

      Blynk.virtualWrite(V3, 0); // Tắt nút V3
      Serial.println("V3 Button OFF");
    }
  }

  delay(1000);
}
