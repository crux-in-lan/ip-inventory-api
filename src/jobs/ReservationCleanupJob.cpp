#include "ReservationCleanupJob.h"
#include <pqxx/pqxx>

namespace api::jobs {

ReservationCleanupJob::ReservationCleanupJob(std::shared_ptr<api::db::Db> db,
                                             std::shared_ptr<spdlog::logger> logger,
                                             std::chrono::seconds interval)
    : db_(std::move(db)), logger_(std::move(logger)), interval_(interval) {}

ReservationCleanupJob::~ReservationCleanupJob() {
    stop();
}

void ReservationCleanupJob::start() {
    if (running_) return;
    running_ = true;
    worker_ = std::thread(&ReservationCleanupJob::run, this);
}

void ReservationCleanupJob::stop() {
    running_ = false;
    if (worker_.joinable()) {
        worker_.join();
    }
}

void ReservationCleanupJob::run() {
    while (running_) {
        try {
            pqxx::work tx(db_->connection());
            auto result = tx.exec(R"(
                UPDATE ip_inventory_pool
                SET state = 'FREE',
                    reserved_for_service_id = NULL,
                    reserved_at = NULL,
                    reservation_expires_at = NULL
                WHERE state = 'RESERVED'
                  AND assigned_service_id IS NULL
                  AND reservation_expires_at IS NOT NULL
                  AND reservation_expires_at < NOW()
            )");

            logger_->info("[CLEANUP] Released {} expired reservations", result.affected_rows());

            tx.commit();
        } catch (const std::exception& ex) {
            // log error in real implementation
            logger_->error("[CLEANUP] Error: {}", ex.what());
        }

        std::this_thread::sleep_for(interval_);
    }
}

}