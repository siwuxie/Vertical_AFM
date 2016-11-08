
#ifndef _STRING_H
#define _STRING_H
#include "stdint.h"
void    *memccpy(void * , const void * , int, size_t);

void    *memchr(const void *, int, size_t);
int      memcmp(const void *, const void *, size_t);
void    *memcpy(void * , const void * , size_t);
void    *memmove(void *, const void *, size_t);
void    *memset(void *, int, size_t);
char    *strcat(char * , const char * );
char    *strchr(const char *, int);
int      strcmp(const char *, const char *);
int      strcoll(const char *, const char *);
char    *strcpy(char * , const char * );
size_t   strcspn(const char *, const char *);

#endif /*_STRING_H */


