#include "mongoose.h"

#include "Router.h"
class MongooseWebServer {

public:

  MongooseWebServer (Router &r);

  ~MongooseWebServer ();

  static void EventHandler(mg_connection *c, int ev, void *p);

  void StartServer();

  bool ProcessRoute (struct mg_connection *nc, struct http_message *hm);

  mg_serve_http_opts GetServerOptions() { return s_http_server_opts;}

  void HandleSsiCall(struct mg_connection *nc, const char *param);

  int IsWebsocket(const struct mg_connection *nc);

  void Broadcast(struct mg_connection *nc, const struct mg_str msg);

  struct mg_serve_http_opts s_http_server_opts;

 protected:
  const char *s_http_port {"8000"};

  struct mg_mgr mgr;

  Router &mRouter;

  struct device_settings {
    char setting1[100];
    char setting2[100];
  } s_settings = {"value1", "value2"};
};