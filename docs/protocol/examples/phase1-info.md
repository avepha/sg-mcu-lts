# Phase 1 `info.query` Wire Examples

All examples below use the real Phase 1 framing constants: sync `0xA5 0x5A`, operation `0x01`, little-endian lengths, and CRC16 over the frame body.

## Request Example

`request_id = 0x1234`, empty request payload.

```text
A5 5A 08 00  01 01 01 34 12 00 00 00  24 6A
```

Breakdown:

- `A5 5A` sync bytes
- `08 00` body length = 8 bytes
- `01` version
- `01` message type = request
- `01` operation = `info.query`
- `34 12` request_id = `0x1234`
- `00` status = `OK`
- `00 00` payload length = 0
- `24 6A` CRC16 over `01 01 01 34 12 00 00 00`

## Success Response Example

Response echoes `request_id = 0x1234` and returns:

- `version = "2.2.4"`
- `model = "v2"`
- `device_environment = DEVELOPMENT (0x02)`

```text
A5 5A 15 00  01 02 01 34 12 00 0D 00  0A 05 32 2E 32 2E 34 12 02 76 32 18 02  11 A9
```

Payload bytes:

- `0A 05 32 2E 32 2E 34` → field 1 `version = "2.2.4"`
- `12 02 76 32` → field 2 `model = "v2"`
- `18 02` → field 3 `device_environment = 0x02`

## Failure Response Example

Unknown operation request returns canonical status `UNKNOWN_OPERATION (0x05)` with no payload.

```text
A5 5A 08 00  01 02 FE 21 43 05 00 00  0E 5B
```

Breakdown:

- `01` version
- `02` message type = response
- `FE` unknown operation ID echoed from the rejected request
- `21 43` request_id = `0x4321`
- `05` status = `UNKNOWN_OPERATION`
- `00 00` payload length = 0

## Malformed CRC Example

This request has the correct body bytes but an intentionally corrupted CRC. Firmware should reject it with `CRC_FAIL (0x02)` and the backend should treat it as an explicit failure, not a retryable success.

```text
A5 5A 08 00  01 01 01 34 12 00 00 00  DB 6A
```
