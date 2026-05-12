#if defined(ESP32)
  #include <WiFi.h>
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
#endif
#include <WiFiClientSecure.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

const char* ssid = "pranav";
const char* password ="12345678";

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

double collegeLat = 12.870497;
double collegeLng = 80.219833;

// Simulated bus location starting near college
double busLat = 12.870497;
double busLng = 80.219833;

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
    WiFiClientSecure client;
    
    #if defined(ESP8266)
    client.setBufferSizes(512, 512); // Reduce SSL buffer size to prevent ESP8266 memory crashes
    #endif
    
    client.setInsecure(); // Bypass SSL certificate validation for Firebase
    HTTPClient http;

    String url = "https://bustracker-b8007-default-rtdb.firebaseio.com/bus.json";

    http.begin(client, url); 
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.addHeader("Content-Type", "application/json");

    String data = "{\"lat\":" + String(busLat, 6) + ",\"lng\":" + String(busLng, 6) + ",\"ts\":" + String(millis()) + "}";

    int httpCode = http.PUT(data);

    // Print HTTP Status to OLED so user can see it!
    display.setTextSize(1);
    display.setCursor(0, 45);
    display.print("FB Code: ");
    display.print(httpCode);
    display.display();

    if (httpCode > 0) {
      Serial.printf("[HTTP] PUT... code: %d\n", httpCode);
    } else {
      Serial.printf("[HTTP] PUT... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }


  delay(1000); // Wait 1 second before next update
}