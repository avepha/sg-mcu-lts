# Unit test
    pio test -f mutation -e node32s --verbose
    
# Upload
    pio run -t upload
    
# monitor
    pio device monitor

# Command
```
{"topic": "date", "method": "query"}
{"topic": "sensor", "method": "query"}
{"topic": "sensor_order", "method": "query"}

{"topic": "date_save", "method": "mutation", "data": {"date": "2019-08-12T17:00:17.613Z"}}
{"topic": "sensor_order_save", "method": "mutation", "data": {"names": ["sssssss", "sensor_2"]}}
{"topic": "clear_memory", "method": "mutation"}

```

