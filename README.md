# esp8266-sdm-energymeter
This is a online energy meter with rest api based on esp8266 and SDM energy meter

## Hardware
The following hardware is required:
```
- D1 mini (ESP8266)
- MAX485
- SDM energy meter
```

Connection:
![alt text](https://github.com/datjan/esp8266-sdm-energymeter/blob/main/Schaltplan.png?raw=true)

## Development
This sketch is for following development environment
```
Arduino
```

Following libraries are required
```
https://github.com/datjan/SDM_Energy_Meter (fork from reaper7/SDM_Energy_Meter:master)
https://github.com/me-no-dev/ESPAsyncTCP
https://github.com/me-no-dev/ESPAsyncWebServer
```

## Setup
Setup wifi in esp8266-sdm-energymeter.ino:
```
const char* wifi_ssid = "xxx";
const char* wifi_password = "xxx";
```

Setup values from modbus in esp8266-sdm-energymeter.ino:
```
//SDM72
volatile sdm_struct sdmarr[] = {  
  {0.00, SDM_AVERAGE_L_TO_N_VOLTS, "001_SDM_AVERAGE_L_TO_N_VOLTS", "V"},                               
  {0.00, SDM_SUM_LINE_CURRENT, "001_SDM_SUM_LINE_CURRENT", "A"},                                             
  {0.00, SDM_TOTAL_SYSTEM_POWER, "001_SDM_TOTAL_SYSTEM_POWER", "W"},                                            
  {0.00, SDM_TOTAL_SYSTEM_POWER_FACTOR, "001_SDM_TOTAL_SYSTEM_POWER_FACTOR", "PF"},                                       
  {0.00, SDM_FREQUENCY, "001_SDM_FREQUENCY", "Hz"},
  {0.00, SDM_TOTAL_ACTIVE_ENERGY, "001_SDM_TOTAL_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_IMPORT_ACTIVE_ENERGY, "001_SDM_IMPORT_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_EXPORT_ACTIVE_ENERGY, "001_SDM_EXPORT_ACTIVE_ENERGY", "kWh"},
  {0.00, SDM_PHASE_1_POWER, "001_SDM_PHASE_1_POWER", "W"},
  {0.00, SDM_PHASE_2_POWER, "001_SDM_PHASE_2_POWER", "W"},
  {0.00, SDM_PHASE_3_POWER, "001_SDM_PHASE_3_POWER", "W"}                                                  
};
```

## Upload to device
Following files needs to be uploaded to the ESP8266 (D1 mini)
```
esp8266-sdm-energymeter.ino
index_page.h
```

## Show info
Access webpage on http://<IPADDR>
Access rest api on GET http://<IPADDR>/rest
