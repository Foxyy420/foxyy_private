#pragma once
#include "valve_sdk\csgostructs.hpp"

namespace Resolver
{
	void OnFrameStageNotify(ClientFrameStage_t stage);
	void FireGameEvent(IGameEvent* event);
}