#pragma once

#include <memory>
#include <pqxx/pqxx>

namespace api::db {

class Db {
public:
    explicit Db(const std::string& connectionString);

    pqxx::connection& connection();

private:
    std::unique_ptr<pqxx::connection> conn_;
};

}  // namespace api::db