#define NOB_IMPLEMENTATION
#include "nob.h"

#define DEBUG

void cc(Nob_Cmd *cmd) {
    nob_cmd_append(cmd, "gcc");
#ifdef DEBUG
    nob_cmd_append(cmd, "-Wall", "-Wextra", "-ggdb");
#else
    nob_cmd_append(cmd, "-Wall", "-Wextra");
#endif // DEBUG
    nob_cmd_append(cmd, "-I./raylib/raylib-5.5_linux_amd64/include");
}

void libs(Nob_Cmd *cmd) {
    nob_cmd_append(cmd, "-Wl,-rpath=./raylib/raylib-5.5_linux_amd64/lib");
    nob_cmd_append(cmd, "-L./raylib/raylib-5.5_linux_amd64/lib");
    nob_cmd_append(cmd, "-l:libraylib.so", "-lm");
}

bool build_lib() {
    Nob_Cmd cmd = {0};

    cc(&cmd);
    nob_cmd_append(&cmd, "-c");
    nob_cmd_append(&cmd, "marsim_lib.c");
    libs(&cmd);

    return nob_cmd_run_sync(cmd);
}

bool build_main() {
    Nob_Cmd cmd = {0};

    cc(&cmd);
    nob_cmd_append(&cmd, "marsim_lib.o");
    nob_cmd_append(&cmd, "-o", "main");
    nob_cmd_append(&cmd, "marsim.c");
    libs(&cmd);

    return nob_cmd_run_sync(cmd);
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

	if (!build_lib()) return 1;
    if (!build_main()) return 1;
    else nob_log(NOB_INFO, "marsim.c -> 'main'");

    return 0;
}
