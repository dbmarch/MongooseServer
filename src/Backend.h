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
  static constexpr char const * DEFAULT_HEADERS {
    "Access-Control-Allow-Headers: Content-Type\r\n"
    "Content-Type: application/json"
  };

  virtual void AddRoutes();

  bool ExtractJsonFromBody(Json::Value & root, struct mg_connection *nc, struct http_message *hm);
  
  void SendReply (struct mg_connection *nc, int httpCode);

  void SendReply (struct mg_connection *nc, int httpCode, std::string content);

  void SendError (struct mg_connection *nc, int httpCode, const char* reason);

  // The one default route we provide
  bool HandleCors (struct mg_connection *nc, struct http_message *hm);
};

#endif