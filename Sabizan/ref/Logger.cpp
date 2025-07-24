#include "Logger.h"

namespace sl
{
  Logger::Logger()
  {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
  }

  Logger::~Logger()
  {
    FreeConsole();
  }

  void Logger::Info(std::string_view message, std::string_view file, int line, std::string_view func)
  {
    Write(LogLevel(LogLevel::Value::Info), message, file, line, func);
  }

  void Logger::Warn(std::string_view message, std::string_view file, int line, std::string_view func)
  {
    Write(LogLevel(LogLevel::Value::Warn), message, file, line, func);
  }

  void Logger::Error(std::string_view message, std::string_view file, int line, std::string_view func)
  {
    Write(LogLevel(LogLevel::Value::Error), message, file, line, func);
  }

  void Logger::Debug(std::string_view message, std::string_view file, int line, std::string_view func)
  {
    Write(LogLevel(LogLevel::Value::Debug), message, file, line, func);
  }

  void Logger::Write(const LogLevel& level, std::string_view message, std::string_view file, int line, std::string_view func)
  {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, level.ToColor());

    std::ostringstream oss;
    oss << "[" << GetTimestamp() << "] ";
    oss << "[" << level.ToString() << "] ";
    if (!file.empty()) {
      oss << "[" << ExtractFilename(file) << ":" << line << " " << func << "] ";
    }
    oss << message << std::endl;

    std::cout << oss.str();
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
  }

  std::string Logger::GetTimestamp() const
  {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto itt = system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &itt);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    return oss.str();
  }

  std::string Logger::ExtractFilename(std::string_view filepath) const
  {
    size_t pos = filepath.find_last_of("/\\");
    return (pos == std::string_view::npos) ? std::string(filepath) : std::string(filepath.substr(pos + 1));
  }
}
