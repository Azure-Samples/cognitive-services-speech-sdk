AgentTime
================

##Overview

AgentTime exports platform independent time related functions. It is a platform abstration and it requires a specific implementation for each platform.
Most of the times these functions can simply call the C standard time functions.

**SRS_AGENT_TIME_99_001: [** AGENT_TIME shall have the following interface**]**
```c
/* same functionality as time() of standard C function */
time_t get_time(time_t* p);

/* same functionality as gmtime() of standard C function */
struct tm* get_gmtime(time_t* currentTime)

/* similar functionality with mktime() of standard C function but it takes UTC as its parameter */
time_t get_mkgmtime(struct tm* gmTime)

/*the same as C's difftime*/
extern double get_difftime(time_t stopTime, time_t startTime);
```

time_t and struct tm are the ones defined in time.h of C standard. Value of time_t is platform dependent.
