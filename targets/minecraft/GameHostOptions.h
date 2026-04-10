#pragma once

#include <cstdint>

#include "app/common/App_Defines.h"
#include "minecraft/GameEnums.h"

// Stateless bitfield utilities - no app dependency.
// The global-state overloads (get/set with no settings arg) have moved
// to IgameServices().getGameHostOption / setGameHostOption.
namespace GameHostOptions {

unsigned int get(unsigned int settings, eGameHostOption option);
void set(unsigned int& settings, eGameHostOption option, unsigned int value);

}  // namespace GameHostOptions
