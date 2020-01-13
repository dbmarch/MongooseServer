#ifndef MONGOOSEWEBSERVER_H__
#define MONGOOSEWEBSERVER_H__

#include <string>
#include <vector>
#include <map>



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
    
  using RouteCallback = bool (*)(struct mg_connection *nc, struct http_message *hm) ;

  Route (Route::Verb v, std::string route, RouteCallback handler);

  ~Route() ;
  
  bool IsMatch (Route::Verb verb , std::string route);
  bool IsMatch (std::string verb , std::string route);
 
  std::string VerbToString(Route::Verb) const ;
  Route::Verb GetVerb(std::string s);

  bool Process (struct mg_connection *nc, struct http_message *hm);

 protected:
   Verb mVerb;
   std::string mRoute;
   RouteCallback mHandler;


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