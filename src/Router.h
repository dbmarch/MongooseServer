#ifndef ROUTER_H__
#define ROUTER_H__

#include "vector"
#include "Route.h"
#include "mongoose.h"


class Router {
public:

  Router();
  Router(Router&& a); // Move constructor

  virtual ~Router();
  

  Router& operator=(Router&& a);// Move assignment
  // Router& operator=(const Router& a);

  void AddRoute (Route * route);

  bool ProcessRoute (struct mg_connection *nc, struct http_message *hm);

 protected:
  std::vector <Route*> mRoutes;

};

#endif