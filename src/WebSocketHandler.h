#ifndef WEBSOCKETHANDLER_H__
#define WEBSOCKETHANDLER_H__
#include "mongoose.h"



class WebSocketHandler {
  public:

    enum MessageType {
       Chat,
       Chart,
    };
    
  WebSocketHandler ();

  ~WebSocketHandler() ;

  bool ProcessPacket (struct mg_connection *nc,   struct websocket_message *wm );


 protected:
  
};

#endif