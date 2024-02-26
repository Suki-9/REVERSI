#include "stream.h"


void WebSocketChat::handleNewMessage(
  const WebSocketConnectionPtr &wsConnPtr,
  std::string &&message,
  const WebSocketMessageType &type
) {
  auto &s = wsConnPtr->getContextRef<Subscriber>();
  Json::Value json_res;
  std::shared_ptr<InstanceState> instance_state;
  InstanceId i = s.chatRoomName_.substr(0, 10);
  PlayerId p = s.chatRoomName_.substr(10, 20);

  if (!(instance_.count(i))) {
    json_res["type"] = "err";
    json_res["body"]["err"] = "invalid instance_id";
    chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
    return;
  } else {
    instance_state = instance_[i];
  }

  if (type == WebSocketMessageType::Text) {
    Json::Value json_req;
    json_parse(json_req, message);

    std::string event_type = json_req["type"].asCString();
    mtx_.lock();
    if (event_type == "connection") {
      LOGGER("Stream", "event_type -> connection");

      if (Verify::player_id(p, instance_state)) {
        int p_s = (p != instance_state->player_ids[0]);
        if (instance_state->is_connected[p_s]) {
          LOGGER("Stream", "Room join " << p << " is ReConnection");

          if (WebSocketChat::verify_connection(instance_state)) {
            json_res["type"] = "reconnection";
            for (char i = 0; i - 64; json_res["body"]["board"][i++] = instance_state->board[i]) {}
            if (instance_state->current_player == p) {
              json_res["body"]["yourTurn"] = true;
            }
            chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
            mtx_.unlock(); return;
          } else {
            mtx_.unlock(); return;
          }
        } else {
          LOGGER("Stream", "Room join " << p << " is Connection");
          instance_state->is_connected[p_s] = true;

          if (WebSocketChat::verify_connection(instance_state)) {
            json_res["type"] = "start";
            for (char i = 0; i - 64; json_res["body"]["board"][i++] = instance_state->board[i]) {}

            Json::Value json_res_ = json_res;
            json_res_["body"]["yourTurn"] = true;
            for (int i = 0; i < 64; i++) {
              json_res_["body"]["board"][i] = BoardOperator::verify_put(i, instance_state, instance_state->current_player) ? 3 : instance_state->board[i];
            }

            json_res_["body"]["yourNum"] = (instance_state->current_player != instance_state->player_ids[0]) + 1;
            json_res["body"]["yourNum"] = (instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]] != instance_state->player_ids[0]) + 1;

            chatRooms_.publish(i + instance_state->current_player, json_stringify(json_res_));
            chatRooms_.publish(i + instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]] , json_stringify(json_res));
            mtx_.unlock(); return;
          } else {
            mtx_.unlock(); return;
          }
        }
      } else {
        json_res["type"] = "err";
        json_res["body"]["err"] = "Invalid player_id";
        ERR_LOGGER("Stream", "Invalid player_id " << p);
        chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
        mtx_.unlock(); return;
      }

    } else if (event_type == "put") {
      LOGGER("Stream", "event_type -> put");

      // if (!WebSocketChat::verify_connection(instance_state)) {
      //   json_res["type"] = "err";
      //   json_res["body"]["err"] = "Invalid request";
      //   ERR_LOGGER("Stream", "Invalid request." << p);
      //   chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
      //   mtx_.unlock(); return;
      // }

      int put = json_req["body"]["put"].asInt();

      if (BoardOperator::update_board(put, instance_state, p)) {
        json_res["type"] = "put";
        for (int i = 0;i < 64; i++) {
          json_res["body"]["board"][i] = instance_state->board[i];
        }

        Json::Value json_res_ = json_res;
        json_res_["body"]["yourTurn"] = true;
        instance_state->current_player = instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]];

        int t = 0;
        for (int i = 0; i < 64; i++) {
          if (BoardOperator::verify_put(i, instance_state, instance_state->current_player)) {
            json_res_["body"]["board"][i] = 3;
            t++;
          }
        }
        std::cout  << t << std::endl;
        if (!t) {
          instance_state->current_player = instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]];
          t = 0;
          for (int i = 0, t = 0; i < 64; i++) {
            if (BoardOperator::verify_put(i, instance_state, instance_state->current_player)) {
              json_res_["body"]["board"][i] = 3;
              t++;
            }
          }
          std::cout  << t << std::endl;
          if (!t) {
            LOGGER("Stream", "Game set");
            json_res["type"] = "gameSet";
            instance_state->is_game_set = true;

            PlayerId winner = BoardOperator::judge(instance_state);

            json_res["body"]["win"] = false;
            chatRooms_.publish(i + instance_state->player_ids[winner == instance_state->player_ids[0]] , json_stringify(json_res));

            json_res["body"]["win"] = true;
            chatRooms_.publish(i + winner, json_stringify(json_res));
  
            mtx_.unlock(); return;
          }
        }

        chatRooms_.publish(i + instance_state->current_player, json_stringify(json_res_));
        chatRooms_.publish(i + instance_state->player_ids[instance_state->current_player == instance_state->player_ids[0]] , json_stringify(json_res));
        mtx_.unlock(); return;
      } else {
        json_res["type"] = "err";
        json_res["body"]["err"] = "Invalid request.";
        chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
        mtx_.unlock(); return;
      }
    } else if (event_type == "end") {
      json_res["type"] = "gameSet";
      json_res["body"]["win"] = false;

      instance_state->is_game_set = true;

      PlayerId winner = BoardOperator::judge(instance_state);
      chatRooms_.publish(i + instance_state->player_ids[winner == instance_state->player_ids[0]] , json_stringify(json_res));
      json_res["body"]["win"] = true;
      chatRooms_.publish(i + winner, json_stringify(json_res));
      mtx_.unlock(); return;
    } else {
      json_res["type"] = "err";
      json_res["body"]["err"] = "This is a valid event";

      chatRooms_.publish(s.chatRoomName_, json_stringify(json_res));
      mtx_.unlock(); return;
    }
  }
}

