#ifndef MONGOOSE_WEB_SERVER_H__
#define MONGOOSE_WEB_SERVER_H__

#include <vector>
#include <thread>

#include "mongoose.h"
#include "Router.h"
#include "WebSocketHandler.h"

class MongooseWebServer : public WebSocketHandler {

public:

  MongooseWebServer (Router &r);

  virtual ~MongooseWebServer ();

  MongooseWebServer(const MongooseWebServer&) = delete;    
  MongooseWebServer& operator=(const MongooseWebServer& r) = delete;

  static void StaticEventHandler(mg_connection *c, int ev, void *p);
  void EventHandler(mg_connection *c, int ev, void *p);

  virtual void StartServer();
  virtual void StopServer();

  std::string GetServerPort() const {return std::string(mHttpPort);}

  bool ProcessRoute (struct mg_connection *nc, struct http_message *hm);

  mg_serve_http_opts GetServerOptions() { return mHttpServerOpts;}

  // void HandleSsiCall(struct mg_connection *nc, const char *param);

  int IsWebsocket(const struct mg_connection *nc);

  // Inherit from WebSocketHandler
  virtual bool ProcessAction( std::string action, struct mg_connection * nc, Json::Value root ) override;  

  
protected:
  void * MongooseEventLoop();

  bool mTrace{false};
  bool mDebug{true};

  struct mg_serve_http_opts mHttpServerOpts;

  const char *mHttpPort {"8000"};
  unsigned short  mPollingInterval{100};   // Every 100msec

  struct mg_mgr mMgMgr;

  Router &mRouter;

  std::thread mServerThread;

  bool mServerRunning {false};
};

#endif