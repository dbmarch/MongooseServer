#include <iostream>
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
// Function: WebSocketHandler::ProcessWebSocketPacket
// 
// We are going to look for a JSON object with the following shape:
// {
//    action: 'string',
//    args: <custom json object>
//  }
// Stringified and sent via the websocket.
// actions are assumed well known.
//-----------------------------------------------------------------------------
bool WebSocketHandler::ProcessWebSocketPacket (struct mg_connection *nc,   struct websocket_message *wm ) {
  bool handled{false};
  struct mg_str d = {(char *) wm->data, wm->size};
  // Assume a JSON object:

  // std::string s(d.p, d.len);
  // printf ("RCV: %s\n", s.c_str());

  std::string msg(d.p,d.len);
  std::istringstream is(msg);
  Json::Value root;

  printf ("%s:  JSON RX: '%s'\n", __func__, msg.c_str());

  try {
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
    
    bool ok = Json::parseFromStream(rbuilder, is, &root, &errs);

    // at this point we have parsed the JSON object into root.
    // We can print it out to be sure:

    if (ok) {
      for (auto &it: root.getMemberNames()) {
        std::cout << it  << " : " <<  root[it] << " [" << typeid(root[it]).name() <<"]"<< std::endl;
      }
    } else {
      printf ("Unable to parse the Json object: %s\n",errs.c_str());
    }
  } catch(const std::exception &ex) {
    printf ("Exception %s\n", ex.what() );
  } catch (...) {
    printf ("unknown exception\n");
  }


  printf ("Extracting data from the JSON object:\n");
  std::string actionField (root["action"].asString());

  if (actionField == std::string (WS_ACTION_HELLO)) {
    mWsConnections[nc] = actionField;
    handled = ProcessHelloAction(nc, root["args"]);
  } else {
    mWsConnections[nc] = actionField;
    handled = ProcessAction(actionField, nc, root["args"]);
  } 

  return handled;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::AddWebsocketConnection
//-----------------------------------------------------------------------------
void WebSocketHandler::AddWebSocketConnection (struct mg_connection *nc) {
    std::string msg (TaggedString(nc, "++ joined"));
    printf ("%s %s %p\n", __func__, msg.c_str(), nc);
    SendWebSocketPacket (nc, mg_mk_str("++ joined"));
    
    mWsConnections[nc] = WS_ACTION_NONE;
    
    printf ("%lu connections\n", mWsConnections.size());
}
      
//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::RemoveWebsocketConnection
//-----------------------------------------------------------------------------
void WebSocketHandler::RemoveWebSocketConnection (struct mg_connection *nc) {
  std::string msg (TaggedString(nc, "-- disconnected"));
  printf ("%s %s %p\n", __func__, msg.c_str(), nc);

  auto it = mWsConnections.find(nc);
  if (it != mWsConnections.end()) {
    mWsConnections.erase (it);
    printf ("Element Removed\n");
  }
  printf ("%lu connections\n", mWsConnections.size());
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::TaggedString
//-----------------------------------------------------------------------------
std::string WebSocketHandler::TaggedString (struct mg_connection *nc, std::string text) {
  char addr[32];
  mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                      MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
  std::string response(addr);
  response+=text;
  return response;
}

//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::LookupConnection
//-----------------------------------------------------------------------------
struct mg_connection * WebSocketHandler::LookupConnection (std::string action ) {
  
  for (auto it : mWsConnections ) {
    if (it.second == action) {
      return it.first;
    }
  }
  return nullptr;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::SendWebSocketPacket
//-----------------------------------------------------------------------------
bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value root ){
  if (nc) {
    printf ("bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value root ) NOT IMPLEMENTED\n");

  }
  return false;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::SendWebSocketPacket
//-----------------------------------------------------------------------------
bool SendWebSocketPacket (struct mg_connection * nc, const std::string text ){
  if (nc) {
    printf ("bool SendWebSocketPacket (struct mg_connection * nc,  const std::string text ) NOT IMPLEMENTED\n");
    
  }
  return false;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::SendWebSocketPacket
//-----------------------------------------------------------------------------
bool WebSocketHandler::SendWebSocketPacket(struct mg_connection *nc, const struct mg_str msg) {
if (nc) {
    char buf[500];
    char addr[32];
    printf ("Sending a message\n");
    mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);

    snprintf(buf, sizeof(buf), "%s %.*s", addr, (int) msg.len, msg.p);
    printf("%s %s\n", __func__, buf); /* Local echo. */
    mg_send_websocket_frame(nc, WEBSOCKET_OP_TEXT, buf, strlen(buf));    
    return true;
  }
 return false;

}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::Broadcast
//-----------------------------------------------------------------------------
bool WebSocketHandler::BroadcastWebSocketPacket(struct mg_connection *nc, const struct mg_str msg) {
  if (nc) {
    struct mg_connection *c;
    char buf[500];
    char addr[32];
    mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr),
                        MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
    snprintf(buf, sizeof(buf), "%s %.*s", addr, (int) msg.len, msg.p);
    printf("%s\n", buf); /* Local echo. */
    for (c = mg_next(nc->mgr, NULL); c != NULL; c = mg_next(nc->mgr, c)) {
      if (c == nc) continue; /* Don't send to the sender. */
      mg_send_websocket_frame(c, WEBSOCKET_OP_TEXT, buf, strlen(buf));
    }
    return true;
  }
  return false;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::ProcessHelloAction
//-----------------------------------------------------------------------------
bool WebSocketHandler::ProcessHelloAction( struct mg_connection * nc,  Json::Value args ) {
  printf ("%s\n", __func__);
  SendWebSocketPacket(nc, mg_mk_str("-- WS Hello Packet Rx") );
  return true;
 }
