# Wi-Fi / Web UI Direction

未來 ESP32-S3 可建立 SoftAP。開發期預設 SSID 可為 `VAG-Data-Server`，可支援 open AP；架構保留 WPA2 擴充可能。

預計頁面：Realtime Dashboard、Vehicle information、ECU list、Read DTC、Measuring Values、Development Hardware / GPIO Settings。

Realtime UI 優先讀 `VehicleData Cache`，瀏覽器 refresh 不應直接重複 query ECU。開發階段 GPIO Settings 流程為 Web UI → validation → NVS → reboot → HardwareConfig；正式定案後可隱藏 UI、禁止 write API 與 NVS override，但不得移除內部 abstraction。
