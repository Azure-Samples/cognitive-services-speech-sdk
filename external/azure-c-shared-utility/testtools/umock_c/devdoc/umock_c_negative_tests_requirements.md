#umock_c_negative_tests requirements
 
#Overview

umock_c_negative_tests is an addon to umock_c that exposes APIs used to automate negative tests.

#Exposed API

```c
    extern int umock_c_negative_tests_init(void);
    extern void umock_c_negative_tests_deinit(void);
    extern void umock_c_negative_tests_snapshot(void);
    extern void umock_c_negative_tests_reset(void);
    extern void umock_c_negative_tests_fail_call(size_t index);
    extern size_t umock_c_negative_tests_call_count(void);
```

##umock_c_negative_tests_init

```c
extern int umock_c_negative_tests_init(void);
```

**SRS_UMOCK_C_NEGATIVE_TESTS_01_001: [** umock_c_negative_tests_init shall initialize the umock_c negative tests library.**]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_004: [** On success, umock_c_negative_tests_init shall return 0. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_005: [** If the module has already been initialized, umock_c_negative_tests_init shall return a non-zero value. **]** 

##umock_c_negative_tests_deinit

```c
extern void umock_c_negative_tests_deinit(void);
```

**SRS_UMOCK_C_NEGATIVE_TESTS_01_002: [** umock_c_negative_tests_deinit shall free all resources associated with the negative tests module. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_003: [** If the module was not previously initialized, umock_c_negative_tests_deinit shall do nothing. **]**

##umock_c_negative_tests_snapshot

```c
extern void umock_c_negative_tests_snapshot(void);
```

**SRS_UMOCK_C_NEGATIVE_TESTS_01_006: [** umock_c_negative_tests_snapshot shall make a copy of the current call recorder for umock_c with all its recorded calls. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_007: [** The current call recorder shall be obtained by calling umock_c_get_call_recorder. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_008: [** The copy of the recorder shall be made by calling umockcallrecorder_clone. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_009: [** If getting the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_010: [** If copying the call recorder fails, umock_c_negative_tests_snapshot shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_015: [** If the module was not previously initialized, umock_c_negative_tests_snapshot shall do nothing. **]**

##umock_c_negative_tests_reset

```c
extern void umock_c_negative_tests_reset(void);
```

**SRS_UMOCK_C_NEGATIVE_TESTS_01_011: [** umock_c_negative_tests_reset shall reset the call recorder used by umock_c to the call recorder stored in umock_c_negative_tests_reset. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_012: [** If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_013: [** The reset shall be done by calling umock_c_set_call_recorder and passing the call recorder stored in umock_c_negative_tests_reset as argument. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_014: [** If umock_c_set_call_recorder fails, umock_c_negative_tests_reset shall indicate a failure via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_016: [** If the module was not previously initialized, umock_c_negative_tests_reset shall do nothing. **]**

##umock_c_negative_tests_fail_call

```c
extern void umock_c_negative_tests_fail_call(size_t index);
```

**SRS_UMOCK_C_NEGATIVE_TESTS_01_017: [** umock_c_negative_tests_fail_call shall call umockcallrecorder_fail_call on the currently used call recorder used by umock_c. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_018: [** The currently used recorder shall be obtained by calling umock_c_get_call_recorder. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_019: [** If umock_c_get_call_recorder fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_025: [** If failing the call by calling umockcallrecorder_fail_call fails, umock_c_negative_tests_fail_call shall indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_020: [** If the module was not previously initialized, umock_c_negative_tests_fail_call shall do nothing. **]**

##umock_c_negative_tests_call_count

```c
extern size_t umock_c_negative_tests_call_count(void);
```

XX**SRS_UMOCK_C_NEGATIVE_TESTS_01_021: [** umock_c_negative_tests_call_count shall return the count of expected calls for the current snapshot call recorder by calling umockcallrecorder_get_expected_call_count. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_022: [** If no call has been made to umock_c_negative_tests_snapshot, umock_c_negative_tests_fail_call shall return 0 and indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_023: [** If umockcallrecorder_get_expected_call_count fails, umock_c_negative_tests_fail_call shall return 0 and indicate the error via the umock error callback with error code UMOCK_C_ERROR. **]**
**SRS_UMOCK_C_NEGATIVE_TESTS_01_024: [** If the module was not previously initialized, umock_c_negative_tests_call_count shall return 0. **]**
