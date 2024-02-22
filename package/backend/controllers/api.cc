#include <iostream>
#include <compare>

#include "api.h"

/* ----- use modules include -----*/
#include "../modules/logger_module.h"
#include "json_module.h"
#include "game_core_module.h"
#include "db_module.h"


void api::post_create(
  const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback
) {
  Json::Value json_res;
  Json::Value json_req;
  json_parse(json_req, std::string(req->body()));
  std::string pass = json_req["pass"].asCString();

  bool is_err = false;

  if (verify_pass(pass)) {
    PlayerId playerId = create_id();
    InstanceId instance_id = create_instance(pass, playerId);

    if (instance_id != "") {
      json_res["roomId"] = instance_id;
      json_res["playerId"] = playerId;
    } else {
      is_err = true;
      json_res["err"] = "";
      ERR_LOGGER( "API service", "Err -> " << json_res["err"] );
    }
  } else {
    is_err = true;
    json_res["err"] = "Invalid pass error!";
    ERR_LOGGER( "API service", "Err -> " << json_res["err"] );
  }

  auto res = HttpResponse::newHttpJsonResponse(json_res);
  res->setStatusCode(is_err ? k500InternalServerError : k200OK);
  callback(res);
}

void api::post_find(
  const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback
) {
  Json::Value json_res;
  Json::Value json_req;
  json_parse(json_req, std::string(req->body()));
  std::string pass = json_req["pass"].asCString();

  bool is_err = false;

  std::string p[2]{""};

  Instance instance;
  instance.player_ids = p;

  search_Instance(instance, pass);

  LOGGER("API service", "Search request. (pass=" << pass  << ")");

  if (instance.instance_id != "" && !(instance.is_matched)) {
    instance.player_ids[1] = create_id();
    json_res["playerId"] = instance.player_ids[1];
    json_res["roomId"] = instance.instance_id;

    DB_RUN_WITH_VALUE(
      DB_UPDATE_QUERY("pass = '" + pass + "'", "player_ids", "is_matched"),
      VarConverter::arr2str_string(instance.player_ids, 2), true
    );

  } else {
    is_err = true;
    json_res["err"] = "Your request pass is Not found";
    ERR_LOGGER( "API service", "Err -> " << json_res["err"] );
  }

  auto res = HttpResponse::newHttpJsonResponse(json_res);
  res->setStatusCode(is_err ? k500InternalServerError : k200OK);
  callback(res);
}
