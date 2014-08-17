#pragma once

#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>


// ---
// --- Printing
// ---
#define pr(str) printf("%s\n", str); fflush(stdout)
#define prf(x...) printf(x); printf("\n"); fflush(stdout)

#define db(str) pr(str); fflush(stdout)
#define dbf(x...) prf(x); fflush(stdout)

// ---
// --- Error/Warn
// ---
#define errf(x...) {fprintf(stderr, "FAILED! %s:%d: ", __FILE__, __LINE__); fprintf(stderr, x); fprintf(stderr, "\n"); exit(1);}

#define errif(cond) if((cond)) {fprintf(stderr, "FAILED! %s:%d: %s\n", __FILE__, __LINE__, #cond); exit(1);}

#define warn(str) fprintf(stderr, "WARNING! %s:%d: %s\n", __FILE__, __LINE__, str); fflush(stderr)
#define warnf(x...) fprintf(stderr, "WARNING! %s:%d: ", __FILE__, __LINE__); fprintf(stderr, x); fprintf(stderr, "\n"); fflush(stderr)


// ---
// --- Misc
// ---
void notify(const std::string &title, const std::string &message);

std::vector<std::string> split(const std::string &source, const char *delimiter = " ", bool keepEmpty = false);

