# Homepod




## What is it 
Its a tiny homepod for for a smart home. 

It consists of a esp32 s3 n16r8 as its main board.

A 2.8inch touch TFT display for smooth navigation.

Also a joystick module for extra navigation specs.


### **Bill of Materials (BOM)**

| Item # | Component Name | Description | Qty | Est. Unit Price | Total Price | Primary Link | Alternative Link |
| :---: | :--- | :--- | :---: | :---: | :---: | :---: | :---: |
| **1** | K023 Joystick Module | Analog 2-axis joystick with push button | 1 | $0.50 | $0.50 | https://robocraze.com/products/joystick-module?variant=40192421527705c | 
| **2** | ESP32-S3 N16R8 | Development board (16MB Flash / 8MB PSRAM) | 1 | $10.00 | $10.00 | https://robocraze.com/products/7semi-esp32-s3-dev-boardc-1-n8r8-wifi-bluetooth-dual-usb-c-rgb-led?variant=48519036272864|
| **3** | SmartElex 2.8" TFT Touch Screen | Display module with XPT2046 touch interface | 1 | $10.00 | $10.00 | https://robocraze.com/products/smartelex-xpt2046-2-8-inch-tft-lcd-resistive-touch-display-240-320?variant=48481353203936 |
| **4** | Custom PCB | Main circuit board for project assembly | 1 | $9.00 | $9.00 |  https://robu.in/|
| | | **Total Price:** | | | **$29.50** | | |

## Uniqueness

It has a special idle mode and menu in it.

It allows the users to connect to the wifi without editing the code.

## Guide
So for this project we would be using Kicad for designing the PCB.

## Schematic 

<img width="1389" height="874" alt="Screenshot 2026-10-29 182450" src="https://github.com/user-attachments/assets/f279b80f-44c7-4309-af2f-2fbab00b4996" />

The connections for the schematic are like this.

##### Symbols
For the esp 32 s3 we would be using the symbol of ESP32S3 Devkit C1.

For the TFT we will use Conn_01x14_Pin.

For the joystick we will use Conn_01x05_Pin.

## Footprint assignment

<img width="1259" height="713" alt="Screenshot 2026-10-29 183058" src="https://github.com/user-attachments/assets/33b6a808-4b65-4940-a7a3-d472b993cb91" />
The assignments are shown here.

## PCB and Routing 

<img width="991" height="644" alt="Screenshot 2026-10-29 183318" src="https://github.com/user-attachments/assets/350d8589-1108-4a6e-b5c2-904476475d20" />

The PCB is routed like this as shown here.

## 3D view

<img width="1725" height="931" alt="Screenshot 2026-10-29 183533" src="https://github.com/user-attachments/assets/048ec2a3-b52a-48f2-af32-6b3f6a855270" />


# Built for half life
