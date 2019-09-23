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
    ```{"topic": "date_save", "method": "mutation", "data": {"date": "2019-08-12T17:00:17.613Z"}}```
* sensor_order_save 
    ```{"topic": "sensor_order_save", "method": "mutation", "data": {"names": ["sssssss", "sensor_2"]}}```
* clear_nvmemory
    ```{"topic": "clear_nvmemory", "method": "mutation"}```
* precondition_save for timer
    ```{"topic": "precondition_save", "method": "mutation", "data": {"index": 0, "timers": [[0, 3600], [7200, 8400]]}}```
* precondition_save for criteria
    ```{"topic": "precondition_save", "method": "mutation", "data": {"index": 0, "criteria": { "sensor": 2, "criteria": 77.33, "greater": false }}}```
* criteria_save 
    ```{ "topic": "criteria_save", "method": "mutation", "data": { "index": 7, "criteria": { "sensor": 2, "criteria": 77.33, "greater": false } } }```
* timer_save
   ```{"topic": "timer_save", "method": "mutation", "data": {"index": 0, "timers": [[0, 3600], [7200, 8400]]}}```

###Query
* Info ```{"topic": "info", "method": "query", "reqId": "1"}```
* Date ```{"topic": "date", "method": "query", "reqId": "16"}```
* sensor ```{"topic": "sensor", "method": "query"}```
* sensor_order ```{"topic": "sensor_order", "method": "query"}```
* precondition for timer ```{"topic": "precondition", "method": "query", "data": {"type": "timer", "index": 0}}```
* precondition for criteria ```{"topic": "precondition", "method": "query", "data": {"type": "criteria", "index": 0}}```
* criteria ```{"topic": "criteria", "method": "query", "data": {"index": 1}}```
* timer ```{"topic": "timer", "method": "query", "data": {"index": 0}}```
* nsensors ```{"topic": "nsensors", "method": "query"}```
* nsensors (with sensor = false) ```{"topic": "nsensors", "method": "query", "data": {"withSensor": false}}```
* nsensor ```{"topic": "nsensor", "method": "query", "data": {"index": 1}}```
* channel ```{"topic": "channel", "method": "query", "data": {"index": 1}}```