#pragma once

#include <signal.h>

void breakpoint_handler(int sig, siginfo_t *siginfo, void *raw_context);
