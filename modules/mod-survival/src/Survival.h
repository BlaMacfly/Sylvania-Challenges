#pragma once

#include "SurvivalModel.h"

class Player;
class GameObject;

namespace SylvaniaSurvival
{
    bool IsAvailable();
    bool IsParticipant(Player* player);
    void AddAltarOption(Player* player);
    bool ActivateAtAltar(Player* player, GameObject* altar);
}
