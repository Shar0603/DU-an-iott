#define BLYNK_TEMPLATE_ID "TMPL6gw-MsB4g"
#define BLYNK_TEMPLATE_NAME "Becathongminh"
#define BLYNK_AUTH_TOKEN "rmVngQfsyQAL1amLF4zKjPWjAbCWDMEH"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char ssid[] = "iPhone của Dương";       // Đổi thành tên WiFi của bạn
char pass[] = "11111111";  // Đổi thành mật khẩu WiFi của bạn

void setup() {
    Serial.begin(115200);
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

void loop() {
    Blynk.run();  // Giữ kết nối với Blynk
}
