# Evidence Inbox

本目錄是 sanitized、repo-safe 的 evidence staging surface，供 ChatGPT 保存待 reconciliation 的 observation、provenance 與 measurement condition。

- 不具 execution authority，ordinary bootstrap 預設不載入。
- 不取代 `VALIDATION.md` 或其他 canonical source of truth；正式結論須經 reconciliation 後由 canonical owner 吸收。
- 只可寫入已去除 secrets、credentials、tokens、私人 endpoint、MAC、個資與其他敏感 raw material 的內容。
- 敏感 raw artifact 應留在 repository 外；必要時僅保存允許的 redacted metadata、digest、hash 或 pointer。
