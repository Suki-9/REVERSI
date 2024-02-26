#pragma once

#include <drogon/drogon.h>

/* ----- use modules include -----*/
#include "db_module.h"
#include "logger_module.h"

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
  bool is_game_set;
};

/* ----- prototype declaration ----- */
namespace InstanceOperator {
  const Instance* search(Instance &instance, const std::string &pass = "", const InstanceId &instance_id = "");
  const InstanceId create(const std::string &pass, const PlayerId &player);
}

namespace Verify {
  const bool pass(const std::string &pass);
  const bool instance_id(const InstanceId &i);
  const bool player_id(const PlayerId &p, const std::shared_ptr<InstanceState> i);
}

namespace BoardOperator {
  const PlayerId judge(std::shared_ptr<InstanceState> i);
  const int update_board(int &p, std::shared_ptr<InstanceState> i, PlayerId &n);
  const bool verify_put(int &p, std::shared_ptr<InstanceState> i, PlayerId &n);
  std::vector<int> verify_put_len(int &p, int &c, int m, int *b);
  void init_board(int *b);
}

long int getTimeStamp();
const std::string create_id();
