
##Usage
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
* precondition_save 
    ```{"topic": "precondition_save", "method": "mutation", "data": {"index": 0, "timers": [[0, 3600], [7200, 8400]]}}```
* criteria_save 
    ```{ "topic": "criteria_save", "method": "mutation", "data": { "index": 7, "criteria": { "sensor": 2, "criteria": 77.33, "greater": false } } }```


###Query
* Date ```{"topic": "date", "method": "query"}```    
* sensor ```{"topic": "sensor", "method": "query"}```
* sensor_order ```{"topic": "sensor_order", "method": "query"}```
* precondition ```{"topic": "precondition", "method": "query", "data": {"type": "timer", "index": 0}}```
* criteria ```{"topic": "criteria", "method": "query", "data": {"index": 1}}```