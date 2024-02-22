#pragma once

/* ----- constant declaration ----- */
#define PASS_LENGTH 4

/* ----- type and struct declaration ----- */
typedef std::string PlayerId;
typedef std::string InstanceId;
typedef std::tuple<std::string, int ,int, std::string> GameLogItem;

struct Instance {
  long int create_at;
  PlayerId* player_ids;
  std::string pass;
  InstanceId instance_id;
  bool is_matched;
};

struct InstanceState {
  PlayerId player_ids[2];
  int board[64];
  PlayerId current_player;
  bool is_connected[2];
};

/* ----- prototype declaration ----- */
const Instance* search_Instance(Instance &instance, const std::string &pass = "", const InstanceId &instance_id = "");
const InstanceId create_instance(const std::string &pass, const PlayerId &player);
const std::string create_id();

const bool verify_pass(const std::string &pass);
const bool verify_instance_id(const std::string &instance_id);

long int getTimeStamp();

std::vector<int> verify_put_len(int &p, int &c, int m, int *b);
const int update_board(int &p, std::shared_ptr<InstanceState> i, PlayerId &n);
const bool verify_put(int &p, std::shared_ptr<InstanceState> i, PlayerId &n);
void init_board(int *b);