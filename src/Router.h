#ifndef ROUTER_H__
#define ROUTER_H__

#include "vector"
#include "Route.h"
#include "mongoose.h"


class Router {
public:

  Router() {}
  virtual ~Router() {}

  void AddRoute (Route * route);

  bool ProcessRoute (struct mg_connection *nc, struct http_message *hm);


 protected:
  std::vector <Route*> mRoutes;

};

#endif