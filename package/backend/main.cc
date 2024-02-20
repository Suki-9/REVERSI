#include <drogon/drogon.h>

int main() {
  drogon::app().addListener("0.0.0.0", 8000);
  drogon::app().loadConfigFile("../config.json");
  drogon::app().run();

  return 0;
}
