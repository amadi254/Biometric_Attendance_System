
# Biometric Attendance System Firmware  

## Overview  
This is the firmware for a fingerprint attendance system used to track staff clock-in and clock-out times. It works with a PHP API, a JavaScript user interface, and runs on an ESP32 using C++.  

## Features  
- Uses **DY50-V2** fingerprint sensor to scan fingerprints  
- Connects to WiFi to send data to the server in real time  
- Sends fingerprint ID to an API for processing  
- Prevents double clock-in or clock-out on the same day  
- Stores attendance records in an SQL database  

## How It Works  
1. **Connecting to WiFi**  
   - When the device starts, it connects to WiFi.  

2. **Clocking In**  
   - The user presses the "Clock-In" button.  
   - The device scans the fingerprint and checks if it is in memory.  
   - If a match is found, it sends the fingerprint ID to:  
     ```
     https://amadi.tech/embeddedsys/api/clock-in.php
     ```
   - The API records the fingerprint ID, date, and time in the database.  

3. **Clocking Out**  
   - The same process happens when pressing the "Clock-Out" button.  

4. **Preventing Multiple Entries**  
   - The system does not allow clocking in or out more than once a day.  

## Technologies Used  
- **Firmware:** C++ (ESP32/Arduino)  
- **Backend:** PHP with SQL database  
- **Frontend:** JavaScript  

## Setup Guide  
1. **Upload the Firmware**  
   - Flash the firmware to your ESP32 using Arduino IDE or PlatformIO.  

2. **Connect to WiFi**  
   - Update the code with your WiFi name (SSID) and password.  

3. **Set Up API and Database**  
   - Install the PHP API and set up the SQL database.  

4. **Run the User Interface**  
   - Host the JavaScript frontend for managing records.  

## License  
This firmware is **free to use**.  

## Author  
**Amadi Collins**  
- 📧 Email: [admin@amadi.tech](mailto:admin@amadi.tech)  
- 📞 Mobile: +254794693472
