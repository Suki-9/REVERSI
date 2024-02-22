#include "stream.h"

#include <map>

/* ----- use modules include -----*/
#include "game_core_module.h"
#include "json_module.h"
#include "db_module.h"

void WebSocketChat::handleNewMessage(
  const WebSocketConnectionPtr &wsConnPtr,
  std::string &&message,
  const WebSocketMessageType &type
) {
  auto &s = wsConnPtr->getContextRef<Subscriber>();

  Json::Value json_res;
  InstanceId i = s.chatRoomName_.substr(0, 10);
  PlayerId p = s.chatRoomName_.substr(10, 20);

  std::shared_ptr<InstanceState> instance_state = instance_[i];

  if (type == WebSocketMessageType::Text) {
    Json::Value json_req;
    json_parse(json_req, message);

    std::string event_type = json_req["type"].asCString();
    if (event_type == "connection") {
      instance_state->is_connected[0] ? instance_state->is_connected[1] = true : instance_state->is_connected[0] = true;
      if (verify_connection(instance_state)) {
        json_res["type"] = "start";
        for (char i = 0; i - 64; json_res["body"]["board"][i++] = instance_state->board[i]) {}

        Json::Value json_res_ = json_res;
        json_res_["body"]["yourTurn"] = true;

        chatRooms_.publish(i + instance_state->current_player, json_stringify(json_res_));
        chatRooms_.publish(i + instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]] , json_stringify(json_res));
      }
    } else if (event_type == "put") {
      int put = json_req["body"]["put"].asInt();
      int placed = update_board(put, instance_state, p);
      if (placed) {
        json_res["type"] = "put";
        for (int i = 0;i < 64; i++) {
          json_res["body"]["board"][i] = instance_state->board[i];
        }

        Json::Value json_res_ = json_res;
        json_res_["body"]["yourTurn"] = true;

        instance_state->current_player = instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]];

        int t = 0;
        for (int i = 0; i < 64; i++) {
          t++; 
          json_res_["body"]["board"][i] = verify_put(i, instance_state, instance_state->current_player) ? 3 : instance_state->board[i];
        }
        if (!t) {
          instance_state->current_player = instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]];
          t = 0;
          for (int i = 0; i < 64; i++) {
            t++;
            json_res_["body"]["board"][i] = verify_put(i, instance_state, instance_state->current_player) ? 3 : instance_state->board[i];
          }
          if (!t) {
            for (int i = 0;i < 64; i++) {
              json_res["body"]["board"][i] = instance_state->board[i];
              json_res_["body"]["board"][i] = instance_state->board[i];
            }
            json_res["type"] = "end";
            json_res_["type"] = "end";
          }
        }

        chatRooms_.publish(i + instance_state->current_player, json_stringify(json_res_));
        chatRooms_.publish(i + instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]] , json_stringify(json_res));
      } else {
        json_res["type"] = "err";
        json_res["body"]["err"] = "This is a valid request.";
        chatRooms_.publish(i + p, json_stringify(json_res));
      }
    } else if (event_type == "end") {
      // 降参
    } else {
      json_res["type"] = "err";
      json_res["body"]["err"] = "This is a valid event";
    }
  }
}

void WebSocketChat::handleConnectionClosed(const WebSocketConnectionPtr &conn) {
  auto &s = conn->getContextRef<Subscriber>();
  chatRooms_.unsubscribe(s.chatRoomName_, s.id_);
}

void WebSocketChat::handleNewConnection (
  const HttpRequestPtr &req,
  const WebSocketConnectionPtr &conn
) {
  InstanceId i_id = req->getParameter("roomId");
  PlayerId p_id = req->getParameter("playerId");
  Json::Value res;

  bool verify = verify_instance_id(i_id);
  res["body"]["connection"] = verify;

  if (!verify) {
    res["body"]["err"] = "invalid roomId";
    conn->send(json_stringify(res));
    conn->shutdown();
  }
  else {
    mtx_.lock();
    if (!instance_.count(i_id)) {
      auto r = DB_RUN(DB_SELECT_QUERY("instance_id='" + i_id + "'", "player_ids"));
      std::unique_ptr<InstanceState> instance_state(new InstanceState);
      VarConverter::str2arr_string(r[0]["player_ids"].as<std::string>(), instance_state->player_ids);
      init_board(instance_state->board);
      instance_state->current_player = instance_state->player_ids[rand() % 1];
      //instance_state->is_connected = {0,0};
  
      instance_[i_id] = std::move(instance_state);
    }
    mtx_.unlock();
    if (!(p_id == instance_[i_id]->player_ids[0] || p_id == instance_[i_id]->player_ids[1])) {
      res["body"]["err"] = "invalid player_id";
      conn->send(json_stringify(res));
      conn->shutdown();
    } else {
      conn->send(json_stringify(res));
    }
  }

  Subscriber s;
  s.chatRoomName_ = i_id + p_id;
  s.id_ = chatRooms_.subscribe(s.chatRoomName_, [conn](
    const std::string &topic,
    const std::string &message
  ) {
    conn->send(message);
  });
  conn->setContext(std::make_shared<Subscriber>(std::move(s)));
}

bool verify_connection(std::shared_ptr<InstanceState> i) {
  return (i->is_connected[0] && i->is_connected[1]);
}

bool verify_put() {
  return true;
}
