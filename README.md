# MyoMobile
Codebase for Mylo the MyoMobile. An EMG-Controlled Robot, intended for use on the anterior tibialis muscle of both legs, for children who are undergoing lower-limb physical therapy.

## Description
* **mylo_eye_blink** - This file includes the control to display and animate the eyes on the ESP32 display screen.
* **mylo_myomobile_emg** - This file includes the emg data collection as well as processing from the myoware muscle sensor to the ESP32 board for wireless transfer.
* **mylo_myomobile_emg_extra** - This file includes similar code to `mylo_myomobile_emg`, with the additional feature to dynamically detect baseline and maximum emg amplitudes.
* **mylo_motor_control** - This file includes the control to the motors on the built car kit from the jetson nano.

[![Mylo GIF](https://github.com/Detadja/MyoMobile/blob/main/media/Blink_Car.gif)](https://github.com/Detadja)

## Components
* MyoWare Muscle Sensor 2.0
* ESP32 microcontroller board
* NVIDIA Jetson Nano
* Robot Car Kit
* ESP32-3248S035 Display Screen

## Setup
Construct the robot car kit with two motors connected to the two back wheels.

#### mylo_myomobile_emg or mylo_myomobile_emg_extra
* Attach both Myoware sensors to respective anterior tibialis muscles on both legs (the muscle that bulges out when you lift your foot).
* Connect sensors to power, ground and analog data on the ESP32 board.

#### mylo_eye_blink and mylo_motor_control
* Once the car is built, edit the code on the SD card of the Jetson for motor control.
* Connect the ESP32 display screen to the Jetson and edit the ESP32 display board code to display Mylo's eyes.
* Connect a powerbank (preferrably one with large power capacity).

Turn on Mylo, and activate emg sensing to start.

## To Run
Turn on Mylo, and activate emg sensing to start (type in 'start' in the Arduino IDE).

## Demo
Click to watch.
[![Video Demo 1 Link](https://github.com/Detadja/MyoMobile/blob/main/media/Beep_Boop_Up_Down_AHHH.mp4)](https://youtu.be/cPQCvYvUF9M "Mylo Demo 1")
[![Video Demo 2 Link](https://github.com/Detadja/MyoMobile/blob/main/media/IMG4202.mov.mp4)](https://youtu.be/iq79IugJJkw "Mylo Demo 2")

## Members
* Denise Tanumihardja
* Navya Challa
* James Wilson
* Eunsol Lee
* Brice Liu

[![Group Pic Here](https://github.com/Detadja/MyoMobile/blob/main/media/IMG20250517175116.jpg)](https://github.com/Detadja)
