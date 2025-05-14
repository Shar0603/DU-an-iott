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

void setup() {
  Serial.begin(115200);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  Wire.begin(21, 22);

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

  delay(1000);
}
