#ifndef _WINCE_STDBOOL
#define _WINCE_STDBOOL

#define __bool_true_false_are_defined	1

#define HAS_STDBOOL

#ifndef __cplusplus

typedef unsigned char bool;
typedef bool _Bool;

#define false	0
#define true	1

#endif /* __cplusplus */

#endif /* _WINCE_STDBOOL */