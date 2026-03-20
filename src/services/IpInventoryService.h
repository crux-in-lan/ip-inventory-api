#pragma once

#include "db/Db.h"
#include "models/ApiDtos.h"
#include <memory>

namespace api::service {

class IpInventoryService {
public:
    explicit IpInventoryService(std::shared_ptr<api::db::Db> db);

    dto::StatusResponse addIpPool(const dto::IpPoolRequest& req);
    dto::IpAddressesResponse reserveIp(const dto::ReserveIpRequest& req);
    dto::StatusResponse assignIpToServiceId(const dto::AssignTerminateRequest& req);
    dto::StatusResponse terminateIpFromServiceId(const dto::AssignTerminateRequest& req);
    dto::StatusResponse changeServiceId(const dto::ServiceIdChangeRequest& req);
    dto::IpAddressesResponse getByServiceId(const std::string& serviceId);

private:
    std::shared_ptr<api::db::Db> db_;
};

}  // namespace api::service