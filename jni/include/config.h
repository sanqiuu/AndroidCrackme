#pragma once
//#define DEBUG
#if defined(DEBUG) 
#define print(format, ...) printf(format, ##__VA_ARGS__)
#define print_error(format, ...) printf(format, ##__VA_ARGS__)
#else
#define print(format, ...)
#define print_error(format, ...) printf("[ERROR]Don't cheat\n")
#endif
