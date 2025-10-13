# WeatherTech Client-Server Project

**Project:** Utveckling av klient–server lösning för väderdata  
**Client:** WeatherTech Solutions AB  
**Project Start:** Vecka 43  
**Final Delivery:** 2025-12-29 och 2025-12-30  
**Methodology:** Scrum (customer is Product Owner)

---

## 📋 Table of Contents

1. [Project Overview](#project-overview)
2. [Project Structure](#project-structure)
3. [Team Division](#team-division)
4. [Sprint Planning](#sprint-planning)
5. [Technical Requirements](#technical-requirements)
6. [Deliverables](#deliverables)
7. [Getting Started](#getting-started)

---

## 🎯 Project Overview

WeatherTech Solutions AB needs a robust client-server platform for real-time weather data. The system will enable customers to fetch, store, and analyze weather data efficiently.

### Key Goals
- Centralized server handling multiple concurrent client requests
- Server connects to multiple external weather services simultaneously
- Clients in both C and C++ for fetching, displaying, and caching weather data
- Architecture designed for future ESP32 porting (with touch-screen support)
- Comprehensive documentation for handover and future development

---

## 📁 Project Structure

```
weather-platform/
│
├── server/                          # Team 1
│   ├── src/
│   │   ├── main.c                   # Server entry point
│   │   ├── tcp_server.c/h           # TCP socket handling
│   │   ├── http_handler.c/h         # HTTP GET/POST parser
│   │   ├── json_parser.c/h          # JSON response builder
│   │   ├── weather_api.c/h          # External API connector
│   │   ├── cache.c/h                # Server-side caching
│   │   └── config.c/h               # Configuration management
│   ├── tests/
│   ├── Makefile
│   └── README.md
│
├── client-c/                        # Team 2
│   ├── src/
│   │   ├── main.c                   # Client entry point
│   │   ├── network.c/h              # TCP client connection
│   │   ├── request.c/h              # HTTP request builder
│   │   ├── json_parse.c/h           # JSON response parser
│   │   ├── display.c/h              # Terminal UI
│   │   ├── cache.c/h                # File-based caching
│   │   └── esp32_compat.h           # ESP32 compatibility layer
│   ├── tests/
│   ├── Makefile
│   └── README.md
│
├── client-cpp/                      # Team 3
│   ├── src/
│   │   ├── main.cpp                 # Client entry point
│   │   ├── Network.cpp/hpp          # TCP client (class-based)
│   │   ├── WeatherClient.cpp/hpp    # Main client class
│   │   ├── JsonParser.cpp/hpp       # JSON parsing
│   │   ├── Display.cpp/hpp          # Terminal UI (OOP)
│   │   └── Cache.cpp/hpp            # File caching (class-based)
│   ├── tests/
│   ├── Makefile
│   └── README.md
│
├── shared/                          # All Teams
│   ├── protocol.h                   # Shared protocol definitions
│   └── constants.h                  # Shared constants
│
├── docs/                            # All Teams
│   ├── architecture.md              # System architecture
│   ├── api_specification.md         # Server API documentation
│   ├── user_guide.md                # User instructions
│   ├── development_guide.md         # Future development guide
│   └── test_plan.md                 # Testing strategy
│
└── tests/                           # All Teams
    ├── integration/                 # End-to-end tests
    └── performance/                 # Load tests
```

---

## 👥 Team Division (6 Programmers)

### Team 1: Server Development (2 programmers)

**Responsibilities:**
- TCP server implementation with multi-client support
- HTTP GET/POST request handling
- JSON response formatting
- External weather API integration (multiple services for redundancy)
- Server-side caching mechanism
- Error handling and logging
- Configuration management

**Deliverables:**
- Functional C server handling concurrent connections
- REST API with JSON responses
- Documentation for server setup and API endpoints

**Key Technologies:**
- C programming
- TCP sockets
- HTTP protocol
- JSON formatting
- Multi-threading/async handling

---

### Team 2: C Client Development (2 programmers)

**Responsibilities:**
- TCP client connection to server
- HTTP request construction
- JSON response parsing
- Terminal UI for weather data display
- File-based caching implementation
- ESP32 compatibility considerations (memory, networking)
- Input validation and error handling

**Deliverables:**
- Functional C client with terminal UI
- Local caching system
- ESP32-ready code architecture
- User guide for C client

**Key Technologies:**
- C programming
- TCP client sockets
- JSON parsing (lightweight)
- File I/O for caching
- Resource-constrained programming (ESP32 future)

**ESP32 Considerations:**
- Avoid dynamic memory allocation where possible
- Use fixed-size buffers
- Memory constraint: ~520KB RAM
- Consider FreeRTOS compatibility
- Network stack limitations

---

### Team 3: C++ Client Development (2 programmers)

**Responsibilities:**
- Object-oriented client architecture
- TCP networking classes
- JSON parsing with modern C++ features
- Terminal UI with C++ approach
- File-based caching with classes
- Unit tests for C++ components
- Code documentation

**Deliverables:**
- Functional C++ client with OOP design
- Parallel functionality to C client
- User guide for C++ client
- Class documentation

**Key Technologies:**
- Modern C++ (C++11/14/17)
- Object-oriented design
- STL containers
- Smart pointers
- RAII principles

---

## 📅 Sprint Planning

### Sprint 1: Foundation & Architecture (Week 43)

**Focus:** Setup and basic architecture

**Tasks:**
- ✅ Setup project structure and repositories
- ✅ Define shared protocol and communication format
- ✅ **Server:** Basic TCP server with single connection
- ✅ **Clients:** Basic connection and simple request
- ✅ Document architecture decisions
- ✅ Setup development environment

**Goal:** All teams can compile and run basic "hello world" versions

---

### Sprint 2: Core Functionality - Prototype (Week 44)

**Focus:** First working end-to-end demo

**Tasks:**
- ✅ **Server:** Multi-client support, HTTP parsing
- ✅ **Server:** Connect to one external weather API
- ✅ **Clients:** Parse JSON responses
- ✅ **Clients:** Display data in terminal
- ✅ Basic error handling
- ✅ **Milestone:** First working end-to-end demo

**Goal:** Customer can see weather data from external API through your system

---

### Sprint 3: Advanced Features (Week 45-46)

**Focus:** Complete core functionality

**Tasks:**
- ✅ **Server:** Multiple API integration + redundancy
- ✅ **Server:** Implement caching mechanism
- ✅ **Clients:** Implement local caching
- ✅ **C Client:** ESP32 compatibility layer
- ✅ Improve error handling and validation
- ✅ Integration testing

**Goal:** All core features working, redundancy in place

---

### Sprint 4: Polish & Testing (Week 47-48)

**Focus:** Quality assurance

**Tasks:**
- ✅ Performance optimization
- ✅ Comprehensive testing (unit, integration, load)
- ✅ Code review and refactoring
- ✅ Security review
- ✅ Complete all documentation
- ✅ User acceptance testing

**Goal:** Production-ready code with full test coverage

---

### Sprint 5: Final Delivery (Week 49-52)

**Focus:** Delivery preparation

**Tasks:**
- ✅ Bug fixes from testing
- ✅ Final documentation review
- ✅ Deployment preparation
- ✅ Knowledge transfer
- ✅ **Final delivery:** Dec 29-30, 2025

**Goal:** Complete handover to customer

---

## 🔧 Technical Requirements

### Server (K1-K2)
- **Language:** C
- **Protocol:** TCP
- **API:** REST with HTTP GET/POST
- **Format:** JSON responses
- **Features:**
  - Multi-client concurrent connections
  - Multiple external weather API connections
  - Server-side caching
  - Modular design for easy endpoint addition

### Clients (K3-K6)
- **Languages:** C and C++
- **Features:**
  - Connect to server and request weather data
  - Display data in terminal with clear structure
  - Local file-system caching
  - Input validation and error handling
- **C Client Specific:**
  - Must be portable to ESP32
  - Resource-efficient design
  - Memory-conscious implementation

### Documentation (K7)
- System architecture
- API specification
- User guides
- Development/extension guide
- Test plan

### Quality Requirements (K8-K10)
- Modular design for future extensions
- Resource-efficient code (ESP32 consideration)
- Test plan with error simulation
- Security considerations

---

## 📦 Deliverables

### Code
1. ✅ Functional server application (C)
2. ✅ Functional C client application
3. ✅ Functional C++ client application

### Documentation
1. ✅ System architecture document
2. ✅ API specification
3. ✅ User instructions (for both clients)
4. ✅ Development guide for future extensions
5. ✅ Test plan and test results

### Testing
1. ✅ Unit tests for all components
2. ✅ Integration tests
3. ✅ Performance/load tests
4. ✅ Error scenario tests

---

## 🚀 Getting Started

### Prerequisites
```bash
# Required tools
- GCC or Clang compiler
- Make
- Git
- JSON library (e.g., cJSON, json-c)
- For testing: valgrind, gdb
```

### Building the Project

#### Server
```bash
cd server/
make
./weather_server
```

#### C Client
```bash
cd client-c/
make
./weather_client
```

#### C++ Client
```bash
cd client-cpp/
make
./weather_client_cpp
```

### Running Tests
```bash
# Unit tests
make test

# Integration tests
cd tests/integration/
./run_integration_tests.sh

# Performance tests
cd tests/performance/
./run_load_tests.sh
```

---

## ⚠️ Important Notes

### Week 43 Critical Tasks
**ALL TEAMS MUST:**
1. Define the shared protocol (JSON format, API endpoints)
2. Agree on error codes and response formats
3. Create `shared/protocol.h` with common definitions
4. Setup Git workflow and branching strategy

### Daily Coordination
- **Daily standups** (15 min) - sync between teams
- **Weekly sprint review** with customer (PO)
- **Sprint retrospective** - continuous improvement

### ESP32 Considerations (Team 2)
- **Memory:** Target ~520KB RAM total
- **No dynamic allocation:** Use static buffers
- **Network:** Consider lwIP stack limitations
- **Display:** Plan for TFT touch screen in Phase 2
- **Testing:** Test on actual ESP32 hardware when possible

### API Integration (Team 1)
Consider these weather APIs:
- OpenWeatherMap
- WeatherAPI.com
- Visual Crossing
- SMHI (Swedish Meteorological Institute)

### Caching Strategy
- **Server cache:** Reduce API costs, 5-15 min TTL
- **Client cache:** Offline capability, 30-60 min TTL
- **Cache invalidation:** Time-based + manual refresh

---

## 📞 Contact

**Project Manager:** Robin Andersson  
**Email:** robin.andersson1@chasacademy.se  
**Phone:** +46 (0)8 123 456 78

---

## 📄 License

[Project license information]

---

**Last Updated:** 2025-10-13  
**Version:** 1.0
