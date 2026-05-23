#include "DictionaryEngine.h"

#include <HalStorage.h>

static constexpr char DICT_PATH[] =
    "/crosspoint/dict/en_th.txt";

static constexpr size_t MAX_BUF = 8192;

std::string lookupWord(const std::string& word) {

  if (!Storage.exists(DICT_PATH)) {
    return "DICT NOT FOUND";
  }

  auto* buf = static_cast<char*>(malloc(MAX_BUF));

  if (!buf) {
    return "MEM ERROR";
  }

  const size_t bytesRead =
      Storage.readFileToBuffer(
          DICT_PATH,
          buf,
          MAX_BUF);

  std::string result = "NOT FOUND";

  size_t lineStart = 0;

  for (size_t i = 0; i <= bytesRead; ++i) {

    if (i == bytesRead ||
        buf[i] == '\n' ||
        buf[i] == '\r') {

      if (i > lineStart) {

        std::string line(
            buf + lineStart,
            i - lineStart);

        size_t sep = line.find('|');

        if (sep != std::string::npos) {

          std::string key =
              line.substr(0, sep);

          std::string value =
              line.substr(sep + 1);

          if (key == word) {

            result = value;
            break;
          }
        }
      }

      lineStart = i + 1;
    }
  }

  free(buf);

  return result;
}