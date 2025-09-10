#pragma once

#include <iostream>
#include <string>
#include <memory>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <chrono>

#include <WinSock2.h>
#include <mswsock.h>

#include <PxPhysicsAPI.h>
#include <spdlog/spdlog.h>

using std::shared_ptr;
using std::unique_ptr;
using std::make_shared;

using std::cout;
using std::endl;
using std::string;

using std::chrono::system_clock;
using std::chrono::local_time;
using std::chrono::zoned_time;
using std::chrono::current_zone;

using std::vector;
using std::unordered_map;
using std::unordered_set;
using std::queue;

using std::thread;
using std::mutex;
using std::condition_variable;
using std::atomic;

using NetworkId_t = unsigned int;

using namespace physx;