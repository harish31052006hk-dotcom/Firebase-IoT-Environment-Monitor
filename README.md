# Firebase IoT Environment Monitor

### Task 5 — Firebase Logging, Automation & Data Export

> The capstone project integrating physical sensors, an ESP32, Firebase cloud services, and autonomous hysteresis logic to create a fully self-sufficient smart environment monitor.

---

## 📌 Overview
This task unites the physical hardware with the custom Firebase cloud dashboard created in Task 4. The ESP32 gathers environmental data from DHT11 and LDR sensors, logs it to the cloud, and operates an AC bulb. It supports a two-way communication model where it can be manually controlled via the dashboard or operate completely autonomously based on environmental thresholds.

## 🎯 Objectives
- Wire DHT11, LDR, and Relay modules to the ESP32.
- Integrate Firebase ESP32 Client libraries.
- Push live telemetry (Temp, Humidity, Light) and historical logs to Firebase.
- Implement Manual/AUTO mode toggling from the cloud.
- Code embedded hysteresis logic to prevent relay chatter during autonomous operation.

## 🧠 Concepts Covered
- Embedded Hysteresis Logic
- Two-way Cloud Synchronization
- Sensor Data Acquisition
- JSON Serialization on Microcontrollers
- Edge vs. Cloud Automation

## 🏗️ System Architecture
1. **Sensors**: DHT11 (GPIO 4) and LDR (GPIO 34) read physical environment data.
2. **ESP32 Edge Node**: Processes data, applies automation logic, and syncs with Firebase over Wi-Fi.
3. **Actuator**: Relay (GPIO 5) switches the high-voltage Bulb.
4. **Cloud**: Firebase Realtime Database acts as the central state manager.
5. **Dashboard**: Web UI for monitoring and manual override.

## 🔧 Hardware
- ESP32 Development Board
- DHT11 Temperature & Humidity Sensor
- LDR (Photoresistor) + Fixed Resistor (Voltage Divider)
- 2-Channel Relay Module
- 230V AC Bulb
- Breadboard & Jumper Wires

## 💻 Software
- Arduino IDE (C++)
- `Firebase ESP32 Client` Library
- `DHT sensor library`

## ⚙️ Implementation
- **Data Logging**: The ESP32 reads sensors every 3 seconds and pushes to `sensors/`. Every 15 seconds, it pushes a historical log entry to `logs/` using a Firebase Server Timestamp.
- **Automation Configuration**: 
  - **ON**: Temperature >= 30°C OR Humidity >= 70%
  - **OFF**: Temperature <= 26°C AND Humidity < 70%
- **Mode Switching**: The ESP32 listens to `control/mode`. If set to `MANUAL`, it obeys `control/bulb` commands from the dashboard. If `AUTO`, it uses the onboard hysteresis logic to trigger the relay.

## 🧪 Testing
- **Manual Mode**: Verified the dashboard instantly toggled the physical bulb.
- **Auto Mode**: Applied heat/moisture to the DHT11. Observed the bulb turn ON when thresholds were exceeded, and remain ON until values dropped past the lower hysteresis bound, preventing rapid flickering.
- **Data Export**: Successfully downloaded hours of continuous CSV telemetry from the web dashboard.

## 🛠️ Challenges & Fixes
- **Relay Chatter**: In initial tests, the bulb flickered rapidly when temperature fluctuated exactly at 30°C. Implemented Hysteresis (lower threshold at 26°C) to create a deadband, resolving the mechanical chatter.
- **Memory Leaks**: Pushing large JSON payloads too quickly crashed the ESP32. Separated the live update loop (3s) from the historical logging loop (15s) to manage heap memory effectively.

## 📚 Learning Outcomes
- Realized the importance of edge computing (handling automation on the ESP32) to ensure the system works even if the internet temporarily drops.
- Mastered robust JSON handling and Firebase streaming on microcontrollers.

## 💭 Reflection
This capstone task beautifully combined all the concepts learned throughout the week. It demonstrated that true IoT is not just remote control, but autonomous, data-driven systems capable of self-regulation.

## 🔗 Project Information
- **Live Login**: [https://env-monitor-845af.web.app/index.html](https://env-monitor-845af.web.app/index.html)
- **Live Dashboard**: [https://env-monitor-845af.web.app/dashboard.html](https://env-monitor-845af.web.app/dashboard.html)
- **Developer**: Harish Kumaran
- **Course**: ProtoSem

## 🏁 Conclusion
The Firebase IoT Environment Monitor is a resounding success, offering professional-grade telemetry logging, robust automation, and a highly responsive cloud interface.
