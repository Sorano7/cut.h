#ifndef CUT_H
#define CUT_H

#include <assert.h>
#include <stdarg.h>
#include <vadefs.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

// Types of a TestEvent.
typedef enum
{
    // Informational
    TEST_EVENT_DEBUG,
    // Regular Failure
    TEST_EVENT_ERROR,
    // Fatail Failure
    TEST_EVENT_FATAL,
} TestEventType;

// An event emitted in a TestCase.
typedef struct
{
    TestEventType type;
    int line;
    char *message;
    const char *file;
} TestEvent;

// The context of a TestCase.
typedef struct
{
    size_t failure_count;
    TestEvent *events;
    size_t event_count;
    size_t event_capacity;
} TestCtx;

// A test function run by a TestCase.
typedef void (*TestFn)(TestCtx *__res);

// A test case. Node in a linked list.
typedef struct TestCase
{
    const char *name;
    TestFn run;
    const char *file;
    int line;
    struct TestCase *next;
} TestCase;


/************************************************
 * Test Declaration
 ************************************************/

// Add a test case to the global registry.
void test_registry_add(const char *name, TestFn fn, const char *file, int line);

// Test function definition. `__ctx` will be passed.
#define TEST(t) \
    static void t(TestCtx *test_context); \
    static void __attribute__((constructor)) _register_test_##t(void) { \
        test_registry_add(#t, t, __FILE__, __LINE__); \
    } \
    static void t(TestCtx *test_context)


/************************************************
 * Test Event Emission
 ************************************************/

// Emit a test event to the test result.
void test_event_emit(TestCtx *ctx, TestEvent te, const char *fmt, ...);
#define TE(t, msg, ...) test_event_emit(( \
    _Generic((test_context), \
        TestCtx:  &test_context, \
        TestCtx *: test_context)), \
    (TestEvent){.type=(t), .line=__LINE__, .file=__FILE__}, \
    (msg) __VA_OPT__(,) __VA_ARGS__)

#define TE_DEBUG(msg, ...) TE(TEST_EVENT_DEBUG, (msg),  __VA_ARGS__)
#define TE_ERROR(msg, ...) TE(TEST_EVENT_ERROR, (msg),  __VA_ARGS__)
#define TE_FATAL(msg, ...) TE(TEST_EVENT_FATAL, (msg),  __VA_ARGS__)


/************************************************
 * Debug Level
 ************************************************/

#define DEBUGF(fmt, ...) do { \
    TE_DEBUG(fmt, __VA_ARGS__); \
} while (0)

/************************************************
 * Error Level
 ************************************************/

#define CHECK(exp) do { \
    if (!(exp)) { TE_ERROR(#exp); } \
} while (0)

#define ERRORF(fmt, ...) do { \
    TE_ERROR(fmt, __VA_ARGS__); \
} while (0)

/************************************************
 * Fatal Level
 ************************************************/

#define MUST(exp) do { \
    if (!(exp)) { TE_FATAL(#exp); return ; } \
} while (0)

#define MUST_EQ(got, want) do { \
    if ((got) != (want)) { TE_FATAL(#got" != "#want); return; } \
} while (0)

#define FATALF(fmt, ...) do { \
    TE_FATAL(fmt, __VA_ARGS__); return; \
} while (0)


/************************************************
 * Test Run
 ************************************************/

// Options for a test run.
typedef struct
{
    // The file descriptor to output to.
    FILE *fdout;
} TestRunOpt;

// Run tests with options.
void test_run_opt(TestRunOpt opt);
#define test_run(...) test_run_opt((TestRunOpt){ \
        .fdout=stdout, __VA_ARGS__})

// Alias for the main function to run all tests.
#define TEST_RUN() int main(void) { test_run(); return 0; }

#endif


#define CUT_IMPLEMENTATION
#ifdef CUT_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

// Global linked list of test cases.
static TestCase *test_registry_head = NULL;

// Count of registered tests.
static size_t test_registry_size = 0;

static TestCtx test_context = {0};

/**
 * Add a test case to the global registry.
 */
void test_registry_add(const char *name, TestFn fn, const char *file, int line)
{
    TestCase *new_test = malloc(sizeof(TestCase));
    new_test->file = file;
    new_test->line = line;
    new_test->run = fn;
    new_test->name = name;

    new_test->next = test_registry_head;
    test_registry_head = new_test;
    test_registry_size++;
}

static void test_ctx_reset(TestCtx *ctx)
{
    assert(ctx != NULL);

    if (ctx->events != NULL)
    {
        for (size_t i = 0; i < ctx->event_count; i++)
        {
            TestEvent e = ctx->events[i];
            if (e.message != NULL) free(e.message);
        }
    }

    ctx->event_capacity = 4;
    ctx->event_count = 0;
    ctx->failure_count = 0;
}

