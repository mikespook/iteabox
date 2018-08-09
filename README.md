# iTeaBox
This is the repo for the ESP8266 based devices.

# Libraries

iTea libraries saved in `src/libraries/iTea/` with following classes.

 * iTeaConfigClass - Using EEPROM to store the configration information
 * iTeaHandlerClass - A just-runable state handler to let the device switch between the states
 * iTeaSetupClass - Soft AP mode setup panel
	- `setup-html` has the files for generating iTeaSetupClass WebUI.
 * iTeaWiFiClass - WiFi client
 * iTeaMQTTClass - MQTT client

# Devices

 * `delayedpump` - Water level trigered relay with MQTT notification
 * `petsfeeder` - Feed pets with `paw-controled` switch or MQTT remove control
 * `doorlocker` - TODO
 * `doorwatcher` - TODO

# Open Source - MIT Software License

See LICENSE.
