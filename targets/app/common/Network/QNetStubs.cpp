#include "platform/NetTypes.h"
#include "platform/PlatformTypes.h"

IQNetPlayer IQNet::m_player[4];

static bool s_gameRunning = false;

uint8_t IQNetPlayer::GetSmallId() { return 0; }
void IQNetPlayer::SendData(IQNetPlayer* player, const void* pvData,
                           uint32_t dwDataSize, uint32_t dwFlags) {}
bool IQNetPlayer::IsSameSystem(IQNetPlayer* player) { return true; }
uint32_t IQNetPlayer::GetSendQueueSize(IQNetPlayer* player, uint32_t dwFlags) {
    return 0;
}
uint32_t IQNetPlayer::GetCurrentRtt() { return 0; }
bool IQNetPlayer::IsHost() { return this == &IQNet::m_player[0]; }
bool IQNetPlayer::IsGuest() { return false; }
bool IQNetPlayer::IsLocal() { return true; }
PlayerUID IQNetPlayer::GetXuid() { return INVALID_XUID; }
const char* IQNetPlayer::GetGamertag() {
    static const char* name = "stub";
    return name;
}
int IQNetPlayer::GetSessionIndex() { return 0; }
bool IQNetPlayer::IsTalking() { return false; }
bool IQNetPlayer::IsMutedByLocalUser(uint32_t dwUserIndex) { return false; }
bool IQNetPlayer::HasVoice() { return false; }
bool IQNetPlayer::HasCamera() { return false; }
int IQNetPlayer::GetUserIndex() { return this - &IQNet::m_player[0]; }
void IQNetPlayer::SetCustomDataValue(uintptr_t ulpCustomDataValue) {
    m_customData = ulpCustomDataValue;
}
uintptr_t IQNetPlayer::GetCustomDataValue() { return m_customData; }

int32_t IQNet::AddLocalPlayerByUserIndex(uint32_t dwUserIndex) { return 0; }
IQNetPlayer* IQNet::GetHostPlayer() { return &m_player[0]; }
IQNetPlayer* IQNet::GetLocalPlayerByUserIndex(uint32_t dwUserIndex) {
    return &m_player[dwUserIndex];
}
IQNetPlayer* IQNet::GetPlayerByIndex(uint32_t dwPlayerIndex) {
    return &m_player[0];
}
IQNetPlayer* IQNet::GetPlayerBySmallId(uint8_t SmallId) { return &m_player[0]; }
IQNetPlayer* IQNet::GetPlayerByXuid(PlayerUID xuid) { return &m_player[0]; }
uint32_t IQNet::GetPlayerCount() { return 1; }
QNET_STATE IQNet::GetState() {
    return s_gameRunning ? QNET_STATE_GAME_PLAY : QNET_STATE_IDLE;
}
bool IQNet::IsHost() { return true; }
int32_t IQNet::JoinGameFromInviteInfo(uint32_t dwUserIndex, uint32_t dwUserMask,
                                      const INVITE_INFO* pInviteInfo) {
    return 0;
}
void IQNet::HostGame() { s_gameRunning = true; }
void IQNet::EndGame() { s_gameRunning = false; }
