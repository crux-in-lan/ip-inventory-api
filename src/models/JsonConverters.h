#pragma once

#include <drogon/drogon.h>
#include "ApiDtos.h"
#include <stdexcept>
#include <string>

namespace api::json {

inline std::string toString(dto::IpType type) {
    switch (type) {
        case dto::IpType::IPv4: return "IPv4";
        case dto::IpType::IPv6: return "IPv6";
        case dto::IpType::Both: return "Both";
    }
    throw std::runtime_error("Unknown IpType");
}

inline dto::IpType ipTypeFromString(const std::string& s) {
    if (s == "IPv4") return dto::IpType::IPv4;
    if (s == "IPv6") return dto::IpType::IPv6;
    if (s == "Both") return dto::IpType::Both;
    throw std::invalid_argument("Invalid ipType: " + s);
}

inline dto::IpAddress parseIpAddress(const Json::Value& v, bool requireIpType) {
    dto::IpAddress out{};
    out.ip = v["ip"].asString();

    if (requireIpType) {
        if (!v.isMember("ipType")) {
            throw std::invalid_argument("Missing ipType");
        }
        out.ipType = ipTypeFromString(v["ipType"].asString());
    } else if (v.isMember("ipType")) {
        out.ipType = ipTypeFromString(v["ipType"].asString());
    }

    return out;
}

inline dto::IpPoolRequest parseIpPoolRequest(const Json::Value& root) {
    dto::IpPoolRequest req{};
    for (const auto& item : root["ipAddresses"]) {
        req.ipAddresses.push_back(parseIpAddress(item, true));
    }
    return req;
}

inline dto::ReserveIpRequest parseReserveIpRequest(const Json::Value& root) {
    dto::ReserveIpRequest req{};
    req.serviceId = root["serviceId"].asString();
    req.ipType = ipTypeFromString(root["ipType"].asString());
    return req;
}

inline dto::AssignTerminateRequest parseAssignTerminateRequest(const Json::Value& root) {
    dto::AssignTerminateRequest req{};
    req.serviceId = root["serviceId"].asString();
    for (const auto& item : root["ipAddresses"]) {
        req.ipAddresses.push_back(parseIpAddress(item, false));
    }
    return req;
}

inline dto::ServiceIdChangeRequest parseServiceIdChangeRequest(const Json::Value& root) {
    dto::ServiceIdChangeRequest req{};
    req.serviceIdOld = root["serviceIdOld"].asString();
    req.serviceId = root["serviceId"].asString();
    return req;
}

inline Json::Value toJson(const dto::StatusResponse& resp) {
    Json::Value root;
    root["statusCode"] = resp.statusCode;
    root["statusMessage"] = resp.statusMessage;
    return root;
}

inline Json::Value toJson(const dto::IpAddressesResponse& resp) {
    Json::Value root;
    root["ipAddresses"] = Json::arrayValue;

    for (const auto& ip : resp.ipAddresses) {
        Json::Value item;
        item["ip"] = ip.ip;
        if (ip.ipType.has_value()) {
            item["ipType"] = toString(*ip.ipType);
        }
        root["ipAddresses"].append(item);
    }

    return root;
}

inline Json::Value makeErrorJson(const std::string& code, const std::string& message) {
    Json::Value root;
    root["statusCode"] = code;
    root["statusMessage"] = message;
    return root;
}

}  // namespace api::json