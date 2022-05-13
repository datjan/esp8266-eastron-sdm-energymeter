# esp8266-sdm-energymeter
This is a online information server with rest api based on esp8266 and Eastron SDM energy meter

## Hardware
The following hardware is required:
```
- D1 mini (ESP8266)
- MAX485
- SDM energy meter
```

Connection:

D1 mini -> MAX485
```
3 / RX -> RO
1 / TX -> DI
5 / D1 -> DE + RE
3V3 -> VCC 
GND -> GND 
```

D1 mini -> Netzteil (5V)
```
5V -> + 
GND -> - 
```

MAX485 -> Modbus energy meter (like SDM72)
```
A -> A 
B -> B 
```

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


Setup modbus baud rate in esp8266-sdm-energymeter.ino:
```
SDM sdm(swSerSDM, 9600, 5, SWSERIAL_8N1, 3, 1);  
```


Setup values from modbus in esp8266-sdm-energymeter.ino:

Example: {0.00, [REGISTER NAME],[CUSTOM NAME],[UNIT]}
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
The register names can be found in library "SDM_Energy_Meter" in file "SDM.h".

## Upload to device
Following files needs to be uploaded to the ESP8266 (D1 mini)
```
esp8266-sdm-energymeter.ino
index_page.h
```

## Show info
Access webpage on http://[IPADDR]
![alt text](https://github.com/datjan/esp8266-sdm-energymeter/blob/main/img_webpage.png?raw=true)
  
  
Access rest api on GET http://[IPADDR]/rest
![alt text](https://github.com/datjan/esp8266-sdm-energymeter/blob/main/img_restapi.png?raw=true)
  
