#pragma once

#include "level.h"

namespace loader::file::level
{
class TR3Level : public Level
{
public:
  TR3Level(const Game gameVersion, io::SDLReader&& reader, std::filesystem::path filename)
      : Level{gameVersion, std::move(reader), std::move(filename)}
  {
  }

  void loadFileData() override;
};
} // namespace loader::file::level
