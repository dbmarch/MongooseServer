#include <iostream>
#include <stdio.h>
#include <typeinfo>       // operator typeid
#include "MongooseWebServer.h"
#include "Router.h"
#include "json.h"



//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool HandleCors (struct mg_connection *nc, struct http_message *hm) {
  printf ("CORS ROUTE\n");
  mg_send_head(nc, 200, 0, "Access-Control-Allow-Methods: POST, GET, OPTIONS\r\n"
  "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Content-Type");
  return true;
}



//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool Hello (struct mg_connection *nc, struct http_message *hm) {
  printf ("HELLO ROUTE\n");
  // mg_printf(nc, "%s", "HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n");
  std::string hello("Hello");
  mg_send_head(nc, 200, hello.size(), "Content-Type: text/plain");
  mg_send(nc, hello.c_str(), hello.size());
  return true;
}

//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool JsonHello (struct mg_connection *nc, struct http_message *hm) {
  Json::Value root;
  Json::Value data;

  printf ("%s\n", __func__);

  root["message"] = "Hello";
  data["value"] = 3;
  data["name"] = "name";
  root["data"] = data;

  Json::StreamWriterBuilder builder;
  const std::string json_file = Json::writeString(builder, root);
  
  mg_send_head(nc, 200, json_file.size(), 
  "Access-Control-Allow-Origin: *\r\n"
  "Access-Control-Allow-Headers: Content-Type\r\n"
  "Content-Type: application/json"
   );
  mg_send(nc, json_file.c_str(), json_file.size());
  printf ("Sent CORS header\n");
  return true;
}


//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool HandleExamplePost (struct mg_connection *nc, struct http_message *hm) {
  printf ("%s\n", __func__);

  const struct mg_str *body = hm->query_string.len > 0 ? &hm->query_string : &hm->body;
  std::string msg(body->p, body->len);

  printf ("Rx JSON:  %s\n", msg.c_str());


  std::istringstream is(msg);
  Json::Value root;

  try {
    Json::CharReaderBuilder rbuilder;
    rbuilder["collectComments"] = false;
    std::string errs;
    
    bool ok = Json::parseFromStream(rbuilder, is, &root, &errs);
    if (ok) {
      for (auto &it: root.getMemberNames()) {
        std::cout << it  << " : " <<  root[it] << " [" << typeid(root[it]).name() <<"]"<< std::endl;
      }
    } else {
      printf ("Unable to parse the Json object: %s\n",errs.c_str());
    }
  } catch(const std::exception &ex) {
    printf ("Exception %s\n", ex.what() );
  } catch (...) {
    printf ("unknown exception\n");
  }

  mg_send_head(nc, 200,0,
  "Access-Control-Allow-Origin: *\r\n"
  "Access-Control-Allow-Headers: Content-Type\r\n"
  "Content-Type: application/json"
   );
  // mg_send(nc, json_file.c_str(), json_file.size());
  printf ("Ack POST\n");
  return true;
}


//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool HandleExampleGet (struct mg_connection *nc, struct http_message *hm) {
  Json::Value root;
  Json::Value data;

  printf ("%s\n", __func__);

  root["message"] = "Hello";
  data["value"] = 3;
  data["name"] = "name";
  root["data"] = data;

  Json::StreamWriterBuilder builder;
  const std::string json_file = Json::writeString(builder, root);
  
  mg_send_head(nc, 200, json_file.size(), 
  "Access-Control-Allow-Origin: *\r\n"
  "Access-Control-Allow-Headers: Content-Type\r\n"
  "Content-Type: application/json"
   );
  mg_send(nc, json_file.c_str(), json_file.size());
  printf ("Sent CORS header\n");
  return true;
}



//-----------------------------------------------------------------------------
// Function: main
//-----------------------------------------------------------------------------
int main(void) {
  Router r; // Set up the routes and provide them to the server.
  r.AddRoute(new Route (Route::OPTIONS, "*", HandleCors));
  r.AddRoute(new Route (Route::GET, "/hello", Hello));
  r.AddRoute(new Route (Route::GET, "/json", JsonHello));
  r.AddRoute(new Route (Route::POST, "/example", HandleExamplePost));
  r.AddRoute(new Route (Route::GET,  "/example",  HandleExampleGet));
  
  
  MongooseWebServer webServer(r);
  webServer.StartServer();

  return 0;
}