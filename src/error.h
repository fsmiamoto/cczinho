#ifndef __ERROR_H__
#define __ERROR_H__

void error(char *fmt, ...);
void error_at(char *input, char *loc, char *fmt, ...);

#endif
