#include "yaml-cpp/node/parse.h"

#include <fstream>
#include <sstream>

#include <colors/colors.h>

#include "yaml-cpp/node/node.h"
#include "yaml-cpp/node/impl.h"
#include "yaml-cpp/parser.h"
#include "nodebuilder.h"

#include <iostream>
#include <algorithm>

#ifdef _WIN32
#include <Windows.h>
#else
#include <ioctl.h>
#endif

#define LINE_MAKER_SIZE 10

namespace YAML {

static int GetConsoleWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
#endif
    int cols = 80;
    int lines = 24;

#ifdef _WIN32
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    lines = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#elif defined(TIOCGSIZE)
    struct ttysize ts;
    ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
    cols = ts.ts_cols;
    lines = ts.ts_lines;
#elif defined(TIOCGWINSZ)
    struct winsize ts;
    ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
    cols = ts.ws_col;
    lines = ts.ws_row;
#endif /* TIOCGSIZE */

    return cols;
}

void Loader::ElegantErrorOutput(Exception &exception) {
  std::string text = m_parser->GetText();
  int consoleWidth = GetConsoleWidth();
  int maxContextSize = consoleWidth - LINE_MAKER_SIZE - 5;

  std::string errorMark;
  errorMark += colors::red("error").bold();
  std::cerr << "[line "<< exception.mark.line + 1
    << ", column " << exception.mark.column + 1 << "] "
    << errorMark << ": " << exception.msg << std::endl;

  // Find the first element of that line. Unless it is too faraway.
  int rpos = exception.mark.pos, actualLines = 1;
  for (int i = 0; i < maxContextSize; i++) {
    rpos = exception.mark.pos - i;
    if (rpos <= 0) break;

    if (text[rpos] == '\n') {
      if(actualLines == 1) {
        // Nothing is here, it is because of it points to an empty line.
        actualLines++;
        continue; // continue to next line.
      }
      rpos++;
      break;
    }
  }

  std::string context;
  if (rpos > 0 && text[rpos - 1] != '\n')
    context += " ...";

  for (int j = rpos, columnCount = 0, linesCount = 1; j < text.size(); j++) {
    if (text[j] == '\n'){
      columnCount = 0;
      linesCount++;
    } else
      columnCount++;

    if (linesCount > actualLines) {
      break;
    }
    
    if (columnCount >= consoleWidth - LINE_MAKER_SIZE - 1) {
      context += "...";
      break;
    }

    context += text[j];
  }

  std::cerr << context << std::endl;

  int spaceNumbers = (std::min)(exception.mark.column, maxContextSize + 3);
  std::string posMarker(spaceNumbers, ' ');
  posMarker += colors::green("^");
  posMarker += colors::green(std::string(LINE_MAKER_SIZE, '~'));
  std::cerr << posMarker << std::endl;
}

Loader::Loader(bool textEnabled) : m_textEnabled(textEnabled),
    m_parser(new Parser())
{
}

Node Loader::Load(const std::string& input) {
  std::stringstream stream(input);
  return Load(stream);
}

Node Loader::Load(const char* input) {
  std::stringstream stream(input);
  return Load(stream);
}

Node Loader::Load(std::istream& input) {
    m_parser->Load(input, m_textEnabled);
    NodeBuilder builder;
    if (!m_parser->HandleNextDocument(builder))
        return Node();

    return builder.Root();
}

Node Loader::LoadFile(const std::string& filename) {
  std::ifstream fin(filename.c_str());
  if (!fin)
    throw BadFile();
  return Load(fin);
}

std::vector<Node> Loader::LoadAll(const std::string& input) {
  std::stringstream stream(input);
  return LoadAll(stream);
}

std::vector<Node> Loader::LoadAll(const char* input) {
  std::stringstream stream(input);
  return LoadAll(stream);
}

std::vector<Node> Loader::LoadAll(std::istream& input) {
    std::vector<Node> docs;
    m_parser->Load(input, m_textEnabled);

    while (1) {
        NodeBuilder builder;
        if (!m_parser->HandleNextDocument(builder))
        break;
        docs.push_back(builder.Root());
    }

    return docs;
}

std::vector<Node> Loader::LoadAllFromFile(const std::string& filename) {
  std::ifstream fin(filename.c_str());
  if (!fin)
    throw BadFile();
  return LoadAll(fin);
}
}
