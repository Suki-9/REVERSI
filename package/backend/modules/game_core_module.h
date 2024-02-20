#pragma once

/* ----- constant declaration ----- */
#define PASS_LENGTH 4

/* ----- type and struct declaration ----- */
typedef std::string PlayerId;
typedef std::string InstanceId;
typedef std::tuple<std::string, int ,int, std::string> GameLogItem;

struct Instance {
  long int create_at;
  int current_player;
  PlayerId* player_ids;
  int* board;
  std::string pass;
  InstanceId instance_id;
  bool is_matched;
};

struct InstanceState {
  PlayerId* player_ids;
  int* board;
  std::string current_player;
  bool* is_connected;
};

/* ----- prototype declaration ----- */
const Instance* search_Instance(Instance &instance, const std::string &pass = "", const InstanceId &instance_id = "");
const InstanceId create_instance(const std::string &pass, const PlayerId &player);
const std::string create_id();
const bool verfy_pass(const std::string &pass);
const bool verfy_instance_id(const std::string &instance_id);
long int getTimeStamp();