CREATE DATABASE ip_inventory;

CREATE TABLE IF NOT EXISTS ip_inventory_pool (
    id BIGSERIAL PRIMARY KEY,
    ip_address INET NOT NULL UNIQUE,
    ip_type VARCHAR(10) NOT NULL CHECK (ip_type IN ('IPv4', 'IPv6')),
    state VARCHAR(16) NOT NULL CHECK (state IN ('FREE', 'RESERVED', 'ASSIGNED')),
    reserved_for_service_id VARCHAR(128),
    assigned_service_id VARCHAR(128),
    created_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    reserved_at TIMESTAMPTZ DEFAULT NULL,
    reservation_expires_at TIMESTAMPTZ DEFAULT NULL	
);

CREATE INDEX IF NOT EXISTS idx_ip_inventory_pool_reserved_service
    ON ip_inventory_pool(reserved_for_service_id);

CREATE INDEX IF NOT EXISTS idx_ip_inventory_pool_assigned_service
    ON ip_inventory_pool(assigned_service_id);

CREATE INDEX IF NOT EXISTS idx_ip_inventory_pool_state_type
    ON ip_inventory_pool(state, ip_type);