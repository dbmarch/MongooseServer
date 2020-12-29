#include <iostream>
#include <utility>
#include "Services.h"


//-----------------------------------------------------------------------------
// Function: Services::AddRoutes
//-----------------------------------------------------------------------------
Services::Services () {
  AddRoutes();
}


//-----------------------------------------------------------------------------
// Function: Services::AddRoutes
//-----------------------------------------------------------------------------
void Services::AddRoutes() {

  Route helloRoute ("/hello");
  helloRoute.Get(std::bind(&Services::Hello, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(helloRoute));

  Route jsonHello ("/json");
  jsonHello.Get(std::bind(&Services::Hello, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(jsonHello));

  Route exampleRoute ("/example");
  exampleRoute.Post(std::bind(&Services::HandleExamplePost, this, std::placeholders::_1, std::placeholders::_2));
  exampleRoute.Get(std::bind(&Services::HandleExampleGet, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(exampleRoute));

  Route fileTextRoute ("/file/text");
  fileTextRoute.Get(std::bind(&Services::HandleFileGet, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(fileTextRoute));

  Route fileJsonRoute ("/file/json");
  fileJsonRoute.Get(std::bind(&Services::HandleFileGetJson, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(fileJsonRoute));

  Route fileGraph1 ("/file/graph1");
  fileGraph1.Get(std::bind(&Services::HandleFileGetGraph1, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(fileGraph1));

  Route fileGraph2 ("/file/graph2");
  fileGraph2.Get(std::bind(&Services::HandleFileGetGraph2, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(fileGraph2));

  Route fileLogfile ("/file/logfile");
  fileLogfile.Get(std::bind(&Services::HandleFileGetLogfile, this, std::placeholders::_1, std::placeholders::_2));
  AddRoute(std::move(fileLogfile));

  Backend::AddRoutes();
}

//-----------------------------------------------------------------------------
// Function: Services::Hello
//-----------------------------------------------------------------------------
bool Services::Hello (struct mg_connection *nc, struct mg_http_message *hm) {
  printf ("HELLO ROUTE\n");
  std::string hello("Hello");
  SendReply (nc, 200, hello);
  return true;
}

//-----------------------------------------------------------------------------
// Function: Services::JsonHello
//-----------------------------------------------------------------------------
bool Services::JsonHello (struct mg_connection *nc, struct mg_http_message *hm) {
  Json::Value root;
  Json::Value data;

  printf ("%s\n", __func__);

  root["message"] = "Hello";
  data["value"] = 3;
  data["name"] = "name";
  root["data"] = data;

  SendReply(nc, 200, root);
  return true;
}


//-----------------------------------------------------------------------------
// HandleExamplePost
// The client is posting a JSON object.  
// It has the following shape:
// 
// {
//    radioOption: "value1",
//    checkedOption: [
//                      0:  "cbvalue1",
//                      2:  "cbvalue2",
//                   ],
//    text:  "SomeText",
//    dropdownValue: "item4"
// }
//
//-----------------------------------------------------------------------------
bool Services::HandleExamplePost (struct mg_connection *nc, struct mg_http_message *hm) {
  // const struct mg_str *body = hm->query_string.len > 0 ? &hm->query_string : &hm->body;
  // std::string msg(body->p, body->len);
  // struct mg_str &body = hm->body;
  // std::string msg(body.ptr, body.len);

  // std::istringstream is(msg);
  Json::Value root;

  printf ("%s\n", __func__);

  if (!ExtractJsonFromBody(root, nc, hm)) {
    SendError(nc, 403, "Invalid Request");
  }
    
  printf ("Extracting data from the JSON object:\n");
  std::string textField (root["text"].asString());
  std::string dropdownField (root["dropdownValue"].asString());
  std::string radioOptionField (root["radioOption"].asString());
  std::vector<std::string> checkedOptionField;
  if (root["checkedOption"].isArray()) {
    int index{0};
    for (auto val : root["checkedOption"]) {
      checkedOptionField.push_back(val.asString());
      printf ("checkedOption[%d]: %s\n", index++, val.asCString());
    }
  }
   
  printf ("dropdownValue: %s\n", dropdownField.c_str());
  printf ("text: %s\n", textField.c_str());
  printf ("radioOption: %s\n", radioOptionField.c_str());

  // Our data is stored in our variables now.   We could perform standard back end tasks with this data.

  SendReply(nc, 200);
  printf ("Ack POST\n");
  return true;
}


//-----------------------------------------------------------------------------
// Function: Hello test route
//-----------------------------------------------------------------------------
bool Services::HandleExampleGet (struct mg_connection *nc, struct mg_http_message *hm) {
  Json::Value root;
  Json::Value data;

  printf ("%s\n", __func__);

  root["message"] = "Hello";
  data["value"] = 3;
  data["name"] = "name";
  root["data"] = data;

  SendReply(nc, 200, root);
  return true;
}

//-----------------------------------------------------------------------------
// Function: HandleFileGet
//-----------------------------------------------------------------------------
bool Services::HandleFileGet (struct mg_connection *nc, struct mg_http_message *hm) {

  printf ("%s\n", __func__);
  std::string fileName {"test-data/test.json"};
  
  // mg_http_serve_file(nc, hm, "test-data/test.json",
  //                           mg_mk_str("text/plain"), mg_mk_str(""));
  
  ServeFile (nc, hm, fileName, "application/json");                          
  printf ("Sent File: '%s'\n", fileName.c_str());
  return true;
}

//-----------------------------------------------------------------------------
// Function: HandleFileGetJson
//-----------------------------------------------------------------------------
bool Services::HandleFileGetJson (struct mg_connection *nc, struct mg_http_message *hm) {

  printf ("%s\n", __func__);
  std::string fileName {"test-data/test.json"};
  
  // mg_http_serve_file(nc, hm, fileName, mg_mk_str("application/json"), mg_mk_str(""));
  ServeFile (nc, hm, fileName, "application/json");
  printf ("Sent File: '%s'\n", fileName.c_str());
  return true;
}

//-----------------------------------------------------------------------------
// Function: HandleFileGetLogfile
//-----------------------------------------------------------------------------
bool Services::HandleFileGetLogfile (struct mg_connection *nc, struct mg_http_message *hm) {

  printf ("%s\n", __func__);
  std::string fileName {"test-data/logfile.txt"};
  
  ServeFile (nc, hm, fileName, "text/plain");

  printf ("Sent File: '%s'\n", fileName.c_str());
  return true;
}


//-----------------------------------------------------------------------------
// Function: HandleFileGetGraph1
//-----------------------------------------------------------------------------
bool Services::HandleFileGetGraph1 (struct mg_connection *nc, struct mg_http_message *hm) {

  printf ("%s\n", __func__);
  std::string fileName {"test-data/graph-data-1.json"};
  
  ServeFile(nc, hm, fileName, "application/json");

  printf ("Sent File: '%s'\n", fileName.c_str());
  return true;
}

//-----------------------------------------------------------------------------
// Function: HandleFileGetGraph1
//-----------------------------------------------------------------------------
bool Services::HandleFileGetGraph2 (struct mg_connection *nc, struct mg_http_message *hm) {
  printf ("%s\n", __func__);
  std::string fileName {"test-data/data.json"};

  printf ("Running Script\n");
  if (system ("scripts/CreateGraph.py") == 0) {
    printf ("Script executed successfully\n");
  }
  
  ServeFile(nc, hm, fileName, "application/json");

  printf ("Sent File: '%s'\n", fileName.c_str());
  return true;
}