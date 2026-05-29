#include "DictionaryEngine.h"

#include <HalStorage.h>
#include <Logging.h>

#include <algorithm>

static constexpr char DICT_PATH[] = "/crosspoint/en_th.txt";
static constexpr int MAX_ITERATIONS = 64;

// Forward declaration
std::string lookupWord(const std::string& word);
std::string lookupWordDirect(const std::string& word); // ← NEW: lookup, no stem

static bool readLine(FsFile& file, std::string& outLine) {
  outLine.clear();
  while (file.available()) {
    char c;
    if (file.read(&c, 1) != 1) break;
    if (c == '\n') break;
    if (c != '\r') outLine += c;
  }
  return !outLine.empty();
}

static bool isConsonant(char c) {
  c = std::tolower(static_cast<unsigned char>(c));
  return c != 'a' && c != 'e' && c != 'i' && c != 'o' && c != 'u';
}

std::string stemWord(const std::string& word) {
  if (word.length() <= 3) return word;

  // --- PLURAL / NOUN FORMS ---

  // stories -> story
  if (word.length() > 4 && word.ends_with("ies")) {
    return word.substr(0, word.length() - 3) + "y";
  }

  // wolves -> wolf, shelves -> shelf, knives -> knife
  // FIX: cut "ves" try both +f and +fe
  if (word.length() > 4 && word.ends_with("ves")) {
    std::string base = word.substr(0, word.length() - 3);
    std::string withFe = base + "fe";
    // knife -> knives: base="kni" withFe="knife" ✅
    if (lookupWordDirect(withFe) != "NOT FOUND") return withFe;
    // wolf -> wolves: base="wol" + "f" = "wolf" ✅
    return base + "f";
  }

  // --- PAST TENSE ---

  // walked -> walk, stopped -> stop, hoped -> hope, danced -> dance
  if (word.length() > 4 && word.ends_with("ed")) {
    std::string base = word.substr(0, word.length() - 2);
    bool removedDouble = false;
    if (base.length() >= 2) {
      char last = base.back();
      char prev = base[base.length() - 2];
      if (last == prev && isConsonant(last)) {
        base.pop_back();
        removedDouble = true;
      }
    }
    if (removedDouble) return base;
    std::string withE = base + "e";
    return lookupWordDirect(withE) != "NOT FOUND" ? withE : base;
  }

  // --- PRESENT PARTICIPLE ---

  // walking -> walk, running -> run, making -> make
  if (word.length() > 5 && word.ends_with("ing")) {
    std::string base = word.substr(0, word.length() - 3);
    bool removedDouble = false;
    if (base.length() >= 2) {
      char last = base.back();
      char prev = base[base.length() - 2];
      if (last == prev && isConsonant(last)) {
        base.pop_back();
        removedDouble = true;
      }
    }
    if (removedDouble) return base;
    std::string withE = base + "e";
    return lookupWordDirect(withE) != "NOT FOUND" ? withE : base;
  }

  // --- PLURAL -ES / -S ---

  // FIX: names -> name
  // foxes -> fox, classes -> class, dishes -> dish, names -> name
  if (word.length() > 4 && word.ends_with("es")) {
    std::string base = word.substr(0, word.length() - 2);  // strip "es"
    std::string withE = base + "e";                         // try restore "e"
    // "names" -> base="nam", withE="name" → lookup "name" ✅
    // "foxes" -> base="fox", withE="foxe" → not found → return "fox" ✅
    if (lookupWordDirect(withE) != "NOT FOUND") return withE;
    return base;
  }

  // books -> book
  if (word.length() > 3 && word.ends_with("s")) {
    return word.substr(0, word.length() - 1);
  }

  return word;
}

// ── INTERNAL: lookup directly, no stem fallback (prevent mutual recursion) ──
std::string lookupWordDirect(const std::string& word) {
  FsFile file;
  if (!Storage.openFileForRead("DICT", DICT_PATH, file)) {
    return "DICT NOT FOUND";
  }

  uint32_t fileSize = file.size();
  uint32_t low = 0;
  uint32_t high = fileSize;
  std::string line;
  int iterations = 0;

  while (low < high) {
    if (++iterations > MAX_ITERATIONS) {
      file.close();
      return "NOT FOUND";
    }

    uint32_t mid = low + ((high - low) / 2);
    if (mid >= fileSize) break;

    file.seek(mid);

    if (mid != 0) {
      char c;
      uint32_t pos = mid;
      while (pos > 0) {
        file.seek(pos - 1);
        if (file.read(&c, 1) != 1) break;
        if (c == '\n') break;
        pos--;
      }
      file.seek(pos);
    }

    uint32_t lineStart = file.position();
    if (!readLine(file, line)) break;

    size_t sep = line.find('|');
    if (sep == std::string::npos) {
      low = file.position();
      continue;
    }

    std::string key = line.substr(0, sep);
    int cmp = key.compare(word);

    if (cmp == 0) {
      std::string result = line.substr(sep + 1);
      file.close();
      return result;
    }

    if (cmp < 0) {
      uint32_t newLow = file.position();
      low = (newLow <= low) ? low + 1 : newLow;
    } else {
      if (lineStart == 0) break;
      high = lineStart;
    }
  }

  file.close();
  return "NOT FOUND";
}

// ── PUBLIC: lookup with stem fallback ──
std::string lookupWord(const std::string& word) {
  LOG_INF("DICT", "LOOKUP START: %s", word.c_str());

  std::string result = lookupWordDirect(word);

  if (result != "NOT FOUND" && result != "DICT NOT FOUND") {
    LOG_INF("DICT", "FOUND DIRECT: %s", word.c_str());
    return result;
  }

  // Stem fallback
  std::string stemmed = stemWord(word);
  if (stemmed != word) {
    LOG_INF("DICT", "TRY STEM: %s -> %s", word.c_str(), stemmed.c_str());
    result = lookupWordDirect(stemmed);
    if (result != "NOT FOUND" && result != "DICT NOT FOUND") {
      LOG_INF("DICT", "FOUND VIA STEM: %s", stemmed.c_str());
      return result;
    }
  }

  LOG_ERR("DICT", "WORD NOT FOUND: %s", word.c_str());
  return "NOT FOUND";
}