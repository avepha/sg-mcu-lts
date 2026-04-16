# Phase 1 Binary Protocol Contract: `info.query` (`0x01`)

Phase 1 adds one binary request/response operation alongside the existing JSON API. JSON traffic is unchanged: frames that begin with `{` still use the legacy JSON parser. Binary traffic starts with the sync prefix `0xA5 0x5A` and uses the fixed frame described here.

## Runtime Contract

- **Backend timeout:** `250 ms` per request
- **Retry policy:** `1 retry only` when the backend sees timeout or transport silence
- **No retry:** explicit protocol failures such as `BAD_FRAME`, `CRC_FAIL`, `OVERSIZE`, `UNSUPPORTED_VERSION`, `UNKNOWN_OPERATION`, `DECODE_FAIL`, or `INTERNAL_ERROR`
- **Correlation:** backend sets `request_id`; firmware echoes the same little-endian `request_id` in the response

## Outer Frame Layout

All multi-byte integers are **little-endian**.

```text
[sync1:u8][sync2:u8][body_len:u16][body_bytes...][crc16:u16]
```

| Field | Size | Value / meaning |
| --- | ---: | --- |
| `sync1` | 1 byte | `0xA5` |
| `sync2` | 1 byte | `0x5A` |
| `body_len` | 2 bytes | Envelope + payload byte count |
| `body_bytes` | 8-128 bytes | Fixed envelope followed by typed payload |
| `crc16` | 2 bytes | CRC16 over `body_bytes` only |

## Envelope Layout

The `body_bytes` section always starts with this 8-byte envelope:

```text
[version:u8][message_type:u8][operation_id:u8][request_id:u16][status:u8][payload_len:u16][payload...]
```

| Offset | Field | Size | Notes |
| ---: | --- | ---: | --- |
| 0 | `version` | 1 | Phase 1 is `0x01` |
| 1 | `message_type` | 1 | `0x01` request, `0x02` response |
| 2 | `operation_id` | 1 | `0x01` = `info.query` |
| 3 | `request_id` | 2 | Client-selected correlation ID, echoed in response |
| 5 | `status` | 1 | Canonical status code |
| 6 | `payload_len` | 2 | Typed payload byte count |

## Status Codes

| Code | Name | Meaning | Retry? |
| ---: | --- | --- | --- |
| `0x00` | `OK` | Request decoded and handled successfully | No |
| `0x01` | `BAD_FRAME` | Invalid sync/body structure or malformed frame shape | No |
| `0x02` | `CRC_FAIL` | CRC16 mismatch | No |
| `0x03` | `OVERSIZE` | Declared body or payload exceeds configured maximums | No |
| `0x04` | `UNSUPPORTED_VERSION` | `version` is not `0x01` | No |
| `0x05` | `UNKNOWN_OPERATION` | `operation_id` is not implemented | No |
| `0x06` | `DECODE_FAIL` | Payload cannot be decoded for the declared operation | No |
| `0x07` | `INTERNAL_ERROR` | Firmware could not encode a valid response | No |

## Phase 1 Operation: `0x01` `info.query`

- **Operation ID:** `0x01`
- **Request payload:** empty by default for Phase 1
- **Response payload fields:** `version`, `model`, `device_environment`
- **Request ID semantics:** backend chooses any `u16` value; firmware echoes it unchanged so the backend can correlate outstanding requests

## Size Limits

| Limit | Value |
| --- | ---: |
| Envelope size | `8` bytes |
| Max payload size | `120` bytes |
| Max body size | `128` bytes |
| Max full frame size | `134` bytes |

## Backend Implementation Notes

1. Check the sync bytes `0xA5 0x5A`.
2. Read `body_len` as little-endian.
3. Verify CRC16 over the exact `body_bytes` region.
4. Read the fixed envelope before decoding payload bytes.
5. Treat `status != 0x00` as a canonical protocol failure; do not retry explicit failures.
6. Use the examples in `docs/protocol/examples/phase1-info.md` as copy-paste fixtures for encoder/decoder tests.
