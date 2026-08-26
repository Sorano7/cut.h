#include "../cut.h"

typedef struct
{
    int *data;
    size_t len;
    size_t cap;
} IntList;

typedef struct
{
    IntList *data;
    size_t len;
    size_t cap;
} IntListList;

typedef struct
{
    char *data;
    size_t len;
    size_t cap;
} CharList;

TEST(da_init_and_reserve_works)
{
    IntList il;
    da_init(&il);
    CUT_CHECK(il.data != NULL);
    CUT_CHECK(il.len == 0);
    CUT_CHECK(il.cap == DA_DEFAULT_CAP);
    free(il.data);

    CharList cl;
    da_init(&cl);
    CUT_CHECK(cl.data != NULL);
    CUT_CHECK(cl.len == 0);
    CUT_CHECK(cl.cap == DA_DEFAULT_CAP);
    free(cl.data);

    IntListList ill;
    da_reserve(&ill, 1024);
    CUT_CHECK(ill.data != NULL);
    CUT_CHECK(ill.len == 0);
    CUT_CHECK(ill.cap == 1024);
    free(ill.data);
}

TEST(da_append_works)
{
    IntList il;
    da_reserve(&il, 1);
    da_append(&il, 100);
    CUT_MUST(il.len == 1);
    CUT_CHECK(il.data[0] == 100);

    da_append(&il, 200);
    CUT_MUST(il.len == 2);
    CUT_CHECK(il.cap == 2);
    CUT_CHECK(il.data[1] == 200);

    IntListList ill;
    da_init(&ill);
    da_append(&ill, il);
    CUT_CHECK(ill.data[0].data == il.data);
    CUT_CHECK(ill.data[0].len == il.len);
    CUT_CHECK(ill.data[0].cap == il.cap);
}

TEST(da_appendn_works)
{
    IntList il;
    da_init(&il);
    int arr[] = {1,2,3,4,5};
    da_appendn(&il, arr, 5);
    CUT_MUST(il.len == 5);

    da_for(&il, i)
        CUT_CHECK(il.data[i] == arr[i]);
}

TEST(da_reset_and_free_works)
{
    CharList cl;
    da_init(&cl);
    da_append(&cl, 'a');
    CUT_CHECK(cl.len == 1);
    CUT_MUST(cl.data[0] == 'a');

    da_reset(&cl);
    CUT_MUST(cl.data != NULL);
    CUT_CHECK(cl.len == 0);

    da_append(&cl, 'b');
    CUT_MUST(cl.data[0] == 'b');

    da_free(&cl);
    CUT_MUST(cl.data == NULL);
}

static int succ(int x) { return x + 1; }
static void succ_mut(int *x) { *x += 1; }

TEST(da_map_works)
{
    IntList il;
    da_init(&il);
    int arr[] = {1,2,3,4,5};
    da_appendn(&il, arr, 5);

    da_map(&il, int, succ);

    da_for(&il, i)
        CUT_CHECK(il.data[i] == arr[i]+1);

    da_map_mut(&il, int, succ_mut);

    da_for(&il, i)
        CUT_CHECK(il.data[i] == arr[i]+2);
}
