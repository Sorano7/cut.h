// Defining CUT_IMPL here does not affect other files.
#define CUT_IMPL
#include "../cut.h"

// You can abstract configurations by simply grouping them in a function.
void shared_config(CutUnit *unit)
{
    // *Append* a source. All cut_unit_* functions that take variadic arguments
    // append to the existing list. No NULL at the end is needed.
    cut_unit_sources(unit, "src/my_math.c");
    // No `-I` prefix needed. You don't need prefixes for functions whose
    // name clearly suggests one.
    cut_unit_includes(unit, "include");
    cut_unit_flags(unit, "-Wall", "-Wextra");
    cut_unit_defines(unit, "CUT_NO_DEV");
    cut_unit_libs(unit, "gmp");
}

int main(int argc, char **argv)
{
    // Initializes the build script and automatically rebuilds the executable.
    // Available options:
    // - .cc           the compiler to call, default `cc`
    // - .build_dir    the build directory, default `build`
    cut_build_init();

    CutUnit app;
    // Initializes with a name and type.
    // The name is used for the final output,
    // so this implies `-o <build_dir>/example`
    cut_unit_init(&app, "example", CUT_UNIT_EXE);
    cut_unit_sources(&app, "src/main.c");
    shared_config(&app);

    CutUnit test;
    cut_unit_init(&test, "test", CUT_UNIT_EXE);
    cut_unit_sources(&test, "src/test.c");
    shared_config(&test);

    // Add the units to the build.
    cut_build_add(&app, &test);

    // <cut> build <name>     build the unit as defined by the name
    // <cut> run <name>       build and run the unit in one go
    // <cut> clean            clean artifacts, including <cut>.old
    // <cut> rebuild <cmd>    force rebuild and run the command afterwards
    return cut_build_run(argc, argv);
}
