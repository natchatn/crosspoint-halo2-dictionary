#include "DictionaryEngine.h"

#include <HalStorage.h>
#include <Logging.h>

#include <algorithm>

static constexpr char DICT_PATH[] = "/crosspoint/en_th.txt";
static constexpr int MAX_ITERATIONS = 64;

static bool readLine(FsFile& file, std::string& outLine) {
  outLine.clear();

  while (file.available()) {
    char c;

    if (file.read(&c, 1) != 1) {
      break;
    }

    if (c == '\n') {
      break;
    }

    if (c != '\r') {
      outLine += c;
    }
  }

  return !outLine.empty();
}

std::string lookupWord(const std::string& word) {
  LOG_INF("DICT", "LOOKUP START: %s", word.c_str());

  FsFile file;

  if (!Storage.openFileForRead("DICT", DICT_PATH, file)) {
    LOG_ERR("DICT", "FAILED TO OPEN FILE");

    return "DICT NOT FOUND";
  }

  uint32_t fileSize = file.size();

  LOG_INF("DICT", "FILE SIZE = %lu", fileSize);

  uint32_t low = 0;
  uint32_t high = fileSize;

  std::string line;

  int iterations = 0;

  while (low < high) {
    iterations++;

    if (iterations > MAX_ITERATIONS) {
      LOG_ERR("DICT", "MAX ITERATIONS REACHED");

      file.close();

      return "SEARCH TIMEOUT";
    }

    uint32_t mid = low + ((high - low) / 2);

    LOG_DBG("DICT", "ITER=%d low=%lu high=%lu mid=%lu", iterations, low, high, mid);

    if (mid >= fileSize) {
      LOG_ERR("DICT", "MID OUT OF RANGE");

      break;
    }

    file.seek(mid);

    if (mid != 0) {
      char c;

      uint32_t pos = mid;

      // ถอยหลังหา newline
      while (pos > 0) {
        file.seek(pos - 1);

        if (file.read(&c, 1) != 1) {
          break;
        }

        if (c == '\n') {
          break;
        }

        pos--;
      }

      file.seek(pos);
    }

    uint32_t lineStart = file.position();

    LOG_DBG("DICT", "LINE START = %lu", lineStart);

    if (!readLine(file, line)) {
      LOG_ERR("DICT", "FAILED READ LINE");

      break;
    }

    LOG_DBG("DICT", "LINE = %s", line.c_str());

    size_t sep = line.find('|');

    if (sep == std::string::npos) {
      LOG_ERR("DICT", "BROKEN LINE NO SEPARATOR");

      // ข้าม line พัง
      low = file.position();

      continue;
    }

    std::string key = line.substr(0, sep);

    LOG_DBG("DICT", "KEY = %s", key.c_str());

    int cmp = key.compare(word);

    LOG_DBG("DICT", "COMPARE = %d", cmp);

    if (cmp == 0) {
      LOG_INF("DICT", "FOUND!");

      std::string result = line.substr(sep + 1);

      file.close();

      return result;
    }

    if (cmp < 0) {
      uint32_t newLow = file.position();

      if (newLow <= low) {
        low++;

        LOG_ERR("DICT", "FORCE LOW++ -> %lu", low);

      } else {
        low = newLow;
      }

    } else {
      if (lineStart == 0) {
        LOG_ERR("DICT", "REACHED FILE START");

        break;
      }

      high = lineStart - 1;

      LOG_DBG("DICT", "NEW HIGH = %lu", high);
    }
  }

  LOG_ERR("DICT", "WORD NOT FOUND");

  file.close();

  return "NOT FOUND";
}