#ifndef BACKEND_H__
#define BACKEND_H__

#include "json.h"
#include "reader.h"
#include "value.h"

#include "Router.h"

class Backend : public Router
{
public:
  Backend();
  virtual ~Backend() = default;
  Backend(const Backend&) = delete;    
  Backend& operator=(const Backend& r) = delete;

protected:
  static constexpr char const * JSON_HEADERS {
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "Content-Type: application/json\r\n"
  };
  static constexpr char const * TEXT_HEADERS {
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "Content-Type: text/plain\r\n"
  };

  virtual void AddRoutes();

  // mimeType should be "application/json", "text/plain" or something similar.  
  // MG will format the headers.
  void ServeFile (struct mg_connection *nc, struct mg_http_message *hm, std::string fileName, std::string mimeType);

  bool ExtractJsonFromBody(Json::Value & root, struct mg_connection *nc, struct mg_http_message *hm);
  
  void SendReply (struct mg_connection *nc, int httpCode);

  // mimetype = text/plain
  void SendReply (struct mg_connection *nc, int httpCode, std::string content);
  
  // mimetype = application/json
  void SendReply (struct mg_connection *nc, int httpCode,  Json::Value &root );

  // mimetype = text/plain
  void SendError (struct mg_connection *nc, int httpCode, std::string reason);

  // The one default route we provide
  bool HandleCors (struct mg_connection *nc, struct mg_http_message *hm);
};

#endif