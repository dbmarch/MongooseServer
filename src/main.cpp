#include <signal.h>

#include "MongooseWebServer.h"
#include "Services.h"


static bool serverRunning{};


//-----------------------------------------------------------------------------
// Function: sigintHandler
//-----------------------------------------------------------------------------
void sigintHandler (int sigNum) {
  signal (SIGINT, sigintHandler);
  serverRunning = false;
}

//-----------------------------------------------------------------------------
// Function: main
//-----------------------------------------------------------------------------
int main(void) {
  Services myServices; // Get our back end services.
  const std::string rootFolder{"client/build"};
  const std::string serverUrl {"http://localhost:8000"};
  
  MongooseWebServer webServer(myServices, rootFolder, serverUrl);
  
  webServer.StartServer();
  serverRunning = true;
  
  signal(SIGINT, sigintHandler);

  while (serverRunning) {
    sleep(1);
  }
  printf ("Server Shutting down");
  webServer.StopServer();
  return 0;
}