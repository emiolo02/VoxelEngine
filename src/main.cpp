#include "App/App.hpp"

int
main(int argc, char *argv[]) {
  App app;

  if (app.Open()) {
    app.Run();
    app.Close();
  }

  return 0;
}
