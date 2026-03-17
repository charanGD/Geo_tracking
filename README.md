# Live Bus Tracker (IoT via ESP8266) - Bus 12B

A complete Full-Stack real-time bus tracking system using an ESP8266 microcontroller, a GPS module, Google Firebase Realtime Database, and a Leaflet map front-end web application.

## 🚀 Features
* **Real-Time Tracking:** Bus locations are pushed instantly to Firebase and broadcasted to any open web clients without needing to refresh the page.
* **Geofencing:** Uses the Haversine formula to detect when the bus enters or leaves the college radius, displaying the status on a physical OLED screen on the bus dashboard.
* **Intelligent Timeout:** The web client automatically grays out the bus and sets the status to "Not Active" if it stops receiving pings for more than 10 seconds.
* **Smooth Camera Panning:** The frontend utilizes Leaflet's `flyTo` for smooth, cinematic camera panning as the GPS updates.

## 🛠️ Hardware Setup (The Bus Side)
The `esp_tracker.ino` code is designed to run on an ESP8266 (like a NodeMCU) with a NEO-6M GPS module and a generic SSD1306 I2C OLED display.

### Wiring:
**GPS Module:**
* VCC: 3.3V
* GND: GND
* TX: D1
* RX: D2

**OLED Display (I2C):**
* VCC: 3.3V
* GND: GND
* SCL: D1
* SDA: D2

## 🌐 Software Setup (The Client Side)
1. Ensure your Firebase Realtime database is set up and allows read/write access.
2. Put the generated Firebase DB URL into the `esp_tracker.ino` code and the `index.html` file.
3. Open `index.html` in any web browser, or host it locally utilizing python:
```bash
python3 -m http.server 8000
```
