#ifndef SERVICES_H__
#define SERVICES_H__

#include "Backend.h"


class Services : public Backend {
public:
  Services ();
  virtual ~Services() = default;
  Services(const Services&) = delete;    
  Services& operator=(const Services& r) = delete;

  bool Hello (struct mg_connection *nc, struct mg_http_message *hm) ;
  bool JsonHello (struct mg_connection *nc, struct mg_http_message *hm) ;
  bool HandleExamplePost (struct mg_connection *nc, struct mg_http_message *hm) ;
  bool HandleExampleGet (struct mg_connection *nc, struct mg_http_message *hm) ;
  bool HandleFileGet (struct mg_connection *nc, struct mg_http_message *hm);
  bool HandleFileGetJson (struct mg_connection *nc, struct mg_http_message *hm);
  bool HandleFileGetLogfile (struct mg_connection *nc, struct mg_http_message *hm);
  bool HandleFileGetGraph1 (struct mg_connection *nc, struct mg_http_message *hm);

  bool HandleFileGetGraph2 (struct mg_connection *nc, struct mg_http_message *hm);

  bool HandleFileGetSignalGraph (struct mg_connection *nc, struct mg_http_message *hm);
  
protected:
  virtual void AddRoutes() override;


};

#endif