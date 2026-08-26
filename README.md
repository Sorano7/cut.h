# cut.h

## Description
cut.h is a single-header library for writing build scripts and unit testing, and also comes with useful utilities such as dynamic arrays and string manipulation.

## Examples

You can find a slightly larger example in `example/`.

### Build Script

```c
// cut.c

#define CUT_IMPL
#include "cut.h"

int main(int argc, char **argv)
{
    cut_build_init(.cc="gcc");

    CutUnit app;
    cut_unit_init(&app, "my_app", CUT_UNIT_EXE);
    cut_unit_sources(&app, "main.c");

    cut_build_add(&app);

    return cut_build_run(argc, argv);
}
```

Bootstrap once with `cc cut.c -o cut`, then `./cut build my_app` to build the defined unit. The build script will automatically rebuild itself if the script has changed.

### Testing

```c
// test.c

#define CUT_IMPL
#include "cut.h"

TEST(one_plus_one_equals_three)
{
    int result = 1 + 1;
    CUT_CHECK(result == 3);
    CUT_DEBUG("result: %d", result);
}

TEST_RUN()
```

Output:

```
[test.c:4] one_plus_one_equals_three ... failed
    [test.c:7] [ERROR] result == 3
    [test.c:8] [DEBUG] result: 2

Total: 1, passed: 0, failed: 1
```

`TEST_RUN()` defines an entry point, so the test script should be built like any other executable.
