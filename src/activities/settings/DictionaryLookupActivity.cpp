#include "DictionaryLookupActivity.h"

#include <vector>

#include "../../dict/DictionaryEngine.h"
#include "fontIds.h"

static std::vector<std::string> splitMeaningLines(const std::string& text) {
  std::vector<std::string> lines;

  std::string current;

  for (char c : text) {
    current += c;

    if (c == ',') {
      lines.push_back(current);
      current.clear();
    }
  }

  if (!current.empty()) {
    lines.push_back(current);
  }

  return lines;
}

DictionaryLookupActivity::DictionaryLookupActivity(GfxRenderer& renderer, MappedInputManager& mappedInput,
                                                   const std::string& word)

    : Activity("DictionaryLookupActivity", renderer, mappedInput),

      currentWord(word) {}

void DictionaryLookupActivity::onEnter() {
  Activity::onEnter();

  meaning = lookupWord(currentWord);

  requestUpdate();
}

void DictionaryLookupActivity::loop() {
  if (mappedInput.wasPressed(MappedInputManager::Button::Back)) {
    finish();
  }
}

void DictionaryLookupActivity::render(RenderLock&&) {
  renderer.clearScreen();

  renderer.drawCenteredText(BAIJAMJUREE_16_FONT_ID, 40, currentWord.c_str());

  auto lines = splitMeaningLines(meaning);

  int y = 90;

  for (const auto& line : lines) {
    renderer.drawCenteredText(UI_12_FONT_ID, y, line.c_str());

    y += 40;
  }

  renderer.displayBuffer();
}