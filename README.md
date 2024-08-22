# space-misko


# Introduction

Space Misko is a video game inspired by the classic "Space Invaders," created as a project for the subject Fundamentals of Microprocessor Electronics at the Faculty of Electrical Engineering, University of Ljubljana. This game is designed to run on a student microcontroller development board Misko 3 based on the STM32G474QET.


 ![splashfinal (2)](https://github.com/user-attachments/assets/8a95994e-bc27-400b-a7a1-d28780f15673)

# Required specifications

Misko 3 specifications:
2.8" TFT LCD color display with a resolution of 320 x 240 px
6 buttons
2-axis joystick
8 LED indicators
Analog and digital interfaces (I2C, SPI, UART, GPIOs, Arduino DUE compatible connectors)
USB-C interface for power and communication (virtual COM port and native USB)
ST-LINK V2.1 through the same USB-C port for programming and debugging
Memory: 512 kB Flash and 96 kB SRAM
Microcontroller frequency: 170 MHz


# Gameplay

In Space Misko, your goal is to destroy as many enemy targets (alien invaders) as possible. You control a rocket using a joystick, moving it left and right across the screen. The targets spawn randomly on the screen, with varying speeds. Each time a target hits an edge of the screen, it moves downwards and increases in speed. To destroy them, press the button to fire a bullet. Each time you're hit, you lose a life point. The game ends when all life points are depleted.


You can watch a demo of the game here: https://www.youtube.com/watch?v=1y_KvjgHxaA
