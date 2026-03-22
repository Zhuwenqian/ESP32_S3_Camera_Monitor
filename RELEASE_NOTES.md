# ESP32-S3 Camera Monitor - Release Notes

# Release Title: **ESP32-S3 Camera Monitor v1.0.0 - "Pan-Tilt Guardian"**

## Version 1.0.0 - February 3, 2026

---

## Table of Contents
1. [Project Overview](#project-overview)
2. [Hardware Design](#hardware-design)
3. [Complete Update History](#complete-update-history)
4. [Important Notes](#important-notes)
5. [Future Extensions](#future-extensions)
6. [System Requirements](#system-requirements)
7. [Known Issues](#known-issues)

---

## Project Overview

**Project Name:** ESP32-S3 Camera Monitor  
**Version:** 1.0.0  
**Release Date:** February 3, 2026  
**License:** Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported (CC BY-NC-SA 3.0)

### Description
ESP32-S3 Camera Monitor is a comprehensive network video surveillance system based on the ESP32-S3-CAM development board. The system features real-time video streaming, photo capture, video recording, and remote pan-tilt control via web interface. This release includes complete mechanical design files created with SOLIDWORKS.

### Key Features
- Real-time MJPEG video streaming via web interface
- Photo capture with automatic SD card storage
- Automatic video recording with 2-minute segmentation
- Remote pan-tilt control using two SG-90 servos
- Bilingual web interface (English/Chinese) with light/dark themes
- HTTP Basic Authentication with session management
- SD card file management with auto-cleanup
- NTP time synchronization for timestamped filenames

### Code Sources
This project is modified from the following open-source projects:
- Freenove ESP32-S3 Camera Example (CC BY-NC-SA 3.0)
- Espressif Camera Library Example (Apache License 2.0) - from Arduino ESP32 Board Package

---

## Hardware Design

### SOLIDWORKS 3D Model and Engineering Drawings

This release includes complete mechanical design files for the pan-tilt camera mount:

#### Design Files Location
```
3D_Models/
├── ESP32-S3 Camera Monitor.SLDASM    # Main assembly file
├── Gimbal base.SLDPRT                 # Base mounting bracket
├── Gimbal Center.SLDPRT               # Horizontal rotation bracket
├── Gimbal Top.SLDPRT                  # Vertical rotation bracket
├── Controller Enclosure.SLDPRT        # Electronics enclosure
├── Controller Enclosure Cover.SLDPRT  # Enclosure cover
├── OV5640 Camera.SLDPRT              # Camera model
├── Servo-Pan.SLDPRT                   # Pan servo model
├── Servo-Tilt.SLDPRT                  # Tilt servo model
├── BOM.xls                            # Bill of Materials
├── ESP32-S3 Camera Monitor eDrawing.SLDDRW  # Engineering drawing
├── ESP32-S3 Camera Monitor eDrawing.pdf      # PDF drawing
├── ESP32-S3 Camera Monitor eDrawing.DXF      #DXF drawing
├── ESP32-S3 Camera Monitor Assembly and Disassembly Demonstration.mp4  # Sim Assembly video
├── ESP32-S3CAM.pdf                    # Camera datasheet
├── ESP32-S3CAM Schematic diagram.pdf  # Camera schematic
├── ESP32S3CAM-Pin.jpg                 # Pinout diagram
├── esp32s3cam scale.png               # Scale reference
├── Controller Enclosure.3MF           # 3D print file
├── Controller Enclosure Cover.3MF     # 3D print file
├── Gimbal base.3MF                    # 3D print file
├── Gimbal Center.3MF                  # 3D print file
└── Gimbal Top.3MF                     # 3D print file
```

#### Design Specifications
- **Pan Range:** 0-180 degrees (horizontal rotation)
- **Tilt Range:** 0-180 degrees (vertical rotation)
- **Camera Compatibility:** ESP32-S3-CAM (Guoyun version)
- **Servo Compatibility:** SG-90 micro servos (2x)
- **Material:** PLA/PETG/PETG plus for 3D printed parts (recommended)
- **Print Settings:** Generate support, 30% infill


#### Assembly Instructions Overview
1. Print all 3D parts using recommended settings
2. Assemble by referring to the sim video

*Detailed assembly tutorial video will be uploaded to Bilibili (planned for future release)*

---

## Complete Update History

### 2026-02-03 - Added SOLIDWORKS 3D Model and BOM
**Updates:**
- Created complete pan-tilt mechanical design using SOLIDWORKS
- Designed 9 individual parts: Gimbal base, Gimbal Center, Gimbal Top, Controller Enclosure, Controller Enclosure Cover, OV5640 Camera, Servo-Pan, Servo-Tilt
- Generated engineering drawings for all parts with dimensions
- Exported 3MF files for 3D printing
- Created comprehensive Bill of Materials (BOM) with 16 items
- Documented assembly instructions and print settings
- Designed for SG-90 servos and ESP32-S3-CAM compatibility
- Created assembly demonstration video
- Included camera datasheet and schematic diagram

**Design Files:**
- Main assembly: ESP32-S3 Camera Monitor.SLDASM
- Individual parts: 9 .SLDPRT files
- Engineering drawings: ESP32-S3 Camera Monitor eDrawing.SLDDRW
- 3D print files: 5 .3mf files (Controller Enclosure, Cover, Gimbal base, Center, Top)
- BOM: BOM.xls
- Documentation: Assembly video, datasheet, schematic

**Hardware Specifications:**
- Pan range: 0-180 degrees (horizontal)
- Tilt range: 0-180 degrees (vertical)
- Material: PLA/PETG recommended
- Print settings: 0.2mm layer height, 3 perimeters, 20% infill
- Estimated print time: ~4-6 hours total
- Included electronics enclosure for ESP32-S3-CAM

---


## Important Notes

### 1. SD Card Space Auto-Cleanup (UNTESTED)
**Status:** ⚠️ **NOT TESTED**

The automatic cleanup feature for old files when SD card space reaches 55GB threshold has been implemented but **not tested in real-world conditions**.

**Implementation Details:**
- Trigger threshold: 55GB used space
- Cleanup priority: videos directory first, then photos directory
- Files are deleted in chronological order (oldest first)
- Cleanup occurs before starting new video recording

**Recommendations:**
- Test this feature with a dedicated SD card before production use
- Monitor SD card space usage regularly
- Consider implementing manual cleanup as backup
- Report any issues with file cleanup functionality

### 2. Authentication Security
- Default username: `admin`
- Default password: `password123`
- **Strongly recommended** to change default credentials before deployment
- Use strong passwords with uppercase, lowercase, numbers, and special characters
- Session expires after 14 days
- IP authorization expires after 5 minutes of inactivity

### 3. Hardware Pin Conflicts
The following GPIO pins are **reserved for camera use** and cannot be used for servos or other peripherals:
- GPIO 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18

**Available pins for servos:**
- GPIO 14 (recommended for pan servo - near 3.3V pin)
- GPIO 21 (recommended for tilt servo - near GND pin)
- GPIO 1, 2, 3, 19, 20, 35-48

### 4. Power Requirements
- ESP32-S3-CAM: 5V via USB or external power
- Servos: **External 5V power supply strongly recommended**
- Do not power both servos directly from ESP32 board (insufficient current)
- Ensure common ground between ESP32 and servo power supply

### 5. Video Recording
- Video recording starts automatically on system startup
- Auto-segmentation every 2 minutes
- File format: AVI (MJPEG encoding)
- Default resolution: 1024x768
- Default frame rate: 20fps
- Files saved to: `/camera/videos/` directory on SD card

### 6. Network Requirements
- WiFi connection required for web interface access
- Default HTTP port: 80
- Default video stream port: 81
- Ensure stable WiFi signal for smooth video streaming

---

## Future Extensions

### 1. Feature Extensions
- **Motion Detection:** Implement motion detection algorithm to trigger recording or alerts
- **OTA Updates:** Add Over-The-Air firmware update capability

### 2. Performance Extensions
- **Higher Resolution:** Support for UXGA (1600x1200) and higher resolutions
- **Higher Frame Rate:** Optimize for 30fps video streaming
- **Video Stream Optimization:** Implement adaptive bitrate streaming

### 3. Documentation and Community
- **Assembly Tutorial:** Create detailed video tutorial and upload to Bilibili
- **Community Support:** Establish user forum or discussion group
- **Additional Language Support:** Add more language options to web interface

### 4. Hardware Variants
- **Weatherproof Enclosure:** Design outdoor-rated housing
- **Battery Power:** Add battery management and solar charging support
- **Additional Sensors:** Integrate temperature, humidity, or PIR sensors

---

## System Requirements

### Hardware Requirements
- **Development Board:** ESP32-S3-CAM (Guoyun version with OV5640 camera)
- **Servos:** 2x SG-90 micro servos (9g, 180 degree)
- **SD Card:** 32GB or larger, Class 10 (supports exFAT)
- **Power Supply:** 5V 2A minimum for servos + ESP32
- **Mechanical:** 3D printed parts (see BOM section)

### Software Requirements
- **Arduino IDE:** 2.x or later
- **ESP32 Board Package:** v2.0.14 or later
- **Required Libraries:**
  - ESP32Servo (v1.1.0)
  - ArduinoJson (v6.x)

### Browser Compatibility
- Chrome 80+
- Firefox 75+
- Safari 13+
- Edge 80+

### Network Requirements
- WiFi 802.11 b/g/n
- 2.4GHz band support
- WPA2 or WPA3 encryption

---

## Known Issues

1. **SD Card Auto-Cleanup:** Feature implemented but not tested (see Important Notes)
2. **Servo Power:** Direct powering from ESP32 may cause instability; use external power
3. **WiFi Stability:** Video streaming may be affected by weak WiFi signal
4. **Browser Compatibility:** Some older browsers may not support all features
5. **File System:** Very large SD cards (>128GB) may have compatibility issues

---

## Support and Contribution

### License
This project is licensed under CC BY-NC-SA 3.0:
- ✅ Non-commercial use allowed
- ❌ Commercial use prohibited
- ✅ Modification and distribution allowed (same license)
- ✅ Attribution required

### Contact
For questions, issues, or contributions, please refer to the project documentation or contact the maintainers.

---

**Date:** February 3, 2026  

---

*End of Release Notes*
