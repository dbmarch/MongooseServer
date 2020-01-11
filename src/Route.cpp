#include "Route.h"

//-----------------------------------------------------------------------------
// Function: Route::Route
//-----------------------------------------------------------------------------
Route::Route ( Verb v, std::string route, RouteCallback handler) : 
  mVerb{v}, mRoute{route}, mHandler{handler}
  {
    printf ("Route created %s %s\n", VerbToString(mVerb).c_str(), mRoute.c_str());
  }
 

//-----------------------------------------------------------------------------
// Function: Route::~Route
//-----------------------------------------------------------------------------
Route::~Route () 
  {
  }

//-----------------------------------------------------------------------------
// Function: Route::Process
//-----------------------------------------------------------------------------
bool Route::Process(struct mg_connection *nc, struct http_message *hm) {
  if (mHandler) {
    return mHandler(nc,hm);
  }
  else
  {
    fprintf (stderr, "Invalid Route Handler\n");
  }
  return false;
}

//-----------------------------------------------------------------------------
// Function: Route::IsMatch
//-----------------------------------------------------------------------------
bool  Route::IsMatch(Route::Verb verb, std::string path) {
  if (verb == mVerb && path == mRoute) {
    return true;
  }
  return false;
}


 //-----------------------------------------------------------------------------
// Function: Route::IsMatch
//-----------------------------------------------------------------------------
bool Route::IsMatch(std::string verb, std::string path) {
  for (const auto &v : Route::mVerbMap) {
  if (verb == v.second )
    return IsMatch(v.first, path);
  }
  return false;
 }


 //-----------------------------------------------------------------------------
// Function: Route::VerbToString
//-----------------------------------------------------------------------------
std::string Route::VerbToString(Route::Verb v) const
 {
   std::string s;
   try {
     s = mVerbMap.at(v);
   } catch( std::exception & ex) {
     fprintf (stderr, "Error:  Verb not found  %s", ex.what());
   }
   return s;
 }


//-----------------------------------------------------------------------------
// Function: Route::GetVerb
//-----------------------------------------------------------------------------
Route::Verb Route::GetVerb(std::string s) {
  for (const auto &v : mVerbMap) {
    if (s == v.second )
      return v.first;
  }
  return Route::GET;
}