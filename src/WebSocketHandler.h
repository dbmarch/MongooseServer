#ifndef WEBSOCKETHANDLER_H__
#define WEBSOCKETHANDLER_H__
#include "mongoose.h"
#include "json.h"



class WebSocketHandler {
  public:

    const char* WS_ACTION_NONE="None";
    const char* WS_ACTION_HELLO="Hello";
    const char* WS_ACTION_TEXT="Text";
    const char* WS_ACTION_GRAPH="Graph";
    
    WebSocketHandler ();

    ~WebSocketHandler() ;

 protected:

    bool ProcessWebSocketPacket (struct mg_connection *nc,   struct websocket_message *wm );

    bool SendWebSocketPacket (struct mg_connection * nc, const Json::Value root );
    bool SendWebSocketPacket (struct mg_connection * nc, const std::string text );
    bool SendWebSocketPacket (struct mg_connection * nc, const struct mg_str msg);
    bool BroadcastWebSocketPacket(struct mg_connection *nc, const struct mg_str msg);

    void AddWebSocketConnection(struct mg_connection *nc);
    void RemoveWebSocketConnection(struct mg_connection *nc);

    std::string TaggedString (struct mg_connection *nc, std::string text);

    struct mg_connection * LookupConnection (std::string action );

    // For now just hard code the actionhandler.   
    virtual bool ProcessTextAction( struct mg_connection *nc, Json::Value root ) {return false; }
    virtual bool ProcessGraphAction( struct mg_connection *nc, Json::Value root ) {return false; }
    bool ProcessHelloAction( struct mg_connection *nc, Json::Value root );   // Ping / health check

  private:
    std::map<struct mg_connection*, std::string> mWsConnections; 
};

#endif