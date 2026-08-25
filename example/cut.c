#define CUT_IMPL
#include "../cut.h"

void exe_shared_config(CutUnit *unit)
{
    cut_unit_sources(unit, "src/my_math.c");
    cut_unit_includes(unit, "include");
    cut_unit_flags(unit, "-Wall", "-Wextra");
    cut_unit_defines(unit, "CUT_NO_DEV");
    cut_unit_libs(unit, "gmp");
}

int main(int argc, char **argv)
{
    cut_build_init();

    CutUnit app;
    cut_unit_init(&app, "example", CUT_UNIT_EXE);
    cut_unit_sources(&app, "src/main.c");
    exe_shared_config(&app);

    CutUnit test;
    cut_unit_init(&test, "test", CUT_UNIT_EXE);
    cut_unit_sources(&test, "src/test.c");
    exe_shared_config(&test);

    cut_build_add(&app, &test);

    return cut_build_run(argc, argv);
}
