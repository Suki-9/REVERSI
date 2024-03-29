#pragma once

#include <drogon/WebSocketController.h>
#include <drogon/PubSubService.h>
#include <drogon/HttpAppFramework.h>
#include <mutex>
#include <map>

/* ----- use modules include -----*/
#include "game_core_module.h"
#include "json_module.h"
#include "db_module.h"
#include "logger_module.h"

using namespace drogon;

/* ----- type and struct declaration ----- */
struct Subscriber{
  std::string chatRoomName_;
  drogon::SubscriberID id_;
};

class WebSocketChat : public drogon::WebSocketController<WebSocketChat>
{
  public:
    virtual void handleNewMessage(
      const WebSocketConnectionPtr &,
      std::string &&,
      const WebSocketMessageType &) override;

    virtual void handleConnectionClosed(
        const WebSocketConnectionPtr &
      ) override;

    virtual void handleNewConnection(
      const HttpRequestPtr &,
      const WebSocketConnectionPtr &
    ) override;

    bool verify_connection(std::shared_ptr<InstanceState> i);

    WS_PATH_LIST_BEGIN
    WS_PATH_ADD("/api/stream", Get);
    WS_PATH_LIST_END

  private:
    PubSubService<std::string> chatRooms_;
    std::map<std::string, std::shared_ptr<InstanceState>> instance_;
    std::mutex mtx_;
};


