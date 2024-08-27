#include <core/application.h>

#include <format>
#include <iostream>

int main() {
  // NOTE: test function

  Application app;

  const char* cmd =
      "C:/Home/Soft/DB Browser for SQLite/DB Browser for SQLite.exe";
  if (!Start(cmd, &app)) {
    std::cerr << std::format("Failed to start the app - {}\n", cmd);
    return 1;
  }

  std::cout << app << "\n";

  Sleep(1000);

  Windows(&app);

  if (!Kill(&app)) {
    std::cerr << "Failed to kill the process" << "\n";
    return 1;
  }

  return 0;
}
