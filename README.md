## Smart Interactable Dashboard for Real-Time Monitoring & Diagnosis of Electric Vehicle Performance

### Project Overview:

This project involves the development of a smart dashboard for real-time monitoring and diagnosis of electric vehicle (EV) performance using an ESP32 microcontroller. Data from multiple sensors (e.g., temperature, speed, battery SOC) is transmitted to Firebase and visualized on a web-based dashboard. The project includes hardware implementation as well as virtual simulation in Wokwi to test the data flow and dashboard interactions.

### Block Diagram:

![Block Diagram](https://github.com/user-attachments/assets/7c84904c-d874-4127-ba29-df7c63de147f)


* Sensors (Temperature, Battery SOC, Speed, Vehicle Charging Condition, GPS)
* Electric Vehicle Controller (ESP32)
* Firebase Cloud Database
* Web Dashboard for Real-Time Monitoring

### Hardware Implementation:

![Hardware labeling_1](https://github.com/user-attachments/assets/08ea8e71-9745-483c-bcce-914e549fa645)


* ESP32 microcontroller for data acquisition and communication.
* Sensors:

  * NEO M8N GPS Module
  * Dallas Temperature Sensor (DS18B20)
  * Voltage Sensor
  * Current Sensor
  * Rotary Encoder for speed sensing
  * NTC Temperature Sensor
  * Potentiometer for Tyre Pressure
* Relay Module for controlling indicators and other actuators.
* Power Supply: Battery pack powering sensors and ESP32.
* Communication Protocol: ESP32 transmits sensor data to Firebase, and the web dashboard fetches and visualizes the data.

### Simulation in Wokwi:

![Wokwi Connection Diagram - Copy](https://github.com/user-attachments/assets/12435476-c8c4-4d13-8000-85e92641177b)


* The hardware setup is simulated using Wokwi to verify sensor data acquisition and communication to Firebase.
* Firebase integration is tested using Wokwi's internet-enabled ESP32 to send real-time data to Firebase and display it on the web dashboard.

### Firebase to Web Dashboard Communication:

### Hardware/Simulation to Fire Base ###

![Screenshot 2025-04-19 210237](https://github.com/user-attachments/assets/4bb57cb9-313c-4f6b-b55e-708e1276ae9d)


### Firebase to Web Dashboard ###

![Web 1](https://github.com/user-attachments/assets/c6879905-9fd5-46f8-8a3d-45032e3f7dd1)
![Web 2](https://github.com/user-attachments/assets/28b34f6f-116e-4625-8c53-c91c5a78f296)
![Web 3](https://github.com/user-attachments/assets/cfdf2068-edcd-4ccb-a163-f9de9502d8d3)



* Real-time data from the vehicle is sent to Firebase through the ESP32.
* Web dashboard retrieves and visualizes data using Firebase SDK, displaying critical metrics like speed, temperature, battery status, and GPS location.

### Future Scope:

* Implement autonomous control and diagnostic feedback to the vehicle.
* Develop mobile app integration for enhanced monitoring and alerts.
* Expand sensor integration for more comprehensive data analysis.
