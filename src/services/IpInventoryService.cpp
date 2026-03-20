#include "IpInventoryService.h"
#include "models/JsonConverters.h"
#include "utils/Validation.h"

#include <stdexcept>

namespace api::service {

IpInventoryService::IpInventoryService(std::shared_ptr<api::db::Db> db)
    : db_(std::move(db)) {}

dto::StatusResponse IpInventoryService::addIpPool(const dto::IpPoolRequest& req) {
    api::validation::validate(req);

    pqxx::work tx(db_->connection());

    for (const auto& item : req.ipAddresses) {
        tx.exec_params(
            R"(
                INSERT INTO ip_inventory_pool (ip_address, ip_type, state)
                VALUES ($1, $2, 'FREE')
                ON CONFLICT (ip_address) DO NOTHING
            )",
            item.ip,
            api::json::toString(*item.ipType)
        );
    }

    tx.commit();
    return {};
}

dto::IpAddressesResponse IpInventoryService::reserveIp(const dto::ReserveIpRequest& req) {
    api::validation::validate(req);

    pqxx::work tx(db_->connection());
    dto::IpAddressesResponse response{};

    auto reserveOne = [&](const std::string& wantedType) {
        // 1) Try already reserved for this serviceId
        auto r1 = tx.exec_params(
            R"(
                SELECT ip_address, ip_type
                FROM ip_inventory_pool
                WHERE reserved_for_service_id = $1
                  AND ip_type = $2
                LIMIT 1
            )",
            req.serviceId,
            wantedType
        );

        if (!r1.empty()) {
            dto::IpAddress ip{};
            ip.ip = r1[0]["ip_address"].c_str();
            ip.ipType = api::json::ipTypeFromString(r1[0]["ip_type"].c_str());
            response.ipAddresses.push_back(std::move(ip));
            return;
        }

        // 2) Otherwise take any free IP and reserve it
        auto r2 = tx.exec_params(
            R"(
                WITH picked AS (
                    SELECT id
                    FROM ip_inventory_pool
                    WHERE state = 'FREE'
                      AND ip_type = $1
                    ORDER BY id
                    LIMIT 1
                    FOR UPDATE SKIP LOCKED
                )
                UPDATE ip_inventory_pool p
                   SET state = 'RESERVED',
                       reserved_for_service_id = $2,
                       reserved_at = NOW(),
                       reservation_expires_at = NOW() + INTERVAL '10 minutes' 
                  FROM picked
                 WHERE p.id = picked.id
             RETURNING p.ip_address, p.ip_type
            )",
            wantedType,
            req.serviceId
        );

        if (!r2.empty()) {
            dto::IpAddress ip{};
            ip.ip = r2[0]["ip_address"].c_str();
            ip.ipType = api::json::ipTypeFromString(r2[0]["ip_type"].c_str());
            response.ipAddresses.push_back(std::move(ip));
        }
    };

    switch (req.ipType) {
        case dto::IpType::IPv4:
            reserveOne("IPv4");
            break;
        case dto::IpType::IPv6:
            reserveOne("IPv6");
            break;
        case dto::IpType::Both:
            reserveOne("IPv4");
            reserveOne("IPv6");
            break;
    }

    tx.commit();
    return response;
}

dto::StatusResponse IpInventoryService::assignIpToServiceId(const dto::AssignTerminateRequest& req) {
    api::validation::validate(req);

    pqxx::work tx(db_->connection());

    for (const auto& item : req.ipAddresses) {
        auto result = tx.exec_params(
            R"(
                UPDATE ip_inventory_pool
                   SET state = 'ASSIGNED',
                       assigned_service_id = $1,
                       reserved_for_service_id = NULL
                 WHERE ip_address = $2
                   AND (
                        state = 'FREE'
                        OR (state = 'RESERVED' AND reserved_for_service_id = $1)
                       )
            )",
            req.serviceId,
            item.ip
        );

        (void)result;
    }

    tx.commit();
    return {};
}

dto::StatusResponse IpInventoryService::terminateIpFromServiceId(const dto::AssignTerminateRequest& req) {
    api::validation::validate(req);

    pqxx::work tx(db_->connection());

    for (const auto& item : req.ipAddresses) {
        tx.exec_params(
            R"(
                UPDATE ip_inventory_pool
                   SET state = 'FREE',
                       assigned_service_id = NULL,
                       reserved_for_service_id = NULL
                 WHERE ip_address = $1
                   AND assigned_service_id = $2
            )",
            item.ip,
            req.serviceId
        );
    }

    tx.commit();
    return {};
}

dto::StatusResponse IpInventoryService::changeServiceId(const dto::ServiceIdChangeRequest& req) {
    api::validation::validate(req);

    pqxx::work tx(db_->connection());

    tx.exec_params(
        R"(
            UPDATE ip_inventory_pool
               SET assigned_service_id = $1
             WHERE assigned_service_id = $2
        )",
        req.serviceId,
        req.serviceIdOld
    );

    tx.exec_params(
        R"(
            UPDATE ip_inventory_pool
               SET reserved_for_service_id = $1
             WHERE reserved_for_service_id = $2
        )",
        req.serviceId,
        req.serviceIdOld
    );

    tx.commit();
    return {};
}

dto::IpAddressesResponse IpInventoryService::getByServiceId(const std::string& serviceId) {
    if (serviceId.empty()) {
        throw std::invalid_argument("serviceId must not be empty");
    }

    pqxx::read_transaction tx(db_->connection());

    auto rows = tx.exec_params(
        R"(
            SELECT ip_address, ip_type
            FROM ip_inventory_pool
            WHERE assigned_service_id = $1
            ORDER BY id
        )",
        serviceId
    );

    dto::IpAddressesResponse resp{};

    for (const auto& row : rows) {
        dto::IpAddress ip{};
        ip.ip = row["ip_address"].c_str();
        ip.ipType = api::json::ipTypeFromString(row["ip_type"].c_str());
        resp.ipAddresses.push_back(std::move(ip));
    }

    return resp;
}

}  // namespace api::service