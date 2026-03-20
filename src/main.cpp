#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "db/Db.h"
#include "jobs/ReservationCleanupJob.h"

#include <cstdlib>   // for getenv
#include <memory>

inline std::shared_ptr<spdlog::logger> createLogger() {
    auto logger = spdlog::rotating_logger_mt(
        "ip_inventory_logger",
        "logs/ip_inventory.log",
        1024 * 1024 * 5,  // 5 MB
        3                 // 3 files
    );

    logger->set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    logger->set_level(spdlog::level::info);

    return logger;
}

int main() {
    // Load Drogon config
    drogon::app().loadConfigFile("config/config.json");

    auto logger = createLogger();
    spdlog::set_default_logger(logger);
    logger->flush_on(spdlog::level::info);

    // Get DB connection string from environment
    const char* connStr = std::getenv("IP_INVENTORY_DB_URL");

    std::string dbUrl = connStr
        ? connStr
        : "host=127.0.0.1 port=5432 dbname=ip_inventory user=postgres password=postgres";

    // Create shared DB instance
    auto db = std::make_shared<api::db::Db>(dbUrl);

    // Start cleanup job
    api::jobs::ReservationCleanupJob cleanupJob(db, logger, std::chrono::seconds(60));
    cleanupJob.start();

    logger->info("Application started");

    // Run web server (blocking call)
    drogon::app().run();

    return 0;
}