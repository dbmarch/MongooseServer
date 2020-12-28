#include <utility>
#include <exception>
#include <cstring>
#include <iostream>

#include "Backend.h"
#include "Route.h"

//-----------------------------------------------------------------------------
// Function: Backend::Backend()
//-----------------------------------------------------------------------------
Backend::Backend()
{
   if (mTrace) printf ("Backend::%s\n", __func__);
}


//-----------------------------------------------------------------------------
// Function: Backend::AddRoutes()
//-----------------------------------------------------------------------------
void Backend::AddRoutes()
{
   Route corsRoute ("*");
   corsRoute.Options(std::bind(&Backend::HandleCors, this, std::placeholders::_1, std::placeholders::_2));
   AddRoute(std::move(corsRoute));
};


//-----------------------------------------------------------------------------
// Function: Backend::HandleCors()
//-----------------------------------------------------------------------------
bool Backend::HandleCors (struct mg_connection *nc, struct mg_http_message *hm)
{
  mg_http_reply(nc, 200, "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
   "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Content-Type", "");
   return true;
}


//-----------------------------------------------------------------------------
// Function: Backend::ExtractJsonFromBody()
//-----------------------------------------------------------------------------
bool Backend::ExtractJsonFromBody(Json::Value & root, struct mg_connection *nc, struct mg_http_message *hm) {
  struct mg_str &body = hm->body;
  std::string msg(body.ptr, body.len);
  std::istringstream is(msg);
  bool valid{false};

  printf ("%s JSON RX: '%s'\n", __func__, msg.c_str());

  if (msg.size() == 0) {
      return false;
  }

  try {
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
      
    valid = Json::parseFromStream(rbuilder, is, &root, &errs);

    if (valid) {
      for (auto &it: root.getMemberNames()) {
        std::cout << it  << " : " <<  root[it] << " [" << typeid(root[it]).name() <<"]"<< std::endl;
      } 
    } else {
      printf ("Unable to parse the Json object: %s\n",errs.c_str());
    }
   } catch(const std::exception &ex) {
     printf ("BODY PARSE Exception %s\n", ex.what() );
   } catch (...) {
    printf ("BODY PARSE exception - uknown\n");
   }
   return valid;
}


//-----------------------------------------------------------------------------
// Function: Backend::SendReply()
//-----------------------------------------------------------------------------
void Backend::ServeFile (struct mg_connection *nc, struct mg_http_message *hm, std::string fileName, std::string mimeType) {
  mg_http_serve_file (nc, hm, fileName.c_str(), mimeType.c_str());
}


//-----------------------------------------------------------------------------
// Function: Backend::SendReply()
//-----------------------------------------------------------------------------
void Backend::SendReply (struct mg_connection *nc, int httpCode) {
  mg_http_reply(nc, httpCode, DEFAULT_HEADERS, "");
}


//-----------------------------------------------------------------------------
// Function: Backend::SendReply()
//-----------------------------------------------------------------------------
void Backend::SendReply (struct mg_connection *nc, int httpCode, std::string content ) {
  mg_http_reply(nc, httpCode, DEFAULT_HEADERS, content.c_str());
}

//-----------------------------------------------------------------------------
// Function: Backend::SendReply()
//-----------------------------------------------------------------------------
void Backend::SendReply (struct mg_connection *nc, int httpCode, Json::Value &root  ) {
  Json::StreamWriterBuilder builder;
  const std::string json_file = Json::writeString(builder, root);
  mg_http_reply(nc, httpCode, DEFAULT_HEADERS, json_file.c_str());
}

//-----------------------------------------------------------------------------
// Function: Backend::SendError()
//-----------------------------------------------------------------------------
void Backend::SendError (struct mg_connection *nc, int httpCode, std::string reason ) {
  mg_http_reply(nc, httpCode, DEFAULT_HEADERS, reason.c_str());
}



