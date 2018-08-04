# iTeaBox
This is the repo for my ESP8266 based devices.

# Libraries

iTea libraries saved in `src/libraries/iTea/` with following classes.

 * iTeaConfigClass - Using EEPROM to store the configration information
 * iTeaHandlerClass - A just-runable state handler to let the device switch between the states
 * iTeaSetupClass - Soft AP mode setup panel
 * iTeaWiFi - WiFi client

 `setup-html` has the files for generating iTeaSetupClass WebUI.

# Devices

`delayedpump` - Water level trigered relay with MQTT notification
`catfeeder` - TODO
`doorlocker` - TODO
`doorwatcher` - TODO

#Open Source - MIT Software License

See LICENSE.
