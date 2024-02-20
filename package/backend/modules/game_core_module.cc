#include <drogon/drogon.h>

#include "game_core_module.h"

/* ----- use modules include -----*/
#include "db_module.h"
#include "logger_module.h"


const InstanceId create_instance(
  const std::string& pass,
  const PlayerId& player
) {
  try {
    int b[64]{0};
    std::string p[3]{player, "", create_id()};
    DB_RUN_WITH_VALUE(
      DB_INSERT_QUERY("create_at", "player_ids", "board", "pass", "instance_id"),
      getTimeStamp(), VarConverter::arr2str_string(p, 2), VarConverter::arr2str_int(b, 64), pass, p[2]
    );
    return p[2];

  } catch (const drogon::orm::DrogonDbException &e) {
    ERR_LOGGER("GameCoreModule", e.base().what());
    return "";

  };
}

const Instance* search_Instance(
  Instance& instance,
  const std::string& pass,
  const InstanceId& instance_id
) {
  if (pass != "") {
    auto result = DB_RUN(DB_SERECT_QUERY("pass = '" + pass + "'", "pass", "is_matched", "player_ids", "instance_id"));
    if (result.size()) {
      VarConverter::str2arr_string(result[0]["player_ids"].as<std::string>(), instance.player_ids);
      instance.instance_id = result[0]["instance_id"].as<std::string>();
      instance.is_matched = result[0]["is_matched"].as<bool>();
      instance.pass = result[0]["pass"].as<std::string>();
    }
  }

  if(instance_id != "") {
    auto result = DB_RUN(DB_SERECT_QUERY("instance_id = '" + instance_id + "'", "pass", "is_matched", "player_ids", "instance_id"));
    if (result.size()) {
      VarConverter::str2arr_string(result[0]["player_ids"].as<std::string>(), instance.player_ids);
      instance.instance_id = result[0]["instance_id"].as<std::string>();
      instance.is_matched = result[0]["is_matched"].as<bool>();
      instance.pass = result[0]["pass"].as<std::string>();
    }
  }
  return &instance;
}

const std::string create_id() {
  std::string i;
  int x;
  for(long int t = getTimeStamp(); t; t /= 36) {
    x = t % 36;
    i = (char)(x < 10 ? '0' + x : 'A' + x - 10) + i;
  }
  return i += (char)('A' + rand() % 26), i += (char)('A' + rand() % 26);
}

const bool verfy_pass(const std::string& pass) {
  return (pass.length() == PASS_LENGTH && !DB_RUN(DB_SERECT_QUERY("pass = '" + pass + "'", "pass")).size());
}

const bool verfy_instance_id(const std::string& instance_id) {
  auto rows = DB_RUN(DB_SERECT_QUERY("instance_id = '" + instance_id + "'", "is_matched"));
  return (rows.size() && rows[0]["is_matched"].as<bool>());
}

long int getTimeStamp() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
