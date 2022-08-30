#pragma once

#include "jot_token.hpp"

#include <unordered_map>
#include <vector>

enum class DiagnosticLevel { Warning, Error };

static std::unordered_map<DiagnosticLevel, const char *> diagnostic_level_literal = {
    {DiagnosticLevel::Warning, "Warning"},
    {DiagnosticLevel::Error, "Error"},
};

class JotDiagnostic {
  public:
    JotDiagnostic(TokenSpan location, std::string message, DiagnosticLevel level)
        : location(std::move(location)), message(std::move(message)), level(level) {}

    TokenSpan get_location() { return location; }

    std::string get_message() { return message; }

    DiagnosticLevel get_level() { return level; }

    const char *get_level_literal() { return diagnostic_level_literal[level]; }

  private:
    TokenSpan location;
    std::string message;
    DiagnosticLevel level;
};

class JotDiagnosticEngine {
  public:
    void report_diagnostics(DiagnosticLevel level);

    void add_diagnostic_error(TokenSpan location, std::string message);

    void add_diagnostic_warn(TokenSpan location, std::string message);

    int diagnostics_size();

  private:
    void report_diagnostic(JotDiagnostic &diagnostic);

    std::vector<JotDiagnostic> diagnostics;
};