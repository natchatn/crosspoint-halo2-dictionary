#include "DictionaryEngine.h"

#include <HalStorage.h>

static constexpr char DICT_PATH[] =
    "/crosspoint/en_th.txt";

static constexpr size_t MAX_BUF = 8192;

std::string lookupWord(const std::string& word) {

  if (!Storage.exists(DICT_PATH)) {
    return "DICT NOT FOUND";
  }

  HalFile file;

  if (!Storage.openFileForRead("DICT", DICT_PATH, file)) {
    return "OPEN ERROR";
  }

  std::string currentLine;

  while (file.available()) {

    char c = file.read();

    // skip CR
    if (c == '\r') {
      continue;
    }

    // line finished
    if (c == '\n') {

      size_t sep = currentLine.find('|');

      if (sep != std::string::npos) {

        std::string key =
            currentLine.substr(0, sep);

        std::string value =
            currentLine.substr(sep + 1);

        if (key == word) {
          file.close();
          return value;
        }
      }

      currentLine.clear();
    }
    else {
      currentLine += c;
    }
  }

  // handle last line without newline
  if (!currentLine.empty()) {

    size_t sep = currentLine.find('|');

    if (sep != std::string::npos) {

      std::string key =
          currentLine.substr(0, sep);

      std::string value =
          currentLine.substr(sep + 1);

      if (key == word) {
        file.close();
        return value;
      }
    }
  }

  file.close();

  return "NOT FOUND";
}