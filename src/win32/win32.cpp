#include <win32/win32.h>
#include <windows.h>

#include <format>
#include <iostream>

std::string GetLastErrorAsString() noexcept {
  char buf[256];
  size_t size = FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
      GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf,
      (sizeof(buf) / sizeof(wchar_t)), NULL);
  std::string message(buf, size);

  return message;
}

bool CloseHwnd(HANDLE hwnd) {
  if (hwnd == NULL) {
    std::cerr << "Failed to close handle. Handle is NULL\n";
    return false;
  }
  if (CloseHandle(hwnd) == 0) {
    std::cerr << std::format("Failed to close handle. Error: {}\n",
                             GetLastErrorAsString());
    return false;
  }
  return true;
}

bool IsProcess64Bit(HANDLE p_hwnd) noexcept {
  BOOL is_wow64 = FALSE;

  if (IsWow64Process(p_hwnd, &is_wow64) == 0) {
    std::cerr << std::format(
        "Failed to determine process architecture. Error: {}\n",
        GetLastErrorAsString());
    return false;
  }

  if (is_wow64 != 0) {
    return false;
  }

  SYSTEM_INFO si;
  GetNativeSystemInfo(&si);

  return si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL;
}
