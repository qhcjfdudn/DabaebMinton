#pragma once

#include <unordered_map>
#include <vector>
#include <deque>
#include <thread>
#include <chrono>
#include <random>

#include <spdlog/spdlog.h>

using namespace std::chrono;

using std::unordered_map;
using std::vector;
using std::deque;
using std::thread;
using std::shared_ptr;

using NetworkId_t = uint32_t;
using PlayerId_t = uint32_t;