#include <cstring>
#include <signal.h>
#include <exception>
#include <execinfo.h>

#include <cstring>
#include "MongooseWebServer.h"
#include "Route.h"


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::MongooseWebServer
//-----------------------------------------------------------------------------
MongooseWebServer::MongooseWebServer(Router &r, 
    const std::string &rootFolder, 
    const std::string &serverUrl) :
  mRouter {r},
  mWebRootDir(rootFolder),
  mServerUrl(serverUrl)
{
  if (mTrace) printf ("%s\n", __func__);
}


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::~MongooseWebServer
//-----------------------------------------------------------------------------
MongooseWebServer::~MongooseWebServer() 
{
  if (mTrace) printf ("%s\n", __func__);
  StopServer();
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::StartServer
//-----------------------------------------------------------------------------
void MongooseWebServer::StartServer() 
{
  struct mg_connection *nc;
  // cs_stat_t st;

  mg_mgr_init(&mMgMgr);   // This data is available nc->mgr->user_data

  nc = mg_http_listen (&mMgMgr, mServerUrl.c_str(), StaticEventHandler, this);

  
  // mg_set_protocol_http_websocket(nc);
  // if (mg_stat(mHttpServerOpts.document_root, &st) != 0) {
  //     fprintf(stderr, "%s", "Cannot find web_root directory, exiting\n");
  //     exit(1);
  //   }

  if (mDebug) printf ("Starting Server on %s\n", mServerUrl.c_str());
  mServerThread = std::thread(&MongooseWebServer::MongooseEventLoop, this);
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::StopServer
//-----------------------------------------------------------------------------
void MongooseWebServer::StopServer() 
{
   mServerRunning = false;

  if (mServerThread.joinable()) {
     if (mTrace) printf("%s: Joining accept\n", __func__);
     mServerThread.join();
  }
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::ProcessRoute
//-----------------------------------------------------------------------------
bool MongooseWebServer::ProcessRoute (struct mg_connection *nc, struct mg_http_message *hm) {
    return mRouter.ProcessRoute(nc, hm);
  }

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::ProcessAction
//-----------------------------------------------------------------------------
bool MongooseWebServer::ProcessAction( std::string action, struct mg_connection * nc,  Json::Value args ) {
  if (mDebug) printf ("MongooseWebServer::%s %s\n", __func__, action.c_str());
  SendWebSocketPacket(nc, "-- WS Packet Rx");
  return true;
 }


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::StaticEventHandler
//-----------------------------------------------------------------------------
void MongooseWebServer::StaticEventHandler(struct mg_connection *nc, int ev, void *ev_data, void *fn_data) {
 
  MongooseWebServer * pThis = static_cast<MongooseWebServer*>(fn_data);
  if (pThis) {
    pThis->EventHandler(nc,ev,ev_data);
  } else {
    printf ("%s user_data is invalid\n", __func__);
  }
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::EventHandler
//
//   Event               Description                  ev_data represents
// 
//   MG_EV_ERROR,     // Error                        char *error_message
//   MG_EV_POLL,      // mg_mgr_poll iteration        unsigned long *millis
//   MG_EV_RESOLVE,   // Host name is resolved        NULL
//   MG_EV_CONNECT,   // Connection established       NULL
//   MG_EV_ACCEPT,    // Connection accepted          NULL
//   MG_EV_READ,      // Data received from socket    struct mg_str *
//   MG_EV_WRITE,     // Data written to socket       int *num_bytes_written
//   MG_EV_CLOSE,     // Connection closed            NULL
//   MG_EV_HTTP_MSG,  // HTTP request/response        struct mg_http_message *
//   MG_EV_WS_OPEN,    // Websocket handshake done     NULL
//   MG_EV_WS_MSG,     // Websocket message received   struct mg_ws_message *
//   MG_EV_MQTT_MSG,   // MQTT message                 struct mg_mqtt_message *
//   MG_EV_MQTT_OPEN,  // MQTT CONNACK received        int *connack_status_code
//   MG_EV_SNTP_TIME,  // SNTP time received           struct timeval *
//   MG_EV_USER,       // Starting ID for user events
//-----------------------------------------------------------------------------
void MongooseWebServer::EventHandler(struct mg_connection *nc, int ev, void *ev_data) {
  struct mg_http_message *hm = (struct mg_http_message *) ev_data;
  struct mg_ws_message *wm = (struct mg_ws_message *) ev_data;
  const char* text = (const char*) ev_data;

  switch (ev) {
    case MG_EV_HTTP_MSG: 
    {
       printf ("%s MG_EV_HTTP_REQUEST\n", __func__);
       if (ProcessRoute(nc, hm)) {
         printf ("Route Processed\n");
       } else {
         std::string uri {hm->uri.ptr, hm->uri.len};
         printf ("Serve static content '%s'\n", uri.c_str());
         mg_http_serve_dir(nc, hm, mWebRootDir.c_str());
       } 
    }
      break;

    case MG_EV_WS_OPEN: 
       printf ("%s MG_EV_WS_OPEN (HANDHAKE COMPLETE\n", __func__);
       AddWebSocketConnection(nc);
       break;

    case  MG_EV_WS_MSG: {
       printf ("%s MG_EV_WS_MSG\n", __func__);
       ProcessWebSocketPacket(nc, wm);
       break;
     }

    case MG_EV_ERROR: 
      printf ("%s MG_EV_ERROR '%s'\n", __func__, text);
      break;

    case MG_EV_CLOSE:
      printf ("%s MG_EV_CLOSE\n", __func__);
      if (IsWebsocket(nc)) {
        RemoveWebSocketConnection(nc);
      }
      break;
    
    case MG_EV_POLL:
      break;

    case MG_EV_ACCEPT:
      if (mg_url_is_ssl (mWebRootDir.c_str()))
      printf ("%s MG_EV_ACCEPT\n", __func__);
      break;


    case MG_EV_RESOLVE:
      printf ("%s MG_EV_RESOLVE  'Host Name Resolved\n", __func__);
      break;

    case MG_EV_MQTT_MSG: 
      printf ("%s MG_EV_MQTT_MSG\n", __func__);
      break; 
    
    case MG_EV_MQTT_OPEN: 
      printf ("%s MG_EV_MQTT_OPEN\n", __func__);
      break; 




    default:
      printf ("%s UKNOWN %d\n", __func__, ev);
      break;
  }
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::HandleSsiCall
//-----------------------------------------------------------------------------
// void MongooseWebServer::HandleSsiCall(struct mg_connection *nc, const char *param){
//   if (strcmp(param, "setting1") == 0) {
//     mg_printf_html_escape(nc, "%s", s_settings.setting1);
//   } else if (strcmp(param, "setting2") == 0) {
//     mg_printf_html_escape(nc, "%s", s_settings.setting2);
//   }
// }


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::MongooseEventLoop
//-----------------------------------------------------------------------------
void *MongooseWebServer::MongooseEventLoop()
{
  if (mDebug) printf("MongooseWebServer RUNNING\n");

  mServerRunning = true;
  while( mServerRunning ) {
     // let the cesanta mongoose server do its thing.
     mg_mgr_poll( &mMgMgr, mPollingInterval );
  }
   
  // the server is no longer running, so destroy it.
  // Close and deallocate all active connections.
  mg_mgr_free( &mMgMgr );

  if (mDebug) printf("MongooseWebServer STOPPED\n");
  return nullptr;
}



//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::IsWebsocket
//-----------------------------------------------------------------------------
int MongooseWebServer::IsWebsocket(const struct mg_connection *nc) {
  return nc->is_websocket;
}


