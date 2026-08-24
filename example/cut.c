#define CUT_IMPL
#include "../cut.h"

int main(int argc, char **argv)
{
    cut_build_init();

    CutUnit app;
    cut_unit_init(&app, "example_app", CUT_UNIT_EXE);
    cut_unit_sources(&app, "src/main.c", "src/my_math.c");
    cut_unit_includes(&app, "include");
    cut_unit_flags(&app, "-Wall", "-Wextra");
    cut_unit_defines(&app, "CUT_NO_DEV");
    cut_unit_libs(&app, "gmp");

    cut_build_add(&app);
    return cut_build_run(argc, argv);
}
