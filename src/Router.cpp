
#include "Router.h"

//-----------------------------------------------------------------------------
// Function: Router::AddRoute
//-----------------------------------------------------------------------------
void Router::AddRoute(Route * route) {
  if (route) {
    mRoutes.push_back(route);
  }
}

//-----------------------------------------------------------------------------
// Function: Router::ProcessRoute
//-----------------------------------------------------------------------------
bool Router::ProcessRoute (struct mg_connection *nc, struct http_message *hm) {
  for (const auto route : mRoutes ) {
    std::string verb {hm->method.p, hm->method.len};
    std::string uri {hm->uri.p, hm->uri.len};
    
    printf ("Checking %s %s\n", verb.c_str(), uri.c_str());
    
    if (route->IsMatch(verb, uri)) {
      printf ("MATCH %s %s\n", verb.c_str(), uri.c_str());
      route->Process(nc,hm);
      return true;
    }

  }
  return false;
}