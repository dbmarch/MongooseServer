#ifndef ROUTER_H__
#define ROUTER_H__

#include "vector"
#include "Route.h"
#include "mongoose.h"


class Router {
public:

  Router();
  Router(Router&& a); // Move constructor
  Router(const Router&) = default;
  Router& operator=(const Router&) = default;
  Router& operator=(Router&& a);// Move assignment

  virtual ~Router();
  

  void AddRoute (Route &&route);

  bool ProcessRoute (struct mg_connection *nc, struct mg_http_message *hm);

  void ClearRoutes();

protected:
  bool mTrace{false};
  bool mDebug{true};

  std::vector <Route> mRoutes;    // all of the routes in our router
  std::string mRouteParams{};     // Currently support 1 route param per route.  (Should be a vector)

  std::string GetRouteParams() const {return mRouteParams;}   // Retrieves the Route Params for the backend
  std::string GetQueryParam(const std::string &key); // Retrieve the first  params
  std::vector<std::string> GetQueryParamVector(const std::string &key); // Retrieve a list of all the query params

  void ParseQueryParams(const std::string &queryLine); // parse the query params

  std::string mPath{};  // URI path

  std::multimap<std::string, std::string> mQueryParams; // URI Query Params with key duplicates
};

#endif