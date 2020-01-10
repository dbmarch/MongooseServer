#include <signal.h>
#include <exception>
#include <execinfo.h>

#include <cstring>
#include "MongooseWebServer.h"



MongooseWebServer::MongooseWebServer() 
{
 std::memset (&s_http_server_opts, 0, sizeof(mg_serve_http_opts));
 s_http_server_opts.document_root = "public";
}

MongooseWebServer::~MongooseWebServer() 
{
}


void MongooseWebServer::StartServer() 
{
  struct mg_connection *c;
  cs_stat_t st;

  mg_mgr_init(&mgr, this);   // This data is available nc->mgr->user_data

  c = mg_bind(&mgr, s_http_port, EventHandler);
  if (c ==  nullptr) {
    fprintf(stderr, "Unable to bind to %s\n", s_http_port);
    return ;
  }

  mg_set_protocol_http_websocket(c);
  if (mg_stat(s_http_server_opts.document_root, &st) != 0) {
      fprintf(stderr, "%s", "Cannot find web_root directory, exiting\n");
      exit(1);
    }

  printf ("starting server\n");
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }
  mg_mgr_free(&mgr);

}

void MongooseWebServer::EventHandler(struct mg_connection *nc, int ev, void *p) {
  struct http_message *hm = (struct http_message *) p;
  MongooseWebServer * pThis = static_cast<MongooseWebServer*>(nc->mgr->user_data);
  
  if (pThis == nullptr) {
    printf ("user_data null\n");
    return;
  }

  switch (ev) {
    case MG_EV_HTTP_REQUEST:
       printf ("%s MG_EV_HTTP_REQUEST\n", __func__);

       mg_serve_http(nc, hm, pThis->GetServerOptions());  // Serves static content

      // We have received an HTTP request. Parsed request is contained in `hm`.
      // Send HTTP reply to the client which shows full original request.
      // mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
      // mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
      break;

    case MG_EV_HTTP_CHUNK:
      printf ("%s MG_EV_HTTP_CHUNK\n", __func__);
      break;

    case MG_EV_RECV:
      printf ("%s MG_EV_RECV\n", __func__);
      break;
    case MG_EV_SSI_CALL:
      pThis->HandleSsiCall(nc, (const char*) p);
      break;

    default:
      printf ("%s UKNOWN %d\n", __func__, ev);
      break;
  }
}

void MongooseWebServer::HandleSsiCall(struct mg_connection *nc, const char *param){
  if (strcmp(param, "setting1") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting1);
  } else if (strcmp(param, "setting2") == 0) {
    mg_printf_html_escape(nc, "%s", s_settings.setting2);
  }
}

void MongooseWebServer::Tick() {
  printf ("tick\n");
}