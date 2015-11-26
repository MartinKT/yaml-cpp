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

int GetConsoleWidth() {
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

void ElegantErrorOutput(Exception &exception, Parser &parser) {
  std::string text = parser.GetText();
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

Node Load(const std::string& input, bool managed) {
  std::stringstream stream(input);
  return Load(stream, managed);
}

Node Load(const char* input, bool managed) {
  std::stringstream stream(input);
  return Load(stream, managed);
}

Node Load(std::istream& input, bool managed) {
  Parser parser;

  try {
    parser.Load(input, managed);
    NodeBuilder builder;
    if (!parser.HandleNextDocument(builder))
      return Node();

    return builder.Root();
  } catch (Exception e) {
    if (managed) {
      ElegantErrorOutput(e, parser);
    }

    throw e;
  }
}

Node LoadFile(const std::string& filename, bool managed) {
  std::ifstream fin(filename.c_str());
  if (!fin)
    throw BadFile();
  return Load(fin, managed);
}

std::vector<Node> LoadAll(const std::string& input, bool managed) {
  std::stringstream stream(input);
  return LoadAll(stream, managed);
}

std::vector<Node> LoadAll(const char* input, bool managed) {
  std::stringstream stream(input);
  return LoadAll(stream, managed);
}

std::vector<Node> LoadAll(std::istream& input, bool managed) {
  std::vector<Node> docs;

  Parser parser(input, managed);

  try{
    while (1) {
      NodeBuilder builder;
      if (!parser.HandleNextDocument(builder))
        break;
      docs.push_back(builder.Root());
    }
  } catch (Exception e) {
    if (managed) {
      ElegantErrorOutput(e, parser);
    }

    throw e;
  }

  return docs;
}

std::vector<Node> LoadAllFromFile(const std::string& filename, bool managed) {
  std::ifstream fin(filename.c_str());
  if (!fin)
    throw BadFile();
  return LoadAll(fin, managed);
}
}
