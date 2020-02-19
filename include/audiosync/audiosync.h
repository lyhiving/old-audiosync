#pragma once

#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

// Information about the audio tracks. Both must have the same formats for
// the analysis to work.
#define NUM_CHANNELS 1
#define NUM_CHANNELS_STR "1"
#define SAMPLE_RATE 48000
#define SAMPLE_RATE_STR "48000"
// Conversion factor from the WAV samples with the used sample rate to
// milliseconds.
#define FRAMES_TO_MS (1000.0 / (double) SAMPLE_RATE)

// The value of the last interval in audiosync.c in seconds. This avoids
// having to run an itoa() for intervals[n_intervals-1] and simplifies it a
// bit.
#define MAX_SECONDS_STR "30"

// The minimum cross-correlation coefficient accepted.
#define MIN_CONFIDENCE 0.95

// Easily and consistently printing logs to stderr.
// The ## notation will ignore __VA_ARGS__ if no extra arguments were passed
// when calling the macro. This idiom will only work on gcc and clang.
#define log(str, ...) fprintf(stderr, "audiosync: " str "\n", ##__VA_ARGS__)

// Assertion that only takes place in debug mode. It helps prevent errors,
// while not affecting performance in a release.
#ifdef DEBUG
# define debug_assert(x) assert(x)
#else
# define debug_assert(x) do {} while(0)
#endif

// Easily ignoring warnings about unused variables. Some functions are
// callbacks, meaning that the parameters are required, but not all of them
// have to be used obligatorily.
#define UNUSED(x) (void)(x)


// Struct used to pass the parameters to the threads.
struct ffmpeg_data {
    char *title;
    double *buf;
    size_t total_len;
    size_t len;
    const size_t *intervals;
    const size_t n_intervals;
};
// The global status variable.
typedef enum {
    IDLE_ST,     // Audiosync is doing nothing, it's not running
    RUNNING_ST,  // Audiosync is running
    PAUSED_ST,   // Audiosync is paused (recording and downloading too)
    ABORT_ST     // Audiosync is stopping its algorithm completely
} global_status_t;
extern volatile global_status_t global_status;

// The global mutex and condition variables. They will be initialized from
// the main function.
extern pthread_mutex_t mutex;
// Condition used to know when either thread has finished one of their
// intervals.
extern pthread_cond_t interval_done;
// Condition used to continue the ffmpeg execution after it has been paused
// with audiosync_pause().
extern pthread_cond_t read_continue;

extern global_status_t audiosync_status();
extern char *status_to_string(global_status_t status);
extern void audiosync_abort();
extern void audiosync_pause();
extern void audiosync_resume();
extern int audiosync_setup(char *stream_name);
extern int audiosync_run(char *yt_title, long int *lag);