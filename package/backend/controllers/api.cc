#include <iostream>
#include <compare>

#include "api.h"

/* ----- use modules include -----*/
#include "../modules/logger_module.h"
#include "game_core_module.h"
#include "db_module.h"


void api::post_create(
  const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback
) {
  std::string pass = req->getParameter("pass");
  Json::Value json;

  if (verfy_pass(pass)) {
    PlayerId playerId = create_id();
    InstanceId instance_id = create_instance(pass, playerId);
    if (instance_id != "") {
      json["roomId"] = instance_id;
      json["playerId"] = playerId;
    } else {
      json["err"] = "";
      ERR_LOGGER( "API service", "Err -> " << json["err"] );
    }
  } else {
    json["err"] = "Invalid pass error!";
    ERR_LOGGER( "API service", "Err -> " << json["err"] );
  }

  auto resp = HttpResponse::newHttpJsonResponse(json);
  resp->setStatusCode(k200OK);
  callback(resp);
}

void api::post_find(
  const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback
) {
  std::string pass = req->getParameter("pass");
  Json::Value json;

  std::string p[2]{""};
  int b[64]{0};

  Instance instance;
  instance.board = b;
  instance.player_ids = p;

  search_Instance(instance, pass);

  LOGGER("API service", "Search request. (pass=" << pass  << ")");

  if (instance.instance_id != "" && !(instance.is_matched)) {
    instance.player_ids[1] = create_id();
    json["playerId"] = instance.player_ids[1];
    json["roomId"] = instance.instance_id;

    DB_RUN_WITH_VALUE(
      DB_UPDATE_QUERY("pass = '" + pass + "'", "player_ids", "is_matched"),
      VarConverter::arr2str_string(instance.player_ids, 2), true
    );

  } else {
    json["err"] = "Your request pass is Not found";
    ERR_LOGGER( "API service", "Err -> " << json["err"] );
  }

  auto resp = HttpResponse::newHttpJsonResponse(json);
  resp->setStatusCode(k200OK);
  callback(resp);
}

