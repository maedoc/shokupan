#include "cli_application.h"

int main(int argc, char* argv[]) {
    CliApplication app(argc, argv);
    return app.run();
}