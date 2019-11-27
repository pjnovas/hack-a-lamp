# hack-a-lamp
> this is a work in progress

This is my attemp to hack my little lamp into an IoT device so I can switch it on/off from my local wifi

### Notes
* An MQTT Broker is needed for this to work
* An MQTT Client is also needed to publish
* Schematics soon

### Materials
* ESP-01s
* ESP-01s Relay Module
* PSU 220v > 5v
* The lamp to destroy

### How to
1. Set your config.h
2. Upload hack-a-lamp.ino
3. Set up a MQTT client 
4. Publish under topic: `lamp/light` on of these:
    * `0`: Switch off light
    * `1`: Switch on light
