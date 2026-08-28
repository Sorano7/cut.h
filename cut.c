#define CUT_IMPL
#include "cut.h"

int main(int argc, char **argv)
{
    cut_build_init();

    CutUnit test;
    cut_unit_init(&test, "test", CUT_UNIT_EXE);
    cut_unit_sources(&test, "tests/main.c");
    cut_unit_sources(&test, "tests/da.c", "tests/strings.c", "tests/flags.c");
    cut_unit_flags(&test, "-g", "-Wall", "-Wextra", "-Wno-override-init");

    cut_build_add(&test);

    return cut_build_run(argc, argv);
}