/**
 * Intialize the text ctx.
 */
static void test_ctx_init(TestCtx *ctx)
{
    assert(ctx != NULL);
    test_ctx_reset(ctx);
    ctx->events = malloc(sizeof(TestEvent) * ctx->event_capacity);
    assert(ctx->events != NULL);
}

/**
 * Add a test event to the ctx.
 */
static void test_ctx_add(TestCtx *ctx, TestEvent event)
{
    if (ctx == NULL) return;

    if (ctx->event_count+1 > ctx->event_capacity)
    {
        size_t new_cap = ctx->event_capacity * 2;
        TestEvent *new_data = realloc(ctx->events, new_cap);
        if (new_data == NULL) return;
        ctx->event_capacity = new_cap;
        ctx->events = new_data;
    }

    ctx->events[ctx->event_count++] = event;
}

/**
 * Emit a test event to the test result.
 */
void test_event_emit(TestCtx *ctx, TestEvent te, const char *fmt, ...)
{
    assert(ctx != NULL);

    va_list args;
    va_list args_copy;

    va_start(args, fmt);
    va_copy(args_copy, args);

    int size = vsnprintf(NULL, 0, fmt, args_copy);
    va_end(args_copy);

    if (size < 0)
    {
        va_end(args);
        return;
    }

    char *msg = malloc(size+1);
    if (msg == NULL)
    {
        va_end(args);
        return;
    }

    int result = vsnprintf(msg, size+1, fmt, args);
    if (result < 0)
    {
        free(msg);
        va_end(args);
        return;
    }

    te.message = msg;
    test_ctx_add(ctx, te);

    if (te.type == TEST_EVENT_ERROR || te.type == TEST_EVENT_FATAL)
        ctx->failure_count++;

    va_end(args);
}

/**
 * Flatten the linked list of test cases into an array.
 */
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

#define COLOR_RCT "\033[0m"
#define COLOR_FN "\033[36m"
#define COLOR_OK "\033[32m"
#define COLOR_SUB "\033[2m"
#define COLOR_DEBUG "\033[0m"
#define COLOR_ERROR "\033[33m"
#define COLOR_FATAL "\033[1;31m"

/**
 * Formatted print for TextCtx.
 */
static void test_context_print(TestCtx *ctx, FILE *fdout, const char *prefix)
{
    for (size_t ei = 0; ei < ctx->event_count; ei++)
    {
        TestEvent event = ctx->events[ei];
        if (event.type != TEST_EVENT_DEBUG && ctx->failure_count == 0)
            continue;

        fprintf(fdout, "%s", prefix);
        fprintf(fdout, COLOR_SUB"[%s:%d] "COLOR_RCT, event.file, event.line);
        switch (event.type)
        {
            case TEST_EVENT_DEBUG: 
                fprintf(fdout, COLOR_DEBUG"[DEBUG] "); 
                break;

            case TEST_EVENT_ERROR: 
                fprintf(fdout, COLOR_ERROR"[ERROR] "); 
                break;

            case TEST_EVENT_FATAL: 
                fprintf(fdout, COLOR_FATAL"[FATAL] "); 
                break;
        }
        fprintf(fdout, "%s"COLOR_RCT"\n", event.message);
    }
}

/**
 * Run tests with options.
 */
void test_run_opt(TestRunOpt opt)
{
    size_t test_ran = 0;
    size_t test_failed = 0;

    test_ctx_init(&test_context);

    if (test_registry_size > 0)
    {
        size_t total = test_registry_size;
        TestCase *all_tests[total];
        if (!__flatten_test_list(test_registry_head, total, all_tests))
            goto Summary;

        TestCtx ctx = {0};
        test_ctx_init(&ctx);

        for (size_t ti = 0; ti < total; ti++)
        {
            TestCase *test = all_tests[ti];
            if (test == NULL) continue;
            if (ctx.failure_count > 0) test_failed++;

            fprintf(opt.fdout, COLOR_SUB"[%s:%d] "COLOR_RCT, test->file, test->line);
            fprintf(opt.fdout, COLOR_FN"%s"COLOR_RCT" ... ", test->name);

            test_ran++;
            test->run(&ctx);

            if (ctx.failure_count > 0) fprintf(opt.fdout, COLOR_FATAL"failed\n"COLOR_RCT);
            else                       fprintf(opt.fdout, COLOR_OK   "passed\n"COLOR_RCT);

            test_context_print(&ctx, opt.fdout, "    ");

            free(test);
            all_tests[ti] = NULL;

            test_ctx_reset(&ctx);
        }
    }

    fprintf(opt.fdout, "\n");
    test_context_print(&test_context, opt.fdout, "");

Summary:
    fprintf(opt.fdout, "\nTotal: %zu, passed: %zu, failed: %zu\n", 
            test_ran, test_ran-test_failed, test_failed);
}

#endif
