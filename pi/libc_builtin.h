#ifndef NATIVE_FUNCS_H
#define NATIVE_FUNCS_H

/* Standard I/O functions */
int printf(const char* format, ...);
int sprintf(char* str, const char* format, ...);
int snprintf(char* str, int size, const char* format, ...);
int vprintf(const char* format, void* args);
int vsprintf(char* str, const char* format, void* args);
int vsnprintf(char* str, int size, const char* format, void* args);
int puts(const char* s);
int putchar(int c);

/* Memory manipulation functions */
int memcmp(const void* s1, const void* s2, int n);
void* memcpy(void* dest, const void* src, int n);
void* memmove(void* dest, const void* src, int n);
void* memset(void* s, int c, int n);
void* memchr(const void* s, int c, int n);

/* String manipulation functions */
char* strchr(const char* s, int c);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
int strlen(const char* s);
int strncmp(const char* s1, const char* s2, int n);
char* strncpy(char* dest, const char* src, int n);
char* strdup(const char* s);
char* _strdup(const char* s); // Alias for strdup
int strncasecmp(const char* s1, const char* s2, int n);
int strspn(const char* s, const char* accept);
int strcspn(const char* s, const char* reject);
char* strstr(const char* haystack, const char* needle);

/* Memory allocation functions */
void* malloc(int size);
void* realloc(void* ptr, int size);
void* calloc(int nmemb, int size);
void free(void* ptr);

/* Character classification and conversion functions */
int isupper(int c);
int isalpha(int c);
int isspace(int c);
int isgraph(int c);
int isprint(int c);
int isdigit(int c);
int isxdigit(int c);
int tolower(int c);
int toupper(int c);
int isalnum(int c);

/* String to number conversion functions */
int atoi(const char* str);
long strtol(const char* str, char** endptr, int base);
unsigned long strtoul(const char* str, char** endptr, int base);

/* Time functions */
int clock_gettime(int clk_id, void* tp);
int clock(void);

/* Emscripten-specific functions */
void* emscripten_memcpy_big(void* dest, const void* src, int n);

/* Exception handling functions */
void* __cxa_allocate_exception(int thrown_size);
void __cxa_begin_catch(void* exception_object);
void __cxa_throw(void* exception_object, void* type_info, void (*dest)(void*));

/* Abort functions */
void abort(void);
void abortStackOverflow(int size);
void nullFunc_X(int code);

#endif /* NATIVE_FUNCS_H */
