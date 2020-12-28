#ifndef WEBSOCKETHANDLER_H__
#define WEBSOCKETHANDLER_H__

#include "mongoose.h"
#include "json.h"

class WebSocketHandler {
  public:

    WebSocketHandler ();

    virtual ~WebSocketHandler();

    WebSocketHandler(const WebSocketHandler&) = delete;    
    WebSocketHandler& operator=(const WebSocketHandler&) = delete;

 protected:

    bool ProcessWebSocketPacket (struct mg_connection *nc,   struct mg_ws_message *wm );

    bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value &root );
    bool SendWebSocketPacket (struct mg_connection * nc, const std::string &text );
    bool SendWebSocketPacket (struct mg_connection * nc, const char* msg);
    bool BroadcastWebSocketPacket(struct mg_connection *nc, const struct mg_str msg);

    void AddWebSocketConnection(struct mg_connection *nc);
    void RemoveWebSocketConnection(struct mg_connection *nc);

    virtual bool ProcessAction( std::string action, struct mg_connection *nc, Json::Value root ) {return false; }

    std::string TaggedString (struct mg_connection *nc, std::string text);

    struct mg_connection * LookupConnection (std::string action );

  private:
     // packet types.  Rest are user defined.  They just get routed to other side
     static constexpr char const * WS_ACTION_NONE="None";    // used when we can't find descriptor
     static constexpr char const * WS_ACTION_HELLO="Hello";  // This is an internal ping
  
     // Ping / health check
     bool ProcessHelloAction( struct mg_connection *nc, Json::Value root );   

    std::map<struct mg_connection*, std::string> mWsConnections; 
};

#endif