#include "../cut.h"

#define CHECK_EQ(a, b) CUT_CHECK(sv_equal((a), (b)))

TEST(string_view_equal)
{
    char *a = "hello";

    StringView s1 = {a, 5};
    StringView s2 = {a, 5};

    CHECK_EQ(s1, s2);

    s1 = SV("");
    s2 = SV("");

    CHECK_EQ(s1, s2);
}

TEST(string_append_works)
{
    String s;
    str_init(&s);

    str_append(&s, "hello");
    CHECK_EQ(s, "hello");

    str_appendf(&s, ", world!");
    CHECK_EQ(s, "hello, world!");

    str_free(&s);
}

TEST(string_insert_works)
{
    String s;
    str_init_with(&s, "helo");
    str_insert(&s, 'l', 2);
    CHECK_EQ(s, "hello");
}

TEST(string_is_always_null_terminated)
{
    String s;
    str_init(&s);
    CUT_MUST(s.data[0] == '\0');

    str_appendf(&s, "hello");
    CUT_MUST(s.data[s.len] == '\0');

    str_append_view(&s, SV("hello"));
    CUT_MUST(s.data[s.len] == '\0');

    str_reset(&s);
    CUT_MUST(s.data[0] == '\0');

    str_free(&s);
}

TEST(convertion_to_str_view)
{
    // Fixed char array, where sizeof(char_arr) is length+1.
    char char_arr[] = "hello";
    StringView hello = {char_arr, 5};
    CHECK_EQ(hello, char_arr);

    // String literal
    CHECK_EQ(hello, "hello");

    // Char pointer
    char *cstr = "hello";
    const char *c_cstr = "hello";
    CHECK_EQ(hello, cstr);
    CHECK_EQ(hello, c_cstr);

    // String by value
    String s;
    str_init_with(&s, "hello");
    CHECK_EQ(hello, s);
    str_free(&s);

    // String by pointer
    String *sp = malloc(sizeof(String));
    str_init(sp);
    str_append_cstr(sp, cstr);
    CHECK_EQ(hello, sp);
    str_free(sp);
    free(sp);
}

TEST(string_view_shift_and_split_works)
{
    StringView s = SV("12345");
    StringView tok = sv_shift(&s, 2);
    CHECK_EQ(tok, "12");
    CHECK_EQ(s, "345");

    s = SV("first second third");
    tok = sv_split(&s, ' ');
    CHECK_EQ(tok, "first");
    CHECK_EQ(s, "second third");

    tok = sv_split(&s, ' ');
    CHECK_EQ(tok, "second");
    CHECK_EQ(s, "third");

    tok = sv_split(&s, ' ');
    CHECK_EQ(tok, "third");
    CHECK_EQ(s, "");
}

TEST(string_view_slice_works)
{
    StringView s = SV("123456789");
    StringView sl = sv_slice(s);
    CHECK_EQ(sl, s);

    sl = sv_slice(s, .from=4);
    CHECK_EQ(sl, "56789");

    sl = sv_slice(s, .to=4);
    CHECK_EQ(sl, "1234");

    sl = sv_slice(s, .from=5, .to=8);
    CHECK_EQ(sl, "678");
}

TEST(sv_trim_works)
{
    StringView s = SV("   hello   \n");
    StringView trimmed = sv_trim(s);
    CHECK_EQ(trimmed, "hello");
}

TEST(sv_starts_and_ends_with_works)
{
    StringView s = SV("hello, world!");
    CUT_CHECK(sv_startswith(s, SV("h")));
    CUT_CHECK(sv_startswith(s, SV("hel")));
    CUT_CHECK(!sv_startswith(s, SV("ello")));
    CUT_CHECK(sv_startswith(s, SV("")));

    CUT_CHECK(sv_endswith(s, SV("!")));
    CUT_CHECK(sv_endswith(s, SV("world!")));
    CUT_CHECK(!sv_endswith(s, SV("hello")));
    CUT_CHECK(sv_endswith(s, SV("")));
}
