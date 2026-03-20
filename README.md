# IP Inventory REST API (C++20)

## Overview

This project implements a REST API for managing an IP inventory system.

The API supports:
- Adding IP addresses to a pool
- Reserving IPs for a serviceId
- Assigning IPs to a serviceId
- Terminating IP assignments
- Transferring IPs between serviceIds
- Querying assigned IPs

The implementation is written in modern C++ (C++20) using:
- Drogon (HTTP framework)
- libpqxx (PostgreSQL client)
- PostgreSQL 17 (database)
- CMake (build system)
- vcpkg (dependency management)

A Swagger (OpenAPI) definition, a simple web UI, a cleanup job that periodically releases expired IP reservations, a logger and a sample Google unit tests are included.


## Architecture

The project follows a layered architecture:

    Controller (HTTP layer)
    ↓
    Service (business logic)
    ↓
    Database/Repository (PostgreSQL via libpqxx)

### Components

- **Controllers**
  - Handle HTTP requests/responses
  - Parse JSON input
  - Return JSON output

- **Services**
  - Implement business logic
  - Manage IP state transitions (FREE, RESERVED, ASSIGNED)

- **Database layer**
  - Uses libpqxx
  - Executes parameterized SQL queries

- **Swagger UI**
  - Served as static content
  - Available at `/swagger/`
  
- **Cleanup job**
  - A reservation is automatically returned to the FREE pool if it is not confirmed by assign-ip-serviceId before the configured timeout
  - Executed in separated thread


## API Endpoints

### POST
- `/ip-inventory/ip-pool`
- `/ip-inventory/reserve-ip`
- `/ip-inventory/assign-ip-serviceId`
- `/ip-inventory/terminate-ip-serviceId`
- `/ip-inventory/serviceId-change`

### GET
- `/ip-inventory/serviceId?serviceId=xxx`

- **Simple web UI**
  - http://localhost:8080
  - web/index.html

- **Full API specification**
  - http://localhost:8080/swagger/
  - swagger/openapi.yaml

## Technologies Used

| Component        | Technology        |
|-----------------|------------------|
| Language        | C++20            |
| Web framework   | Drogon           |
| Database        | PostgreSQL 17    |
| DB client       | libpqxx          |
| Build system    | CMake            |
| Package manager | vcpkg            |
| API docs        | OpenAPI (Swagger)|


## Project Structure
```
ip-inventory-api/
├── CMakeLists.txt
├── vcpkg.json
├── CMakePresets.json
├── config/
│ └── config.json
├── src/
│ ├── main.cpp
│ ├── jobs/
│ │ ├── ReservationCleanupJob.h
│ │ └── ReservationCleanupJob.cpp
│ ├── controllers/
│ │ ├── IpInventoryController.h
│ │ └── IpInventoryController.cpp
│ ├── services/
│ │ ├── IpInventoryService.h
│ │ └── IpInventoryService.cpp
│ ├── db/
│ │ ├── Db.h
│ │ └── Db.cpp
│ ├── models/
│ │ ├── ApiDtos.h
│ │ └── JsonConverters.h
│ └── utils/
│   └── Validation.h
├── sql/
│ └── 001_schema.sql
├── swagger/
│ │── index.html
│ │── openapi.yaml
│ │── swagger-initializer.js
│ │── swagger-ui.css
│ │── swagger-ui-bundle.js
│ └── swagger-ui-standalone-preset.js
├── web/
│ └── index.html
└── tests/
  └── ValidationTests.cpp
```


## Prerequisites

- Visual Studio 2022 (with C++ workload)
    * MSVC v143 - C++ compiler/toolchain for VS 2022
    * CMake tools - meta-build system, generating native build files (like Makefiles, Ninja files, or Visual Studio solutions) to compile, test, and package software (CMake → Ninja → MSVC (cl.exe))
    * Windows 11 SDK
    * Google Test adapter - for unit/integration tests.
- vcpkg (standalone) - free package manager for quick installation of libraries like cpprestsdk, oatpp, Boost, pqxx, sqlite, fmt, spdlog, gtest etc.
- PostgreSQL 17 - open-source object-relational database system

## vcpkg (standalone) installation
1. Download and install Git
2. In PowerShell, check(If empty - we need to proceed the installation):
```
echo $env:VCPKG_ROOT
```
3. In PowerShell:
```
cd C:\
git clone https://github.com/microsoft/vcpkg.git
cd .\vcpkg
.\bootstrap-vcpkg.bat
```
4. Then set the environment variable (PowerShell):
```
setx VCPKG_ROOT "C:\vcpkg"
```


## Dependency Installation

Dependencies are managed via **vcpkg manifest mode**.

File: vcpkg.json

Dependencies:
- drogon
- libpqxx
- gtest
- spdlog

## Database Setup

1. Install PostgreSQL 17 with user "postgres" and password "postgres"
2. Execute sql/001_schema.sql content (DDL) via any all-in-one solution for database development e.g. pgAdmin, Navicat etc.

## Build and Run Instructions (Windows)
1. Clone the Repository(PowerShell):
```
cd C:\
git clone https://github.com/crux-in-lan/ip-inventory-api.git
```
2. Add builtin-baseline to vcpkg.json (we are using standalone vcpkg):
```
cd C:\ip-inventory-api
& "$env:VCPKG_ROOT\vcpkg.exe" x-update-baseline --add-initial-baseline
```
3. Open project in Visual Studio:
File → Open → Folder → ip-inventory-api

⚠️ Note:
First build may take **20–40 minutes** because dependencies are compiled from source. Subsequent builds are fast.

4. Ensure vcpkg is configured:
- `VCPKG_ROOT` environment variable is set
- `CMakePresets.json` references vcpkg toolchain

5. Build:
Ctrl + Shift + B

6. Run: F5
7. You can also run some unit tests from "Visual Studio -> Tests -> Run All Tests"

## Design Decisions

CMake chosen for cross-platform support
vcpkg used for reproducible dependency management
Drogon selected for performance and simplicity in C++
PostgreSQL INET type used for IP storage (strong validation)
Clear separation between layers for maintainability

## Future Improvements

1. Add authentication/authorization (API key + JWT)
2. Use connection pooling(when and if necessary)
3. Add pagination for heavy queries(when and if necessary)

## Author

Nikola Vasilev
nicksona.in.lan@gmail.com
TEST
