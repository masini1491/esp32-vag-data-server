# ESP32 VAG Data Server

**Project status:** Architecture / Research stage

## Purpose

唯讀 vehicle data server，以 ESP32-S3 family 為平台，將 Vehicle CAN 的被動與診斷資料正規化為 `VehicleData`，供 BLE、Wi-Fi Web UI 與 Logger 使用。

## Initial research / validation target

Škoda Kamiq 2024 facelift（MQB-A0 family）。本專案不限定 Kamiq-only，且目前不宣稱已支援任何車型。

## Planned outputs

- BLE telemetry
- Wi-Fi Web UI
- Logger
- Future HUD client（獨立專案）

## Hardware status

- ESP32-S3 family
- Exact board: TBD
- CAN transceiver: TBD
- GPIO: TBD

## Read-only scope and safety boundary

未來 diagnostic access 僅允許必要的讀取服務，所有 diagnostic TX 必須經 `ReadOnlyGuard`。不包含 coding、adaptation、clear DTC、actuator control、flashing 或其他寫入/控制功能；Web、BLE、API 亦不得提供 arbitrary raw UDS TX bypass。

## Architecture

Vehicle CAN → Passive CAN / Diagnostic CAN → ISO-TP → OBD-II / UDS → VAG Data Layer → Vehicle Profile → VehicleData Store / Cache → BLE / Web / Logger。

詳見 [Architecture Freeze v0.1](docs/ARCHITECTURE.md)、[Read-only policy](docs/READ_ONLY_POLICY.md) 與 [Development roadmap](docs/DEVELOPMENT.md)。

## Current development status

本輪只完成 repository bootstrap、文件骨架與 Architecture Freeze v0.1；尚未實作 CAN、ISO-TP、OBD-II、UDS、BLE、Web server 或 firmware。

## Disclaimer

This project is not affiliated with Volkswagen AG, Škoda Auto, Ross-Tech, or the referenced upstream projects.
