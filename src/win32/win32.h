#ifndef SRC_WIN32_WIN32_H_
#define SRC_WIN32_WIN32_H_

#include <windows.h>

#include <string>

std::string GetLastErrorAsString() noexcept;
bool CloseHwnd(HANDLE hwnd);
bool IsProcess64Bit(HANDLE p_hwnd) noexcept;

#endif  // SRC_WIN32_WIN32_H_
