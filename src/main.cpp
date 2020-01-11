#include <stdio.h>
#include "MongooseWebServer.h"
#include "Router.h"


//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool Hello (struct mg_connection *nc, struct http_message *hm) {
  printf ("HELLO ROUTE\n");
  // mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
  std::string hello("Hello");
  mg_send_head(nc, 200, hello.size(), "Content-Type: text/plain");
  mg_send(nc, hello.c_str(), hello.size());
  return true;
}

//-----------------------------------------------------------------------------
// Function: main
//-----------------------------------------------------------------------------
int main(void) {
  Router r;
  r.AddRoute(new Route (Route::GET, "/hello", Hello));

  MongooseWebServer webServer(r);


  webServer.StartServer();

  return 0;
}