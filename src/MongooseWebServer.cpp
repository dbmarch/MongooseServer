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
 printf ("%s\n", __func__);
 std::memset (&s_http_server_opts, 0, sizeof(mg_serve_http_opts));
 s_http_server_opts.document_root = "client/build";
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::~MongooseWebServer
//-----------------------------------------------------------------------------
MongooseWebServer::~MongooseWebServer() 
{
  printf ("%s\n", __func__);
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::StartServer
//-----------------------------------------------------------------------------
void MongooseWebServer::StartServer() 
{
  struct mg_connection *nc;
  cs_stat_t st;

  mg_mgr_init(&mgr, this);   // This data is available nc->mgr->user_data

  nc = mg_bind(&mgr, s_http_port, EventHandler);
  if (nc ==  nullptr) {
    fprintf(stderr, "Unable to bind to %s\n", s_http_port);
    return ;
  }

  mg_set_protocol_http_websocket(nc);
  if (mg_stat(s_http_server_opts.document_root, &st) != 0) {
      fprintf(stderr, "%s", "Cannot find web_root directory, exiting\n");
      exit(1);
    }

  printf ("starting server\n");
  for (;;) {
    mg_mgr_poll(&mgr, 200);
  }
  mg_mgr_free(&mgr);

}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::ProcessRoute
//-----------------------------------------------------------------------------
bool MongooseWebServer::ProcessRoute (struct mg_connection *nc, struct http_message *hm) {
    return mRouter.ProcessRoute(nc, hm);
  }


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::EventHandler
//-----------------------------------------------------------------------------
void MongooseWebServer::EventHandler(struct mg_connection *nc, int ev, void *ev_data) {
  struct http_message *hm = (struct http_message *) ev_data;
  struct websocket_message *wm = (struct websocket_message *) ev_data;
 
  MongooseWebServer * pThis = static_cast<MongooseWebServer*>(nc->mgr->user_data);
  
  if (pThis == nullptr) {
    printf ("user_data null\n");
    return;
  }

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
    {
       printf ("%s MG_EV_HTTP_REQUEST\n", __func__);
       if (pThis->ProcessRoute(nc, hm)) {
         printf ("Route Processed\n");
       } else {
         mg_serve_http(nc, hm, pThis->GetServerOptions());  // Serves static content
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
      pThis->Broadcast(nc, mg_mk_str("++ joined"));
      break;

    case MG_EV_WEBSOCKET_FRAME: {
      printf ("%s MG_EV_WEBSOCKET_FRAME\n", __func__);
      struct mg_str d = {(char *) wm->data, wm->size};
      std::string s(d.p, d.len);
      printf ("RCV: %s\n", s.c_str());
      pThis->Broadcast(nc, d);
      break;
    }

    case MG_EV_WEBSOCKET_CONTROL_FRAME: {
      printf ("%s MG_EV_WEBSOCKET_CONTROL_FRAME\n", __func__);

      break;
    }

    case MG_EV_RECV:
      printf ("%s MG_EV_RECV\n", __func__);
      break;

    case MG_EV_SSI_CALL:
      pThis->HandleSsiCall(nc, (const char*) ev_data);
      break;

    case MG_EV_SEND:
      printf ("%s MG_EV_SEND\n", __func__);
      break;

    case MG_EV_CLOSE:
      printf ("%s MG_EV_CLOSE\n", __func__);
      if (pThis->IsWebsocket(nc)) {
        pThis->Broadcast(nc, mg_mk_str("-- left"));
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
void MongooseWebServer::HandleSsiCall(struct mg_connection *nc, const char *param){
  if (strcmp(param, "setting1") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting1);
  } else if (strcmp(param, "setting2") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting2);
  }
}


//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::IsWebsocket
//-----------------------------------------------------------------------------
int MongooseWebServer::IsWebsocket(const struct mg_connection *nc) {
  return nc->flags & MG_F_IS_WEBSOCKET;
}

//-----------------------------------------------------------------------------
// Function:  MongooseWebServer::Broadcast
//-----------------------------------------------------------------------------
void MongooseWebServer::Broadcast(struct mg_connection *nc, const struct mg_str msg) {
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
}