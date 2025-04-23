#pragma once

constexpr double TICK_RATE = 60.0;
constexpr double TICK_INTERVAL_S = 1.0 / TICK_RATE;
constexpr uint64 WORKER_TICK_MS = static_cast<uint64>(TICK_INTERVAL_S * 1000.0);