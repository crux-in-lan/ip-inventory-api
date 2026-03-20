#pragma once

#include <drogon/HttpController.h>
#include "services/IpInventoryService.h"
#include <memory>

namespace api::controller {

class IpInventoryController : public drogon::HttpController<IpInventoryController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(IpInventoryController::swaggerRedirect, "/swagger", drogon::Get);
    ADD_METHOD_TO(IpInventoryController::addIpPool, "/ip-inventory/ip-pool", drogon::Post);
    ADD_METHOD_TO(IpInventoryController::reserveIp, "/ip-inventory/reserve-ip", drogon::Post);
    ADD_METHOD_TO(IpInventoryController::assignIpToServiceId, "/ip-inventory/assign-ip-serviceId", drogon::Post);
    ADD_METHOD_TO(IpInventoryController::terminateIpFromServiceId, "/ip-inventory/terminate-ip-serviceId", drogon::Post);
    ADD_METHOD_TO(IpInventoryController::changeServiceId, "/ip-inventory/serviceId-change", drogon::Post);
    ADD_METHOD_TO(IpInventoryController::getByServiceId, "/ip-inventory/serviceId", drogon::Get);
    METHOD_LIST_END

    IpInventoryController();

    void swaggerRedirect(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void addIpPool(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void reserveIp(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void assignIpToServiceId(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void terminateIpFromServiceId(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void changeServiceId(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void getByServiceId(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::shared_ptr<api::service::IpInventoryService> service_;
};

}  // namespace api::controller