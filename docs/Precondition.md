### Mutation
* Precondition criteria save
```json
{
  "topic": "precondition_criteria_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "criteria": {
      "sensor": 1,
      "criteria": 80.5,
      "greater": false
    }
  }
}
```

* Precondition range save
```json
{
  "topic": "precondition_range_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "range": {
      "sensor": 1,
      "upper": 30.0,
      "lower": 25.0,
      "greater": false
    }
  }
}
```

* Precondition timer save
```json
{
  "topic": "precondition_timer_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "timers": [[0, 1440]]
  }
}
```