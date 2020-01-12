
#include "Router.h"

//-----------------------------------------------------------------------------
// Function: Router::Router
//-----------------------------------------------------------------------------
Router::Router()
{
  printf ("Router::Router\n");
	mRoutes.clear();
}



//-----------------------------------------------------------------------------
// Function: Router::Router
//-----------------------------------------------------------------------------
Router::Router(Router&& a)
		: mRoutes(a.mRoutes)
{
  printf ("Router&&\n");
	a.mRoutes.clear();
}


//-----------------------------------------------------------------------------
// Function: Router::~Router
//-----------------------------------------------------------------------------
Router::~Router() {
  printf ("%s  Removing %ld\n", __func__, mRoutes.size());
  for (const auto &route : mRoutes) {
    delete route;
  }
  mRoutes.clear();
}

//-----------------------------------------------------------------------------
// Function: Router::operator=
//-----------------------------------------------------------------------------
Router& Router::operator=(Router&& a) {
  printf ("operator=(Router&&)\n");
	// Self-assignment detection
	if (&a == this)
		return *this;

	// Release any resource we're holding
	mRoutes.clear();

	// Transfer ownership of a.m_ptr to m_ptr
	mRoutes = a.mRoutes;
	a.mRoutes.clear(); 

	return *this;
}

//-----------------------------------------------------------------------------
// Function: Router::operator=
//-----------------------------------------------------------------------------
// Router& Router::operator=(const Router& a) {
//   printf ("operator=(Router&)\n");

// 	// Self-assignment detection
// 	if (&a == this)
// 		return *this;
 
// 	// Release any resource we're holding
//   mRoutes.clear();
//   mRoutes = a.mRoutes;
// 	return *this;
// }

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