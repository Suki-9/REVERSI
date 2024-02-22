#include <drogon/drogon.h>

using namespace drogon;

int main() {
  app().addListener("0.0.0.0", 8000);
  app().loadConfigFile("../config.json");
  app().registerPostHandlingAdvice(
    [](const HttpRequestPtr &req, const HttpResponsePtr &res) {
      res->addHeader("Access-Control-Allow-Origin", "*");
    });
  app().run();

  return 0;
}
