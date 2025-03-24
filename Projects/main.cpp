#include "App/App.hpp"

int
main(int argc, char *argv[]) {
  string modelPath;
  uint32 subdivisions = 0;
  if (argc != 3) {
    std::cout << "Required command line arguments are:\n" <<
        "\t1. Path to a 3D model, this project uses *assimp* for model loading, thus almost any format should work.\n"
        <<
        "\t2. Number of subdivisions, resulting scene will have the size of 2^n. *Note: any value higher than 10 will require a significant amount of memory*\n";

    std::cout << "\n Enter the model path:\n";
    std::cin >> modelPath;
    std::cout << "\n Enter number of subdivisions:\n";
    std::cin >> subdivisions;
  } else {
    modelPath = argv[1];
    subdivisions = std::stoi(argv[2]);
  }


  App app;

  if (app.Open()) {
    app.SetArgs(modelPath, subdivisions);
    app.Run();
    app.Close();
  }

  return 0;
}
