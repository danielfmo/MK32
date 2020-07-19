# MK32 - Simple BLE keyboard for ESP32

This is a fork of the great work done by Galzai that aims for simplicity only, and for me to learn the esp-idf.
Please refer to the original project for more information, this page will be update as soon as I implement some of the WIP.

## Features

- Customizable layouts
- Macros
- Battery Monitoring
- Power Management
- Basic plugin support

## To-do

- Consult TODO.md for an up to date status.
- Refactor some bad coding habits (magic numbers etc, sorry I started this project when I had 0 experience!).
- Update esp-idf version to v4.1.
- Plugin API support(Implemented):  
  - Currently only one plugin can run at a time (to modify).
- Connection switching.
- Modify keymap via webserver:  
  - Functions for storing (and reading) AP data on flash complete.  
  - Starting web server from plugin complete.  
  - need to create proper web GUI for keymap modifications.  
- Wifi manager:
  - Add plugin for adding AP.  
  - Functions for storing (and reading) AP from flash done.  
