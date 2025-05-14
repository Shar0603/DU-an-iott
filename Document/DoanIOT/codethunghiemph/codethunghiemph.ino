#define SensorPin 34  // Chân analog đọc tín hiệu từ cảm biến pH
float calibration_value = 21.34 - 0.7;  // Hiệu chỉnh giá trị pH nếu cần
int pHValue = 0;
float voltage = 0;
void setup() {
    Serial.begin(115200);
}

void loop() {
    int rawValue = analogRead(SensorPin);
    voltage = rawValue * 5 / 4095.0; // 
    pHValue = -5.7 * voltage + calibration_value + 9.08;  // 
    
    Serial.print("Raw ADC: ");
    Serial.print(rawValue);
    Serial.print(" | Voltage: ");
    Serial.print(voltage, 3);
    Serial.print("V | pH Value: ");
    Serial.println(pHValue, 2);
    
    delay(200);
}
