#include "api.h"


void api::post_create(
  const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback
) {
  bool is_err = false;
  Json::Value json_req;
  Json::Value json_res;
  json_parse(json_req, std::string(req->body()));

  const std::string pass = json_req["pass"].asCString();

  if (Verify::pass(pass)) {
    const PlayerId playerId = create_id();
    const InstanceId instance_id = InstanceOperator::create(pass, playerId);

    if (instance_id != "") {
      LOGGER("[API service]" , "create Instance id -> " << instance_id);
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
  const std::string pass = json_req["pass"].asCString();

  bool is_err = false;
  std::string p[2]{""};

  Instance instance;
  instance.player_ids = p;

  InstanceOperator::search(instance, pass);

  LOGGER("API service", "Search request. (pass=" << pass  << ")");

  if (instance.instance_id != "" && !(instance.is_matched)) {
    json_res["playerId"] = instance.player_ids[1];
    json_res["roomId"] = instance.instance_id;

    DB_RUN_WITH_VALUE(DB_UPDATE_QUERY("pass = '" + pass + "'", "is_matched"), true);
  } else {
    is_err = true;
    json_res["err"] = "Your request pass is Not found";
    ERR_LOGGER( "API service", "Err -> " << json_res["err"] );
  }

  auto res = HttpResponse::newHttpJsonResponse(json_res);
  res->setStatusCode(is_err ? k500InternalServerError : k200OK);
  callback(res);
}
