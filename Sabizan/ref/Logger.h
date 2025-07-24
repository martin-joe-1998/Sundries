#pragma once
#include <string>
#include <string_view>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <windows.h>

namespace sl
{
  struct LogLevel
  {
    enum class Value {
      Info,
      Warn,
      Error,
      Debug
    } value;

    constexpr LogLevel(Value v) : value(v) {}

    constexpr std::string_view ToString() const noexcept {
      switch (value) {
      case Value::Info:  return "INFO";
      case Value::Warn:  return "WARN";
      case Value::Error: return "ERROR";
      case Value::Debug: return "DEBUG";
      }
      return "UNKNOWN";
    }

    WORD ToColor() const noexcept {
      switch (value) {
      case Value::Info:  return FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      case Value::Warn:  return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      case Value::Error: return FOREGROUND_RED | FOREGROUND_INTENSITY;
      case Value::Debug: return FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
      }
      return FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
  };

  class Logger
  {
  public:
    static Logger& Instance() {
      static Logger instance;
      return instance;
    }

    template<typename... Args>
    void InfoMsg(Args&&... args) { Info(FormatLogMessage(std::forward<Args>(args)...), __FILE__, __LINE__, __func__); }

    template<typename... Args>
    void WarnMsg(Args&&... args) { Warn(FormatLogMessage(std::forward<Args>(args)...), __FILE__, __LINE__, __func__); }

    template<typename... Args>
    void ErrorMsg(Args&&... args) { Error(FormatLogMessage(std::forward<Args>(args)...), __FILE__, __LINE__, __func__); }

    template<typename... Args>
    void DebugMsg(Args&&... args) { Debug(FormatLogMessage(std::forward<Args>(args)...), __FILE__, __LINE__, __func__); }

    void Info(std::string_view message, std::string_view file, int line, std::string_view func);
    void Warn(std::string_view message, std::string_view file, int line, std::string_view func);
    void Error(std::string_view message, std::string_view file, int line, std::string_view func);
    void Debug(std::string_view message, std::string_view file, int line, std::string_view func);

  private:
    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void Write(const LogLevel& level, std::string_view message, std::string_view file, int line, std::string_view func);
    std::string GetTimestamp() const;
    std::string ExtractFilename(std::string_view filepath) const;

    template<typename... Args>
    std::string FormatLogMessage(Args&&... args)
    {
      std::ostringstream oss;
      (oss << ... << args);
      return oss.str();
    }
  };
}

#define LOG_INFO(...)  sl::Logger::Instance().InfoMsg(__VA_ARGS__)
#define LOG_WARN(...)  sl::Logger::Instance().WarnMsg(__VA_ARGS__)
#define LOG_ERROR(...) sl::Logger::Instance().ErrorMsg(__VA_ARGS__)
#define LOG_DEBUG(...) sl::Logger::Instance().DebugMsg(__VA_ARGS__)
