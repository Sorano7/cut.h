#include "../cut.h"

#define START() \
    CutFlagParser fp; \
    cut_fp_init(&fp); \
    SVList args; da_init(&args);

#define PARSE(s, ...) do { \
    char *argv[] = {"./flags", __VA_ARGS__}; \
    int argc = sizeof(argv) / sizeof(char *); \
    CUT_MUST((s) == cut_fp_parse(&fp, argc, argv, &args).status); \
} while (0)

#define END() \
    cut_fp_free(&fp); \
    da_free(&args);

TEST(bool_flag_parses)
{
    START();
        bool flag = false;
        cut_fp_add_flag(&fp, &flag, SV("bool"), .short_name='b');

        PARSE(CUT_FP_OK, "--bool");
        CUT_CHECK(flag == true);

        flag = false;
        PARSE(CUT_FP_OK, "-b");
        CUT_CHECK(flag == true);
    END();
}

TEST(string_flag_parses)
{
    START();
        StringView flag = {0};
        cut_fp_add_flag(&fp, &flag, SV("str"), .short_name='s');

        PARSE(CUT_FP_OK, "--str", "my_str");
        CUT_CHECK(sv_equal(flag, "my_str"));

        flag = SV("");
        PARSE(CUT_FP_OK, "-s", "123");
        CUT_CHECK(sv_equal(flag, "123"));

        PARSE(CUT_FP_MISSING_VALUE, "--str");
    END();
}

TEST(int_flag_parses)
{
    START();
        int flag = 0;
        cut_fp_add_flag(&fp, &flag, SV("int"), .short_name='i');

        PARSE(CUT_FP_OK, "--int", "1");
        CUT_CHECK(flag == 1);

        PARSE(CUT_FP_OK, "-i", "12345");
        CUT_CHECK(flag == 12345);

        PARSE(CUT_FP_OK, "-i", "-500");
        CUT_CHECK(flag == -500);

        PARSE(CUT_FP_INVALID_VALUE, "--int", "abc");
        PARSE(CUT_FP_INVALID_VALUE, "--int", "2147483648");
        PARSE(CUT_FP_INVALID_VALUE, "--int", "-2147483649");
    END();
}

TEST(int_short_flag_no_value_increments)
{
    START();
        int flag = 0;
        cut_fp_add_flag(&fp, &flag, SV("flag"), .short_name='f');

        PARSE(CUT_FP_OK, "-f");
        CUT_CHECK(flag == 1);

        flag = 0;
        PARSE(CUT_FP_OK, "-f", "-f");
        CUT_CHECK(flag == 2);

        flag = 0;
        PARSE(CUT_FP_OK, "-fff");
        CUT_CHECK(flag == 3);
    END();
}

TEST(flag_like_value_consumed_unless_registered)
{
    START();
        StringView f = {0};
        StringView g = {0};
        cut_fp_add_flag(&fp, &f, SV("f"), .short_name='f');
        cut_fp_add_flag(&fp, &g, SV("g"), .short_name='g');

        PARSE(CUT_FP_OK, "--f", "--h");
        CUT_CHECK(sv_equal(f, "--h"));

        PARSE(CUT_FP_OK, "-g", "-h");
        CUT_CHECK(sv_equal(g, "-h"));

        PARSE(CUT_FP_MISSING_VALUE, "--f", "--g");
        PARSE(CUT_FP_MISSING_VALUE, "-f", "-g");
    END();
}

TEST(duplicate_flag_overrides)
{
    START();
        int flag = 0;
        cut_fp_add_flag(&fp, &flag, SV("flag"));

        PARSE(CUT_FP_OK, "--flag", "1", "--flag", "2");
        CUT_CHECK(flag == 2);
    END();
}

TEST(positional_arguments_independent_of_optional)
{
    START();
        bool flag = false;
        cut_fp_add_flag(&fp, &flag, SV("bool"));

        PARSE(CUT_FP_OK, "input", "--bool", "output");
        CUT_CHECK(flag == true);

        CUT_CHECK(sv_equal(da_at(&args, 0), "input"));
        CUT_CHECK(sv_equal(da_at(&args, 1), "output"));
    END();
}

TEST(double_dash_forces_positional_parsing)
{
    START();
        bool flag = false;
        cut_fp_add_flag(&fp, &flag, SV("flag"));

        PARSE(CUT_FP_OK, "abc", "--", "--flag");
        CUT_CHECK(flag == false);

        CUT_CHECK(sv_equal(da_at(&args, 0), "abc"));
        CUT_CHECK(sv_equal(da_at(&args, 1), "--flag"));
    END();
}

TEST(bool_or_int_short_can_group)
{
    START();
        bool f = false;
        bool g = false;
        int h = 0;

        cut_fp_add_flag(&fp, &f, SV("f"), .short_name='f');
        cut_fp_add_flag(&fp, &g, SV("g"), .short_name='g');
        cut_fp_add_flag(&fp, &h, SV("h"), .short_name='h');

        PARSE(CUT_FP_OK, "-fgh");
        CUT_CHECK(f = true);
        CUT_CHECK(g = true);
        CUT_CHECK(h == 1);
    END();
}

TEST(equal_sign_assigns_value)
{
    START();
        StringView flag = {0};
        cut_fp_add_flag(&fp, &flag, SV("flag"));

        PARSE(CUT_FP_OK, "--flag=value");
        CUT_CHECK(sv_equal(flag, "value"));
    END();
}
