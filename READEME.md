
##Usage
* Unit test
    ```pio test -f mutation -e node32s --verbose```
    
*  Upload
    ```pio run -t upload```
    
* monitor
    ```pio device monitor```


## Command List
* ###Mutation
1. date_save 
    ```{"topic": "date_save", "method": "mutation", "data": {"date": "2019-08-12T17:00:17.613Z"}}```
2. sensor_order_save 
    ```{"topic": "sensor_order_save", "method": "mutation", "data": {"names": ["sssssss", "sensor_2"]}}```
3. clear_nvmemory 
    ```{"topic": "clear_nvmemory", "method": "mutation"}```

* ###Query

1. Date ```{"topic": "date", "method": "query"}```    
2. sensor ```{"topic": "sensor", "method": "query"}```
3. sensor_order ```{"topic": "sensor_order", "method": "query"}```