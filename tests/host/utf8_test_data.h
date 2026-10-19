#pragma once

namespace vag_data::test::utf8_cases {

// Explicit bytes keep the malformed corpus independent of source-file encoding.
inline constexpr const char* kMalformed[] = {
    "\x80", "\xBF", "\xC0\x80", "\xC1\xBF", "\xC2",
    "\xC2" "A", "\xDF\x7F", "\xE0", "\xE0\xA0",
    "\xE0\x80\x80", "\xE0\x9F\xBF", "\xE1" "A" "\x80",
    "\xE1\x80" "A", "\xED\xA0\x80", "\xED\xBF\xBF",
    "\xF0", "\xF0\x90", "\xF0\x90\x80", "\xF0\x80\x80\x80",
    "\xF0\x8F\xBF\xBF", "\xF1" "A" "\x80\x80",
    "\xF1\x80" "A" "\x80", "\xF1\x80\x80" "A",
    "\xF4\x90\x80\x80", "\xF5\x80\x80\x80",
    "\xF8\x88\x80\x80\x80", "\xFC\x84\x80\x80\x80\x80",
    "\xFE", "\xFF", "ok\x80",
};

inline constexpr const char* kWellFormed[] = {
    "", "ASCII", "\x01\b\f\n\r\t\x1F\x7F", "\xC2\x80", "\xDF\xBF",
    "\xE0\xA0\x80", "\xED\x9F\xBF", "\xEE\x80\x80", "\xEF\xBF\xBF",
    "\xF0\x90\x80\x80", "\xF4\x8F\xBF\xBF",
    "\xC3\xA9", "e\xCC\x81",
};

inline constexpr char kSignalId[] = u8"vehicle.溫度";
inline constexpr char kUnit[] = u8"°C";
inline constexpr char kText[] = u8"é溫🚗";

}  // namespace vag_data::test::utf8_cases
