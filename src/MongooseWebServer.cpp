
#include "MongooseWebServer.h"


MongooseWebServer::MongooseWebServer() 
{}

MongooseWebServer::~MongooseWebServer() 
{}


void MongooseWebServer::StartServer() 
{
  struct mg_connection *c;

  mg_mgr_init(&mgr, this);
  c = mg_bind(&mgr, s_http_port, EventHandler);
  mg_set_protocol_http_websocket(c);

  printf ("starting server\n");
  for (;;) {
    mg_mgr_poll(&mgr, 1000);
  }

}



void MongooseWebServer::EventHandler(struct mg_connection *c, int ev, void *p) {
  struct http_message *hm = (struct http_message *) p;
  MongooseWebServer * pThis = static_cast<MongooseWebServer*>(c->user_data);
  switch (ev) {
    case MG_EV_HTTP_REQUEST:
       printf ("%s MG_EV_HTTP_REQUEST\n", __func__);;

      // We have received an HTTP request. Parsed request is contained in `hm`.
      // Send HTTP reply to the client which shows full original request.
      mg_send_head(c, 200, hm->message.len, "Content-Type: text/plain");
      mg_printf(c, "%.*s", (int)hm->message.len, hm->message.p);
      break;

    case MG_EV_RECV:
      printf ("%s MG_EV_RECV\n", __func__);
      break;

    default:
      printf ("%s UKNOWN %d\n", __func__, ev);
      pThis->Tick();
  }
}


void MongooseWebServer::Tick() {
  printf ("tick\n");
}