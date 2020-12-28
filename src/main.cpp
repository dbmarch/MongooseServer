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
  
  MongooseWebServer webServer(myServices);
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