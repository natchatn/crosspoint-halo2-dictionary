#include "DictionaryLookupActivity.h"

#include "fontIds.h"

#include "../../dict/DictionaryEngine.h"

DictionaryLookupActivity::DictionaryLookupActivity(
    GfxRenderer& renderer,
    MappedInputManager& mappedInput,
    const std::string& word)

    : Activity(
          "DictionaryLookupActivity",
          renderer,
          mappedInput),

      currentWord(word) {}

void DictionaryLookupActivity::onEnter() {

  Activity::onEnter();

  meaning = lookupWord(currentWord);

  requestUpdate();
}

void DictionaryLookupActivity::loop() {

  if (mappedInput.wasPressed(
          MappedInputManager::Button::Back)) {

    finish();
  }
}

void DictionaryLookupActivity::render(RenderLock&&) {

  renderer.clearScreen();

  renderer.drawCenteredText(
      BAIJAMJUREE_16_FONT_ID,
      40,
      currentWord.c_str());

  renderer.drawCenteredText(
      UI_12_FONT_ID,
      90,
      meaning.c_str());

  renderer.displayBuffer();
}