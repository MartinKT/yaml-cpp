#ifndef PARSER_H_62B23520_7C8E_11DE_8A39_0800200C9A66
#define PARSER_H_62B23520_7C8E_11DE_8A39_0800200C9A66

#if defined(_MSC_VER) ||                                            \
    (defined(__GNUC__) && (__GNUC__ == 3 && __GNUC_MINOR__ >= 4) || \
     (__GNUC__ >= 4))  // GCC supports "pragma once" correctly since 3.4
#pragma once
#endif

#include <ios>
#include <memory>

#include "yaml-cpp/dll.h"
#include "yaml-cpp/noncopyable.h"

namespace YAML {
class EventHandler;
class Node;
class Scanner;
struct Directives;
struct Token;

class YAML_CPP_API Parser : private noncopyable {
 public:
  Parser();
  Parser(std::istream& in, bool textEnabled = false);
  ~Parser();

  operator bool() const;

  void Load(std::istream& in, bool textEnabled = false);
  bool HandleNextDocument(EventHandler& eventHandler);

  void PrintTokens(std::ostream& out);
  std::string GetText() const;

 private:
  void ParseDirectives();
  void HandleDirective(const Token& token);
  void HandleYamlDirective(const Token& token);
  void HandleTagDirective(const Token& token);

 private:
  std::unique_ptr<Scanner> m_pScanner;
  std::unique_ptr<Directives> m_pDirectives;
};
}

#endif  // PARSER_H_62B23520_7C8E_11DE_8A39_0800200C9A66
