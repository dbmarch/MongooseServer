#include "mongoose.h"


class MongooseWebServer {

public:

  MongooseWebServer ();

  ~MongooseWebServer ();

  void StartServer();

  static void EventHandler(mg_connection *c, int ev, void *p);

  void Tick();

   mg_serve_http_opts GetServerOptions() { return s_http_server_opts;}

  void HandleSsiCall(struct mg_connection *nc, const char *param);

  struct mg_serve_http_opts s_http_server_opts;

 protected:
  const char *s_http_port {"8000"};

  struct mg_mgr mgr;

  struct device_settings {
    char setting1[100];
    char setting2[100];
  } s_settings = {"value1", "value2"};
};