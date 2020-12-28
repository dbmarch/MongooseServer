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
MongooseWebServer::MongooseWebServer(Router &r) :
  mRouter {r}
{
 if (mTrace) printf ("%s\n", __func__);
 std::memset (&mHttpServerOpts, 0, sizeof(mg_serve_http_opts));
 mHttpServerOpts.document_root = "client/build";
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
  cs_stat_t st;

  mg_mgr_init(&mMgMgr, this);   // This data is available nc->mgr->user_data

  nc = mg_bind(&mMgMgr, mHttpPort, StaticEventHandler);
  if (nc ==  nullptr) {
    fprintf(stderr, "Unable to bind to %s\n", mHttpPort);
    return ;
  }

  mg_set_protocol_http_websocket(nc);
  if (mg_stat(mHttpServerOpts.document_root, &st) != 0) {
      fprintf(stderr, "%s", "Cannot find web_root directory, exiting\n");
      exit(1);
    }

  if (mTrace) printf ("Starting Server\n");
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
bool MongooseWebServer::ProcessRoute (struct mg_connection *nc, struct http_message *hm) {
    return mRouter.ProcessRoute(nc, hm);
  }

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::ProcessAction
//-----------------------------------------------------------------------------
bool MongooseWebServer::ProcessAction( std::string action, struct mg_connection * nc,  Json::Value args ) {
  if (mDebug) printf ("MongooseWebServer::%s %s\n", __func__, action.c_str());
  SendWebSocketPacket(nc, mg_mk_str("-- WS Packet Rx") );
  return true;
 }


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::StaticEventHandler
//-----------------------------------------------------------------------------
void MongooseWebServer::StaticEventHandler(struct mg_connection *nc, int ev, void *ev_data) {
 
  MongooseWebServer * pThis = static_cast<MongooseWebServer*>(nc->mgr->user_data);
  if (pThis) {
    pThis->EventHandler(nc,ev,ev_data);
  } else {
    printf ("%s user_data is invalid\n", __func__);
  }
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::EventHandler
//-----------------------------------------------------------------------------
void MongooseWebServer::EventHandler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;

  struct websocket_message *wm = (struct websocket_message *) ev_data;

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
    {
       printf ("%s MG_EV_HTTP_REQUEST\n", __func__);
       if (ProcessRoute(nc, hm)) {
         printf ("Route Processed\n");
       } else {
         std::string uri {hm->uri.p, hm->uri.len};
         printf ("Serve static content '%s'\n", uri.c_str());
         mg_serve_http(nc, hm, GetServerOptions());  // Serves static content
       } 
    }
      break;

    case MG_EV_HTTP_CHUNK:
      printf ("%s MG_EV_HTTP_CHUNK\n", __func__);
      break;

    case MG_EV_WEBSOCKET_HANDSHAKE_REQUEST: 
      printf ("%s MG_EV_WEBSOCKET_HANDSHAKE_REQUEST\n", __func__);
      break;

    case MG_EV_WEBSOCKET_HANDSHAKE_DONE:
      printf ("%s MG_EV_WEBSOCKET_HANDSHAKE_DONE\n", __func__);
      AddWebSocketConnection(nc);
      break;

    case MG_EV_WEBSOCKET_FRAME: {
      printf ("%s MG_EV_WEBSOCKET_FRAME\n", __func__);
      ProcessWebSocketPacket(nc, wm);
      break;
    }

    case MG_EV_WEBSOCKET_CONTROL_FRAME: {
      if (mTrace) printf ("%s MG_EV_WEBSOCKET_CONTROL_FRAME\n", __func__);
      break;
    }

    case MG_EV_RECV:
      if (mTrace) printf ("%s MG_EV_RECV\n", __func__);
      break;

    case MG_EV_SSI_CALL:
      // pThis->HandleSsiCall(nc, (const char*) ev_data);
      break;

    case MG_EV_SEND:
      if (mTrace) printf ("%s MG_EV_SEND\n", __func__);
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
      printf ("%s MG_EV_ACCEPT\n", __func__);
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
  return nc->flags & MG_F_IS_WEBSOCKET;
}