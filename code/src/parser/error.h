#ifndef INC_ERROR_H
#define INC_ERROR_H

#include <stdbool.h>

void error(int line, char* where, const char* message);
bool ThereWasAnError(void);

#endif //INC_ERROR_H
