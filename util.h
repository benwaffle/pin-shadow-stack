#ifndef _UTIL_H_
#define _UTIL_H_

#include <string>

using namespace std;

// terminal colors
#define RED    "\e[41m"
#define GREEN  "\e[32m"
#define YELLOW "\e[33m"
#define BLUE   "\e[34m"
#define PURPLE "\e[35m"
#define RESET  "\e[0m"

extern int indent;
void pr_indent();
void saveio();
void fixio();
void die(string msg) __attribute__((noreturn));

#endif