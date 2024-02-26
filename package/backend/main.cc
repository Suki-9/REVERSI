#include <drogon/drogon.h>
#include <chrono>
#include <thread>

#include "db_module.h"
#include "modules/logger_module.h"

using namespace drogon;

int main() {
  std::thread th_a([](){
    while(1) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 * 60));
      LOGGER("[GarbageCollector]", "GarbageCollected...");
      DB_RUN("DELETE FROM game_instance WHERE is_matched = true;");
    }
  });
  app().addListener("0.0.0.0", 8000);
  app().loadConfigFile("../config.json");
  app().registerPostHandlingAdvice(
    [](const HttpRequestPtr &req, const HttpResponsePtr &res) {
      res->addHeader("Access-Control-Allow-Origin", "*");
    });
  app().run();
  

  return 0;
}
