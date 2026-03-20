#include "IpInventoryController.h"
#include "db/Db.h"
#include "models/JsonConverters.h"

using namespace drogon;

namespace api::controller {

namespace {
HttpResponsePtr makeJsonResponse(const Json::Value& body, HttpStatusCode code = k200OK) {
    auto resp = HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(code);
    return resp;
}
}  // namespace

IpInventoryController::IpInventoryController() {
    const char* connStr = std::getenv("IP_INVENTORY_DB_URL");
    std::string dbUrl = connStr ? connStr : "host=127.0.0.1 port=5432 dbname=ip_inventory user=postgres password=postgres";

    auto db = std::make_shared<api::db::Db>(dbUrl);
    service_ = std::make_shared<api::service::IpInventoryService>(db);
}

void IpInventoryController::swaggerRedirect(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback)
{
    auto resp = HttpResponse::newRedirectionResponse("/swagger/");
    callback(resp);
}

void IpInventoryController::addIpPool(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const auto& jsonBody = *req->getJsonObject();
        auto dto = api::json::parseIpPoolRequest(jsonBody);
        auto result = service_->addIpPool(dto);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

void IpInventoryController::reserveIp(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const auto& jsonBody = *req->getJsonObject();
        auto dto = api::json::parseReserveIpRequest(jsonBody);
        auto result = service_->reserveIp(dto);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

void IpInventoryController::assignIpToServiceId(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const auto& jsonBody = *req->getJsonObject();
        auto dto = api::json::parseAssignTerminateRequest(jsonBody);
        auto result = service_->assignIpToServiceId(dto);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

void IpInventoryController::terminateIpFromServiceId(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const auto& jsonBody = *req->getJsonObject();
        auto dto = api::json::parseAssignTerminateRequest(jsonBody);
        auto result = service_->terminateIpFromServiceId(dto);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

void IpInventoryController::changeServiceId(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        const auto& jsonBody = *req->getJsonObject();
        auto dto = api::json::parseServiceIdChangeRequest(jsonBody);
        auto result = service_->changeServiceId(dto);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

void IpInventoryController::getByServiceId(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& callback) {
    try {
        auto serviceId = req->getOptionalParameter<std::string>("serviceId");
        if (!serviceId || serviceId->empty()) {
            callback(makeJsonResponse(api::json::makeErrorJson("1001", "Missing serviceId"), k400BadRequest));
            return;
        }

        auto result = service_->getByServiceId(*serviceId);
        callback(makeJsonResponse(api::json::toJson(result)));
    } catch (const std::exception& ex) {
        callback(makeJsonResponse(api::json::makeErrorJson("1001", ex.what()), k400BadRequest));
    }
}

}  // namespace api::controller