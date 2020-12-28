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
bool WebSocketHandler::ProcessWebSocketPacket (struct mg_connection *nc,   struct mg_ws_message *wm ) {
  bool handled{false};
  struct mg_str d{wm->data};
  // Assume a JSON object:

  // std::string s(d.p, d.len);
  // printf ("RCV: %s\n", s.c_str());

  std::string msg(d.ptr,d.len);
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
    SendWebSocketPacket (nc,"++ joined");
    
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
  char buf[500];
  std::string response (mg_ntoa(&nc->peer, buf, sizeof(buf)));
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
bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value &root ){
  if (nc) {
    printf ("bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value root )\n");
    
    Json::StreamWriterBuilder builder;
    const std::string jsonString = Json::writeString(builder, root);
    mg_ws_send (nc, jsonString.c_str(), jsonString.length(), WEBSOCKET_OP_TEXT);
  }
  return false;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::SendWebSocketPacket
//-----------------------------------------------------------------------------
bool WebSocketHandler::SendWebSocketPacket (struct mg_connection * nc, const std::string &text ){
  if (nc) {
    printf ("bool SendWebSocketPacket (struct mg_connection * nc,  const std::string text )\n");
    mg_ws_send (nc, text.c_str(), text.length(), WEBSOCKET_OP_TEXT);
  }
  return false;
}


//-----------------------------------------------------------------------------
// Function:  WebSocketHandler::SendWebSocketPacket
//-----------------------------------------------------------------------------
bool WebSocketHandler::SendWebSocketPacket(struct mg_connection *nc, const char* msg) {
if (nc) {
    char buf[500];

    std::string peerAddr(mg_ntoa(&nc->peer, buf, sizeof(buf)));
    std::string msgToSend = peerAddr + std::string (msg);
    printf("%s '%s'\n", __func__, msgToSend.c_str()); /* Local echo. */
    mg_ws_send (nc, msgToSend.c_str(), msgToSend.length(), WEBSOCKET_OP_TEXT);    
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

    std::string peerAddr(mg_ntoa(&nc->peer, buf, sizeof(buf)));
    std::string msgToSend = peerAddr + std::string (msg.ptr, msg.len);
    for (c = nc->mgr->conns; c != nullptr; c = c->next) {
      if (c == nc) continue; /* Don't send to the sender. */
      SendWebSocketPacket(c, msgToSend);
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
  SendWebSocketPacket(nc, "-- WS Hello Packet Rx");
  return true;
 }
