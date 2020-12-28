#include <utility>
#include <sstream>

#include "Route.h"

//-----------------------------------------------------------------------------
// Function: Route::Route
//-----------------------------------------------------------------------------
Route::Route ( std::string route) : 
  mRoute{route}
  {
    if (mTrace) printf ("Route::Route() %s\n", mRoute.c_str());
    mRouteTokens = ExtractRouteTokens(mRoute);   
  }
 

//-----------------------------------------------------------------------------
// Function: Route::~Route
//-----------------------------------------------------------------------------
Route::~Route () 
  {
  }



//-----------------------------------------------------------------------------
// Function: Route::Route
//-----------------------------------------------------------------------------
Route::Route (const Route &r) { 
  if (mTrace) printf("Route::Route&\n");
  if (this != &r)   {
      mRoute = r.mRoute;
      mGetCb = r.mGetCb;
      mPutCb = r.mPutCb;
      mPostCb = r.mPostCb;
      mDeleteCb = r.mDeleteCb;
      mOptionsCb = r.mOptionsCb;
      mRouteTokens = r.mRouteTokens;
      mExact = r.mExact;
  }
}


//-----------------------------------------------------------------------------
// Function: Route::Route
//-----------------------------------------------------------------------------
Route& Route::operator= (const Route &r)
{ 
  if (mTrace) printf("operator= (Route&)\n");
  if (this != &r) {
    mRoute = r.mRoute;
    mGetCb = r.mGetCb;
    mPutCb = r.mPutCb;
    mPostCb = r.mPostCb;
    mDeleteCb = r.mDeleteCb;
    mOptionsCb = r.mOptionsCb;
    mRouteTokens = r.mRouteTokens;
    mExact = r.mExact;
  }
  return *this;
}

//-----------------------------------------------------------------------------
// Function: Route::ExtractRouteTokens
//-----------------------------------------------------------------------------
std::vector<std::string> Route::ExtractRouteTokens(std::string route) { 
  std::stringstream ss{route};  // stream for parsing
  std::string token;      // tokens we parse
  std::vector<std::string> routeTokens;
  while (std::getline(ss, token, '/')){
    if (!token.empty() && token.size()) {
      routeTokens.push_back(token);
    }
  }
  return routeTokens;
}


//-----------------------------------------------------------------------------
// Function: Route::ProcessRoute
//-----------------------------------------------------------------------------
bool Route::ProcessRoute(struct mg_connection *nc, struct http_message *hm, std::string &path, std::string &routeParams ) {
  std::string verb {hm->method.p, hm->method.len};
  mRouteParams = std::string();
  if (mTrace) printf ("'%s' ProcessRoute %s %s\n", mRoute.c_str(), verb.c_str(), path.c_str());
  if (UriMatch(path))
  {
    printf("URI matches mRoute:%s uri:%s\n", mRoute.c_str(), path.c_str());
    try {
      Route::Verb routeAction = GetVerb(verb);
      switch (routeAction) {
        case Verb::GET:
        if (mGetCb) {
          if (mDebug) printf ("Get CB %s\n", mRoute.c_str());
          routeParams = mRouteParams;
          return mGetCb(nc, hm);
        }
        break;
        
        case Verb::POST:
          if (mPostCb) {
            if (mDebug) printf ("Post CB %s\n", mRoute.c_str());
            routeParams = mRouteParams;
            return mPostCb(nc, hm);
          }
          break;       

        case Verb::PUT:
          if (mPutCb) {
            if (mDebug) printf("Put CB %s\n", mRoute.c_str());
            routeParams = mRouteParams;
            return mPutCb(nc, hm);
          }
          break;            

        case Verb::DELETE:
          if (mDeleteCb) {
            if (mDebug) printf ("Delete CB %s\n", mRoute.c_str());
            routeParams = mRouteParams;
            return mDeleteCb(nc, hm);
          }
          break;            

        case Verb::OPTIONS:
          if (mOptionsCb) {
            if (mDebug) printf ("Options CB %s\n", mRoute.c_str());
            return mOptionsCb(nc, hm);
          }
          break;            
            
        default:
          case Verb::PATCH:
          if (mDebug) printf ("Unhandled %s\n", verb.c_str());
          break;
      }
    } catch(...) {
      if (mDebug) printf ("Route handler not found\n");
    }
  }
  return false;
}


//-----------------------------------------------------------------------------
// Function: Route::VerbToString
//-----------------------------------------------------------------------------
std::string Route::VerbToString(Route::Verb v) const  {
  std::string s;
  try {
    s = mVerbMap.at(v);
  } catch( std::exception & ex) {
    fprintf (stderr, "Error:  Verb not found  %s", ex.what());
  }
  return s;
}



//-----------------------------------------------------------------------------
// Function: Route::UriMatch
//-----------------------------------------------------------------------------
bool Route::UriMatch(std::string route) {
  std::vector<std::string> routeTokens{ExtractRouteTokens(route)};
  printf ("%s %s\n", __func__, route.c_str());
  if (!mRouteTokens.size()) {
      printf ("Bad route\n");
   } else if (!routeTokens.size()) {
     printf ("Bad Uri\n");
     return false;
   } else if (mRoute == std::string("*")) {
     printf ("Any Route found\n");
     return true;
   } else if (route == mRoute) {
     printf ("Exact Route found\n");
     return true;
   } else {
     // Look for routeparams, wildcards, etc.
     printf ("looking for routeParams\n");
     auto incomingRoute = routeTokens.begin();
     for (auto ourRoute : mRouteTokens) {
       if (incomingRoute == routeTokens.end()) {
         printf ("Out of tokens\n");
         return !mExact;   // Return false for exact match
       }
       printf ("Checking %s %s\n", ourRoute.c_str(), (*incomingRoute).c_str() );
       if (ourRoute != *incomingRoute) {
         try {
           if (ourRoute.at(0) == ':') {
             mRouteParams = *incomingRoute;
             std::string paramId = ourRoute.substr(1);
             // We should return a std::map<std::string, std::string>
             printf ("Route Param Found: %s:%s\n", paramId.c_str(), mRouteParams.c_str());
           } else {
             return false;               
           }
         } catch(...) {
           printf ("Invalid Route token\n");
           return false;
         }
       }
       ++incomingRoute;
    }
    return true;
  }
  return false;
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


//-----------------------------------------------------------------------------
// Function: Route::Get
//-----------------------------------------------------------------------------
void Route::Get(CallbackFcn cb) {
  mGetCb= cb;
}

//-----------------------------------------------------------------------------
// Function: Route::Put
//-----------------------------------------------------------------------------
void Route::Put(CallbackFcn cb) {
  mPutCb= cb;
}

//-----------------------------------------------------------------------------
// Function: Route::Post
//-----------------------------------------------------------------------------
void Route::Post(CallbackFcn cb) {
  mPostCb= cb;
}

//-----------------------------------------------------------------------------
// Function: Route::Delete
//-----------------------------------------------------------------------------
void Route::Delete(CallbackFcn cb) {
  mDeleteCb= cb;
}

//-----------------------------------------------------------------------------
// Function: Route::Options
//-----------------------------------------------------------------------------
void Route::Options(CallbackFcn cb) 
{
   mOptionsCb= cb;
}

