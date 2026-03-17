#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

const char* ssid = "iQOO Z7 Pro 5G";
const char* password = "1234567890";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

double collegeLat = 12.870497;
double collegeLng = 80.219833;

// Simulated bus location starting near college
double busLat = 12.870500;
double busLng = 80.219820;

double distance(double lat1, double lon1, double lat2, double lon2) {
  // Haversine formula to calculate distance between two GPS coordinates in meters
  double R = 6371000; // Radius of Earth in meters
  double dLat = (lat2 - lat1) * PI / 180;
  double dLon = (lon2 - lon1) * PI / 180;
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(lat1 * PI / 180) * cos(lat2 * PI / 180) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c; 
}

void setup() {
  Serial.begin(115200);

  // 1. Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Connecting WiFi...");
  display.display();

  // 2. Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nWiFi connected");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.display();
  delay(1000);
}

void loop() {
  // 1. Calculate Distance
  double d = distance(busLat, busLng, collegeLat, collegeLng);

  // 2. Update OLED Display
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Distance: ");
  display.print(d, 1);
  display.print(" meters");

  display.setTextSize(2);
  display.setCursor(0, 20);
  if (d < 200) {
    display.println("INSIDE CLG");
  } else {
    display.println("OUTSIDE CLG");
  }
  display.display();

  // 3. Send Data to Firebase
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String url = "https://bustracker-b8007-default-rtdb.firebaseio.com/bus.json";

    http.begin(client, url); 
    http.addHeader("Content-Type", "application/json");

    String data = "{\"lat\":" + String(busLat, 6) + ",\"lng\":" + String(busLng, 6) + "}";

    int httpCode = http.PUT(data);

    if (httpCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }

  // Simulate bus movement away from college for testing
  busLat += 0.0001; 
  busLng -= 0.0001;

  delay(3000); // Wait 3 seconds before next update
}
