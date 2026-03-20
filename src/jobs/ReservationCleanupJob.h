#pragma once

#include "db/Db.h"
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace api::jobs {

class ReservationCleanupJob {
public:
    ReservationCleanupJob(std::shared_ptr<api::db::Db> db,
                          std::shared_ptr<spdlog::logger> logger,
                          std::chrono::seconds interval);
    ~ReservationCleanupJob();

    void start();
    void stop();

private:
    void run();

    std::shared_ptr<api::db::Db> db_;
    std::shared_ptr<spdlog::logger> logger_;
    std::chrono::seconds interval_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

}