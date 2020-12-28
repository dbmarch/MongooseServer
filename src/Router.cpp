#include <utility>
#include <algorithm>

#include "Router.h"

//-----------------------------------------------------------------------------
// Function: Router::Router
//-----------------------------------------------------------------------------
Router::Router()
{
  if (mTrace) printf ("Router::Router\n");
	mRoutes.clear();
}

//-----------------------------------------------------------------------------
// Function: Router::Router
//-----------------------------------------------------------------------------
Router::Router(Router&& a)
		: mRoutes(a.mRoutes)
{
  if (mTrace) printf ("Router&&\n");
	a.mRoutes.clear();
}


//-----------------------------------------------------------------------------
// Function: Router::~Router
//-----------------------------------------------------------------------------
Router::~Router() {
  if (mTrace) printf ("%s  Removing %ld\n", __func__, mRoutes.size());
  mRoutes.clear();
}

//-----------------------------------------------------------------------------
// Function: Router::operator=
//-----------------------------------------------------------------------------
Router& Router::operator=(Router&& a) {
  if (mTrace) printf ("operator=(Router&&)\n");
	// Self-assignment detection
	if (&a != this)
  {
    mRoutes = a.mRoutes;
    a.mRoutes.clear();

  }
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Router::AddRoute
//-----------------------------------------------------------------------------
void Router::AddRoute(Route && route) {
  mRoutes.push_back(route);
  if (mDebug) printf ("%s %s TotalRoutes=%lu\n", __func__, route.GetRoute().c_str(), mRoutes.size());
}


//-----------------------------------------------------------------------------
// Function: Router::ClearRoutes
//-----------------------------------------------------------------------------
void Router::ClearRoutes () {
  mRoutes.clear();
  if (mDebug) printf ("%s\n", __func__);
}


//-----------------------------------------------------------------------------
// Function: Router::ProcessRoute
//-----------------------------------------------------------------------------
bool Router::ProcessRoute (struct mg_connection *nc, struct mg_http_message *hm) {
   bool rval{false};
   std::string verb {hm->method.ptr, hm->method.len};
   std::string uri {hm->uri.ptr, hm->uri.len};
   std::string query {hm->query.ptr, hm->query.len};
   if (uri == "/")
   {
      return false;  // Static content served by mongoose ( mongoose has a bug with regard to these URI's)
   }

   if (uri.size())
   {
      mPath = uri;
      if (mTrace) printf("%s Path '%s'\n", __func__, mPath.c_str());
   }

   ParseQueryParams(query);

   if (mTrace) printf ("Check Route %s %s\n", verb.c_str(), mPath.c_str());
   for (auto &route : mRoutes )  
   {
      if (route.ProcessRoute(nc, hm, mPath, mRouteParams))
      {
         if (mDebug) printf ("MATCH %s %s\n", verb.c_str(), mPath.c_str());
         rval = true;
         break;
      }
   }
   if (!rval)
   {
      if (mDebug) printf ("NOT FOUND %s %s\n", verb.c_str(), mPath.c_str());
   }
   return rval;
}


//-----------------------------------------------------------------------------
// Function: Router::ParseQueryParams
//-----------------------------------------------------------------------------
void Router::ParseQueryParams (const std::string & queryLine) 
{  
  // URI is of the form:   /path?queryName=queryValue&queryName2=queryValue#fragment
  mQueryParams.clear();
  try {
    std::string::const_iterator query_start = queryLine.begin();
    while (query_start != queryLine.end()) {
      std::string arg;  // We could have a number of query parameters separated with &
      std::string::const_iterator query_next = std::find(query_start, queryLine.end(), '&');
      arg.assign(query_start, query_next);
      std::string::iterator split = std::find(arg.begin(), arg.end(), '=');
         
      if (split != arg.end()) {   // an arg is of the form name=value
        std::string name;
        name.assign(arg.begin(), split);
        std::string value;
        ++split;
        if (split != arg.end()) {
          value.assign(split, arg.end());
        }
        std::pair<std::string,std::string> queryParam = std::make_pair(name, value);
        mQueryParams.insert(queryParam);   // Name, value pair
        if (mDebug) printf ("QueryParam '%s'='%s'\n", name.c_str(), value.c_str());
      }
      if (query_next != queryLine.end()) {
        ++query_next;  // go past the &
      }
      query_start = query_next;
    }
  } catch (std::exception &ex) {
    if (mDebug) printf ("Unable to parse the query params'%s'   error='%s'\n", queryLine.c_str(), ex.what());
  }

  if (mQueryParams.empty()) {
    if (mDebug) printf ("QueryParam: NONE\n");
  }
  for (auto item = mQueryParams.begin(); item != mQueryParams.end(); ++item) {
    if (mDebug) printf ("QueryParam '%s'='%s'\n", item->first.c_str(), item->second.c_str());
  }
}

//-----------------------------------------------------------------------------
// Function: Router::GetQueryParam
//-----------------------------------------------------------------------------
std::string Router::GetQueryParam (const std::string &key) {
  std::string param{};
  auto item = mQueryParams.find(key);
  if (item != mQueryParams.end()) {
    param = item->second;
  }
  if (mDebug) printf ("%s '%s'\n", __func__, param.c_str());
  return param;
}

//-----------------------------------------------------------------------------
// Function: Router::GetQueryParamVector
//-----------------------------------------------------------------------------
std::vector<std::string> Router::GetQueryParamVector (const std::string &key) {
  std::vector<std::string> paramVector{};
  for (auto it = mQueryParams.equal_range(key).first; it != mQueryParams.equal_range(key).second; ++it) {
    paramVector.push_back(it->second);
  }
  if (mDebug) printf ("%s %lu entries\n", __func__, paramVector.size());
  for (auto it = paramVector.begin(); it != paramVector.end(); ++it) {
    if (mDebug) printf("'%s'\n", it->c_str());
  }
  return paramVector;
}