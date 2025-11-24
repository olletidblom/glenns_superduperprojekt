#include "../../libs/utils.h"
#include "HTTPServer.h"
#include "Handlers/weather_handler.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
  smw_init();

  HTTPServer *httpServer = NULL;
  if (HTTPServer_Initialize(http_POST, &httpServer) != 0) {
    printf("Failed to initialize HTTP server\n");
    return -1;
  }

  // Register routes
  HTTPServer_RegisterRoute(httpServer, "GET", "/", Handle_Weather);
  HTTPServer_RegisterRoute(httpServer, "GET", "/weather", Handle_Weather);
  HTTPServer_RegisterRoute(httpServer, "GET", "/users", Handle_UsersGET);
  HTTPServer_RegisterRoute(httpServer, "POST", "/users", Handle_UsersPOST);
  
  printf("Server running on port 10180\n");
  printf("Registered routes: %zu\n", httpServer->route_count);
  printf("  GET  /\n");
  printf("  GET  /weather\n");
  printf("  GET  /users\n");
  printf("  POST /users\n");

  uint64_t monTime = 0;
  while (smw_getTaskCount() > 0) {
    monTime = SystemMonotonicMS();
    smw_work(monTime);
  }

  HTTPServer_Dispose(&httpServer);
  smw_dispose();
  

  return 0;
}
