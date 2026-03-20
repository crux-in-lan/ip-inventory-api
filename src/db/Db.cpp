#include "Db.h"
#include <stdexcept>

namespace api::db {

Db::Db(const std::string& connectionString)
    : conn_(std::make_unique<pqxx::connection>(connectionString)) {
    if (!conn_ || !conn_->is_open()) {
        throw std::runtime_error("Failed to open PostgreSQL connection");
    }
}

pqxx::connection& Db::connection() {
    return *conn_;
}

}  // namespace api::db