#include <core/application.h>

#include <format>
#include <iostream>

int main() {
  // NOTE: test function

  Application app;

  const char* cmd = "C:/WINDOWS/system32/notepad.exe";
  if (!Start(cmd, &app)) {
    std::cerr << std::format("Failed to start the app - {}\n", cmd);
    return 1;
  }

  std::cout << app << "\n";

  Sleep(2000);

  if (!Kill(&app)) {
    std::cerr << "Failed to kill the process" << "\n";
    return 1;
  }

  return 0;
}
