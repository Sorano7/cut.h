#ifndef CUT_H
#define CUT_H

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    bool failed;
    int line;
    const char *msg;
} TestResult;

typedef void (*TestFn)(TestResult *__res);

typedef struct TestCase
{
    const char *name;
    TestFn fn;
    const char *file;
    int line;
    struct TestCase *next;
} TestCase;

void test_registry_add(const char *name, TestFn fn, const char *file, int line);

#define TEST(t)                                                         \
    static void t(TestResult *__res);                                   \
    static void __attribute__((constructor)) _register_test_##t(void) { \
        test_registry_add(#t, t, __FILE__, __LINE__);                   \
    }                                                                   \
    static void t(TestResult *__res)

typedef struct
{
    FILE *fdout;
} TestRunOpt;

void test_run_opt(TestRunOpt opt);
#define test_run(...) test_run_opt((TestRunOpt){ \
        .fdout=stdout, __VA_ARGS__})

#define TEST_RUN() int main(void) { test_run(); return 0; }

#define ASSERT(exp)                 \
    do {                            \
        if (!(exp)) {               \
            __res->failed = true;   \
            __res->line = __LINE__; \
            __res->msg = #exp;      \
            return;                 \
        }                           \
    } while (0)

#endif

// #define CUT_IMPLEMENTATION
#ifdef CUT_IMPLEMENTATION

static TestCase *test_registry_head = NULL;
static size_t test_registry_size = 0;

void test_registry_add(const char *name, TestFn fn, const char *file, int line)
{
    TestCase *new_test = malloc(sizeof(TestCase));
    new_test->file = file;
    new_test->line = line;
    new_test->fn = fn;
    new_test->name = name;

    new_test->next = test_registry_head;
    test_registry_head = new_test;
    test_registry_size++;
}

static bool __flatten_test_list(TestCase *head, size_t size, TestCase *out[size])
{
    size_t i = 0;
    while (head != NULL && i < size)
    {
        out[i++] = head;
        head = head->next;
    }

    return i == size && head == NULL;
}

void test_run_opt(TestRunOpt opt)
{
    size_t test_ran = 0;
    size_t test_failed = 0;

    if (test_registry_size > 0)
    {
        size_t total = test_registry_size;
        TestCase *tests[total];
        if (!__flatten_test_list(test_registry_head, total, tests))
            goto PrintResult;

        TestResult res;

        for (size_t i = 0; i < total; i++)
        {
            res = (TestResult){0};

            TestCase *t = tests[i];
            if (t == NULL) continue;

            fprintf(opt.fdout, "[%s:%d] \033[36m%s\033[0m ... ", 
                    t->file, t->line, t->name);

            test_ran++;
            t->fn(&res);

            if (res.failed)
            {
                // Clears backward and move cursor to col 1
                fprintf(opt.fdout, "\033[1K\033[1G");
                fprintf(opt.fdout, "[%s:%d] \033[36m%s\033[0m ... ", 
                        t->file, res.line, t->name);
                fprintf(opt.fdout, "\033[1;31mfailed\033[0m: %s\n", res.msg);
                test_failed++;
            }
            else
            {
                fprintf(opt.fdout, "\033[32mpassed\033[0m\n");
            }

            free(t);
            tests[i] = NULL;
        }
    }

PrintResult:
    fprintf(opt.fdout, "\nTotal: %zu, passed: %zu, failed: %zu\n", 
            test_ran, test_ran-test_failed, test_failed);
}

#endif
