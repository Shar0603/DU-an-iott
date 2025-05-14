#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_ADS1X15.h>

#define BLYNK_TEMPLATE_ID "TMPL6gw-MsB4g"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "rmVngQfsyQAL1amLF4zKjPWjAbCWDMEH"

#define ONE_WIRE_BUS 5
#define SENSOR_PIN 34

#define OFFSET 5
#define SAMPLING_INTERVAL 1000
#define MOISTURE_LOW 80
#define MOISTURE_HIGH 100

char ssid[] = "Lyngh";
char pass[] = "888888888";

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_ADS1115 ads;

float readTemperature() {
  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0);
  Blynk.virtualWrite(V0, tempC);
  Serial.print("Nhiet do: ");
  Serial.print(tempC);
  Serial.println(" C");
  return tempC;
}

float readWaterLevel() {
  int rawValue = analogRead(SENSOR_PIN);
  float percentage = map(rawValue, 0, 4095, 0, 100);
  percentage = constrain(percentage, 0, 100);
  Blynk.virtualWrite(V2, percentage);
  Serial.print("Muc nuoc: ");
  Serial.print(percentage, 1);
  Serial.println(" %");
  return percentage;
}

float readPH() {
  int16_t adc0 = ads.readADC_SingleEnded(0);
  float voltage = adc0 * 0.125 / 1000.0;
  float phValue = 3.5 * voltage;
  Blynk.virtualWrite(V1, phValue);
  Serial.print(" | pH: "); Serial.println(phValue, 2);
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
