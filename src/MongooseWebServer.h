#ifndef MONGOOSE_WEB_SERVER_H__
#define MONGOOSE_WEB_SERVER_H__

#include <vector>
#include <thread>

#include "mongoose.h"
#include "Router.h"
#include "WebSocketHandler.h"

class MongooseWebServer : public WebSocketHandler {

public:

  MongooseWebServer (Router &r, const std::string &webServerRoot, const std::string &serverUrl);

  virtual ~MongooseWebServer ();

  MongooseWebServer(const MongooseWebServer&) = delete;    
  MongooseWebServer& operator=(const MongooseWebServer& r) = delete;

  static void StaticEventHandler(mg_connection *c, int ev, void *p, void *fn_data);
  void EventHandler(mg_connection *c, int ev, void *p);

  virtual void StartServer();
  virtual void StopServer();

  std::string GetServerPort() const;
  
  
  // Checks if we have the route.  If so, handle it.
  bool ProcessRoute (struct mg_connection *nc, struct mg_http_message *hm);

  // Checks if we have the static content.
  bool HaveStaticContent(const std::string &uri) const;

  // Returns true if this is a web socket.
  int IsWebsocket(const struct mg_connection *nc);

  // Inherit from WebSocketHandler
  virtual bool ProcessAction( std::string action, struct mg_connection * nc, Json::Value root ) override;  

protected:
  void * MongooseEventLoop();

  bool mTrace{false};
  bool mDebug{true};

// struct mg_serve_http_opts mHttpServerOpts;
  // Our router
  Router &mRouter;
  std::string mWebRootDir{"client/build"};
  std::string mServerUrl{"http://localhost:8000"};
  
  unsigned short  mPollingInterval{100};   // Every 100msec

  struct mg_mgr mMgMgr;

  // Server Thread
  std::thread mServerThread;  

  bool mServerRunning {false};
};

#endif