# Root Makefile

# Directories
SERVER_DIR := server/src
C_CLIENT_DIR := client-c/src
CXX_CLIENT_DIR := client-cpp/src
LIBS_DIR := libs

# Targets for sub-projects
all: server-client c-client cxx-client

# Delegate build to subfolders
server:
	$(MAKE) -C $(SERVER_DIR)

c-client:
	$(MAKE) -C $(C_CLIENT_DIR)

cxx-client:
	$(MAKE) -C $(CXX_CLIENT_DIR)

run-server:
	$(MAKE) -C $(SERVER_DIR) run

run-c-client:
	$(MAKE) -C $(C_CLIENT_DIR) run

run-cxx-client:
	$(MAKE) -C $(CXX_CLIENT_DIR) run

# TODO - Enable when C++ client is ready, also consider libraries build if we decide to use libs as a shared/built component instead of individual files
clean:
	$(MAKE) -C $(SERVER_DIR) clean
	$(MAKE) -C $(C_CLIENT_DIR) clean
#	$(MAKE) -C $(CXX_CLIENT_DIR) clean 
#	$(MAKE) -C $(LIBS_DIR) clean

.PHONY: all server-client c-client cxx-client clean