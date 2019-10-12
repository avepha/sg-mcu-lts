### Mutation
* Criteria
```json
{
	"topic": "criteria",
	"method": "query",
	"data": {
		"index": 1
	}
}
```
```json
{
  "topic": "criteria_save",
  "method": "mutation",
  "data": {
    "index": 1,
    "criteria": {
      "sensor": 1,
      "criteria": 80.0,
      "greater": false,
      "timing": {
        "working": 10,
        "waiting": 10
      }
    }
  }
}
```

*Range
```json
{
	"topic": "range",
	"method": "query",
	"data": {
		"index": 1
	}
}
```
```json
{
	"topic": "range_save",
	"method": "mutation",
	"data": {
		"index": 1,
		"range": {
            "sensor": 1,
            "upper": 30.5,
            "lower": 25.3,
            "greater": false,
			"timing": {
				"working": 10,
				"waiting": 10
			}
		}
	}
}
```