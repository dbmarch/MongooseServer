#include <string>
#include "WebSocketHandler.h"

//-----------------------------------------------------------------------------
// Function: WebSocketHandler::WebSocketHandler
//-----------------------------------------------------------------------------
WebSocketHandler::WebSocketHandler()
{
  printf ("%s\n", __func__);
}



//-----------------------------------------------------------------------------
// Function: WebSocketHandler::~WebSocketHandler
//-----------------------------------------------------------------------------
WebSocketHandler::~WebSocketHandler() {
  printf ("%s", __func__);
}

//-----------------------------------------------------------------------------
// Function: WebSocketHandler::ProcessPacket
//-----------------------------------------------------------------------------
bool WebSocketHandler::ProcessPacket (struct mg_connection *nc,   struct websocket_message *wm ) {
  struct mg_str d = {(char *) wm->data, wm->size};
  std::string s(d.p, d.len);
  printf ("RCV: %s\n", s.c_str());
  return true;
}