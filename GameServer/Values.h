#pragma once

constexpr double TICK_RATE = 30.0;	// 30 tick/s => tick interval(1 tick) = 0.0333.. s = 33.3.. ms
constexpr double TICK_INTERVAL_S = 1.0 / TICK_RATE;
constexpr uint64 WORKER_TICK_MS = static_cast<uint64>(TICK_INTERVAL_S * 1000.0);