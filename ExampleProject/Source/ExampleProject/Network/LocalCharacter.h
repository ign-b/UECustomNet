#pragma once

#include <chrono>
#include <cstdint>

using clk = std::chrono::steady_clock;

class CLocalCharacter {
protected:
	static uint32_t constexpr SYNC_FREQUENCY_MAXIMUM_RATE = 15; // ms

protected:
	static clk::time_point LastSyncTime;
	
public:
	static void Process();
};
