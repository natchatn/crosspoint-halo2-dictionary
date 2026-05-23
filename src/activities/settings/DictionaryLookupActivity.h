#pragma once

#include "../Activity.h"

class DictionaryLookupActivity : public Activity {

 public:

  DictionaryLookupActivity(
      GfxRenderer& renderer,
      MappedInputManager& mappedInput,
      const std::string& word);

  void onEnter() override;

  void loop() override;

  void render(RenderLock&&) override;

 private:

  std::string currentWord;

  std::string meaning;
};