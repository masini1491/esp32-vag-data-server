# BLE Protocol Direction

本輪只凍結方向，不定案 packet format。

未來預計提供 `VAG Vehicle Data Service`，包含 Realtime Telemetry、Diagnostic Request、Diagnostic Response、Server Status。HUD / App / PC clients 應接收 normalized `VehicleData`，而非以 raw CAN / raw UDS 作為一般即時資料來源。Realtime Telemetry 應考慮 binary compact packet，避免一個 signal 一個 characteristic。
