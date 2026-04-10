#pragma once

#include "DLCFile.h"
#include "app/common/GameRules/LevelGeneration/LevelGenerationOptions.h"

class DLCGameRules : public DLCFile {
public:
    DLCGameRules(DLCManager::EDLCType type, const std::string& path)
        : DLCFile(type, path) {}
};