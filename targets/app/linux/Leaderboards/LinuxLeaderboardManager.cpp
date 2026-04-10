#include "LinuxLeaderboardManager.h"

#include "app/common/Leaderboards/LeaderboardManager.h"

LeaderboardManager* LeaderboardManager::m_instance =
    new LinuxLeaderboardManager();  // Singleton instance of the
                                    // LeaderboardManager