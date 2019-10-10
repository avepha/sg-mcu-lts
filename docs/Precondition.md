### Mutation
* Precondition criteria save
```json
{
  "topic": "precondition_criteria_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "sensor": 1,
    "criteria": 80.0,
    "greater": false
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