## Usage
* Unit test
    ```pio test -f mutation -e node32s --verbose```
    
*  Upload
    ```pio run -t upload```
    
* monitor
    ```pio device monitor```

## Command List
###Mutation
* date_save
```json
{"topic": "date_save", "method": "mutation", "data": {"date": "2019-08-12T17:00:17.6\13Z"}}
```
* sensor_order_save 
```json
{"topic": "sensor_order_save", "method": "mutation", "data": {"names": ["sssssss", "sensor_2"]}}
```
* clear_nvmemory
```json
{"topic": "clear_nvmemory", "method": "mutation"}
```
* precondition_save for timer
```json
{"topic": "precondition_save", "method": "mutation", "data": {"index": 0, "timers": [[0, 3600], [7200, 8400]]}}
```
* precondition_save for criteria
```json
{"topic": "precondition_save", "method": "mutation", "data": {"index": 0, "criteria": { "sensor": 2, "criteria": 77.33, "greater": false }}}
```
* criteria_save 
```json
{ "topic": "criteria_save", "method": "mutation", "data": { "index": 7, "criteria": { "sensor": 2, "criteria": 77.33, "greater": false } } }
```
* timer_save
```json
{"topic": "timer_save", "method": "mutation", "data": {"index": 1, "timers": [ [0, 1440] ]}}
```
* channel_save
```json
{ "topic": "channel_save", "method": "mutation", "data": { "index": 1, "control": { "type": "criteria", "value": 0 }, "preconditions": [] } }
```
* channel_activate
```json
{ "topic": "channel_activate", "method": "mutation", "data": { "index": 1,  "isActive": true}}
```

###Query
* Info 
```json
{"topic": "info", "method": "query", "reqId": "1"}
```
* Date 
```json
{"topic": "date", "method": "query", "reqId": "16"}
```
* sensor 
```json
{"topic": "sensor", "method": "query"}
```
* sensor_order 
```json
{"topic": "sensor_order", "method": "query"}
```
* precondition for timer 
```json
{"topic": "precondition", "method": "query", "data": {"type": "timer", "index": 0}}
```
* precondition for criteria 
```json
{"topic": "precondition", "method": "query", "data": {"type": "criteria", "index": 0}}
```
* criteria 
```json
{"topic": "criteria", "method": "query", "data": {"index": 1}}
```
* timer 
```json
{"topic": "timer", "method": "query", "data": {"index": 0}}
```w
* nsensors 
```json
{"topic": "nsensors", "method": "query"}
```
* nsensors (with sensor = false) 
```json
{"topic": "nsensors", "method": "query", "data": {"withSensor": false}}
```
* nsensor 
```json
{"topic": "nsensor", "method": "query", "data": {"index": 1}}
```
* channel 
```json
{"topic": "channel", "method": "query", "data": {"index": 1}}
```
* gpio 
```json
{"topic": "gpio", "method": "query"}
```
* channel_state 
```json
{"topic": "channel_state", "method": "query", "data":  {"index":  1}}
```

