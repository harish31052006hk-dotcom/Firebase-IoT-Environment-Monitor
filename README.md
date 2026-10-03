# Firebase IoT Environment Monitor

### Task 5 — Firebase Logging, Automation & Data Export

---

## Task Overview
The capstone project integrating physical sensors, an ESP32, Firebase cloud services, and autonomous hysteresis logic to create a fully self-sufficient smart environment monitor. 

## Problem / Purpose
To build a complete edge-to-cloud IoT product that not only reports telemetry but makes intelligent, autonomous decisions locally while remaining configurable and monitored via a secure cloud dashboard.

## Objectives
- Wire DHT11, LDR, and Relay modules to the ESP32.
- Integrate Firebase ESP32 Client libraries.
- Push live telemetry (Temp, Humidity, Light) and historical logs to Firebase.
- Implement Manual/AUTO mode toggling from the cloud.
- Code embedded hysteresis logic to prevent relay chatter.

## Concepts Covered
- Embedded Hysteresis Logic
- Two-way Cloud Synchronization
- Sensor Data Acquisition
- JSON Serialization on Microcontrollers
- Edge Computing vs Cloud Automation

## System Architecture

```text
  [ DHT11 ]    [ LDR ]
      \          /
    (GPIO 4) (GPIO 34)
        \      /
     [ ESP32 Node ] ---(GPIO 5)---> [ Relay ] ---> [ Bulb ]
           |
       (Wi-Fi)
           |
           v
 [ Firebase RTDB Cloud ] <---> [ Web Dashboard (Task 4) ]
```

## Data Flow
1. ESP32 reads sensors every 3000ms.
2. ESP32 evaluates automation logic (if in AUTO mode) and controls the relay.
3. ESP32 pushes live data to `/sensors`.
4. Every 15000ms, ESP32 pushes a logged data point with a Firebase Server Timestamp to `/logs`.
5. Dashboard instantly reflects state changes and telemetry.

## Hardware
| Component | Description |
|-----------|-------------|
| ESP32 | Main microcontroller |
| DHT11 | Temperature and Humidity sensor |
| LDR | Photoresistor for ambient light |
| 2-Channel Relay | AC switching module |
| 230V AC Bulb | Actuator |

## Software & Technologies
| Technology | Role |
|------------|------|
| Arduino IDE | Firmware development |
| `Firebase ESP32 Client` | Handles secure RTDB communication |
| `DHT sensor library` | Parses DHT11 one-wire protocol |

## Wiring / Pin Configuration
| ESP32 Pin | Component |
|-----------|-----------|
| GPIO 4 | DHT11 Data |
| GPIO 34 | LDR (Voltage Divider) |
| GPIO 5 | Relay IN |

## Configuration
- `READ_INTERVAL_MS`: 3000 ms
- `LOG_INTERVAL_MS`: 15000 ms
- `LDR_DARK_THRESHOLD`: 1500

## Automation Logic (Hysteresis)
The system utilizes hysteresis to prevent mechanical relay chatter:
- **ON Condition**: Temperature >= 30°C OR Humidity >= 70%
- **OFF Condition**: Temperature <= 26°C AND Humidity < 70%
*(Note: LDR is currently configured for monitoring only).*

## Implementation Workflow
The firmware establishes Wi-Fi and connects to Firebase using Legacy Token Auth. It runs two non-blocking timers: one for live updates and one for historical logging. It actively listens to changes in `/control/mode`.

## Source Code Explanation
- `task5_esp32_firebase.ino`: The main codebase. Handles sensor reading, Firebase JSON construction, and automation evaluation.
- `Firebase.pushJSON()`: Used to append new historical records securely.
- `Firebase.RTDB.setTimestamp()`: Injects accurate server-side timing into the logs.

## Testing
- **Manual Mode**: Verified the dashboard instantly toggled the physical bulb.
- **Auto Mode**: Applied heat/moisture to the DHT11. Observed the bulb turn ON when thresholds were exceeded, and remain ON until values dropped past the lower hysteresis bound, preventing rapid flickering.

## Evidence
- *Note: Complete architectural diagrams, CSV exports, and live hardware operation videos are documented in the ProtoSem weekly report.*

## Challenges & Fixes
- **Relay Chatter**: In initial tests, the bulb flickered rapidly when temperature fluctuated exactly at 30°C. Implemented Hysteresis (lower threshold at 26°C) to create a deadband, resolving the mechanical chatter.
- **Memory Leaks**: Pushing large JSON payloads too quickly crashed the ESP32. Separated the live update loop from the historical logging loop to manage heap memory effectively.

## Key Learnings
- Realized the importance of edge computing (handling automation on the ESP32) to ensure the system works even if the internet temporarily drops.
- Mastered robust JSON handling and Firebase streaming on microcontrollers.

## Future Improvements
- Integrate the LDR into the active automation logic (e.g., turn on light only if Temp is high AND it is dark).

## Reflection
This capstone task beautifully combined all the concepts learned throughout the week. It demonstrated that true IoT is not just remote control, but autonomous, data-driven systems capable of self-regulation.

## Project Links
- [Live Dashboard](https://env-monitor-845af.web.app/)
- [Task 5 Implementation Code](./task5_esp32_firebase.ino)
- Developer: Harish Kumaran (ProtoSem Week 7)

## Conclusion
The Firebase IoT Environment Monitor is a resounding success, offering professional-grade telemetry logging, robust edge automation, and a highly responsive cloud interface.
