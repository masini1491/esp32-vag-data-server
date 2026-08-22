# Vehicle Profile

Vehicle Profile 是將車型/平台差異隔離於 protocol 與 application logic 之外的資料與規則層。

初始 research / validation target 為 Škoda Kamiq 2024 facelift（MQB-A0 family），但架構不得寫成 Kamiq-only。實際 ECU、DID、scaling、CAN route 與 gateway access 均待實車驗證。