void WebSocketChat::handleConnectionClosed(const WebSocketConnectionPtr &conn) {
  auto &s = conn->getContextRef<Subscriber>();
  chatRooms_.unsubscribe(s.chatRoomName_, s.id_);
  WARN_LOGGER("Stream", "Disconnected. \n  >> chatRoomName_: " << s.chatRoomName_ << "\n  >> id_: " << s.id_);
}

void WebSocketChat::handleNewConnection (
  const HttpRequestPtr &req,
  const WebSocketConnectionPtr &conn
) {
  InstanceId i_id = req->getParameter("roomId");
  PlayerId p_id = req->getParameter("playerId");
  Json::Value res;

  LOGGER("Stream", "New connection!\nClient Status - \n  >> roomId: " << i_id << "\n  >> playerId: " << p_id);

  bool verify = Verify::instance_id(i_id);
  res["type"] = "connection";
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
      BoardOperator::init_board(instance_state->board);
      instance_state->current_player = instance_state->player_ids[rand() % 1];
      instance_state->is_connected[0] = false;
      instance_state->is_connected[1] = false;
      instance_state->is_game_set = false;

      instance_[i_id] = std::move(instance_state);

      LOGGER("Stream", "Assign instance.\n >> instance_id: " << i_id);
    }
    mtx_.unlock();
    if (Verify::player_id(p_id , instance_[i_id])) {
      conn->send(json_stringify(res));
    } else {
      res["body"]["err"] = "invalid player_id";
      conn->send(json_stringify(res));
      conn->shutdown();
      return;
    }
  }

  Subscriber s;
  s.chatRoomName_ = i_id + p_id;
  s.id_ = chatRooms_.subscribe(s.chatRoomName_, [conn](
    const std::string &topic,
    const std::string &message
  ){conn->send(message);});
  conn->setContext(std::make_shared<Subscriber>(std::move(s)));
}

bool WebSocketChat::verify_connection(std::shared_ptr<InstanceState> i) {
  if (i->is_connected[0] && i->is_connected[1]) {
    LOGGER("Stream", "Success verify connection.\n  >> " << i->player_ids[0] << ": OK\n  >> " << i->player_ids[1] << ": OK");
    return 1;
  } else {
    WARN_LOGGER("Stream", "Failed verify connection.\n  >> " << i->player_ids[0] << ": " << (i->is_connected[0] ? "OK" : "Disconnected") << "\n  >> " << i->player_ids[1] << ": " << (i->is_connected[1] ? "OK" : "Disconnected"));
    return 0;
  }
}
