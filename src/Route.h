#ifndef ROUTE_H__
#define ROUTE_H__

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "mongoose.h"

class Route {
  public:
    enum Verb {
       GET,
       POST,
       PUT,
       PATCH,
       DELETE,
       OPTIONS,
    };
    
  using CallbackFcn = std::function<bool( struct mg_connection *nc, struct http_message *hm)>;

  Route (std::string route); 

  virtual ~Route() ;
  
  Route(const Route&); 
  Route& operator=(const Route& r);  // Copy Assignment

  void Get( CallbackFcn cb);   // Adds the GET callback
  void Post( CallbackFcn cb);  // Adds the GET callback
  void Put( CallbackFcn cb);   // Adds the GET callback
  void Delete(CallbackFcn cb);  // Provide the Delete Callback
  void Options(CallbackFcn cb);

  bool UriMatch(std::string uri);
  std::vector<std::string> ExtractRouteTokens(std::string route);

  std::string VerbToString(Route::Verb) const ;
  Route::Verb GetVerb(std::string s);

  bool ProcessRoute (struct mg_connection *nc, struct http_message *hm, std::string &path, std::string &routeParams);
  std::string GetRoute() const {return mRoute;}
  
 protected:
  bool mTrace{false};
  bool mDebug{true};
  
  std::string mRoute; // Our route

  bool mExact{true};  // Do we need exact match?

  std::vector<std::string> mRouteTokens{};
  
  std::string mRouteParams{};   // the route params

  CallbackFcn mGetCb{};
  CallbackFcn mPostCb{};
  CallbackFcn mPutCb{};
  CallbackFcn mDeleteCb{};
  CallbackFcn mOptionsCb{};
  const std::map<Verb, std::string> mVerbMap {
      {Route::GET,   "GET"},
      {Route::POST,  "POST"},
      {Route::PUT,   "PUT"},
      {Route::PATCH, "PATCH"},
      {Route::DELETE, "DELETE"},
      {Route::OPTIONS, "OPTIONS"}
    };

};

#endif