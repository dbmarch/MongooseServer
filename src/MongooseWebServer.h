#include "mongoose.h"


class MongooseWebServer {

public:

  MongooseWebServer ();

  ~MongooseWebServer ();

  void StartServer();

  static void EventHandler(struct mg_connection *c, int ev, void *p);


 protected:
  const char *s_http_port {"8000"};

  struct mg_mgr mgr;
};