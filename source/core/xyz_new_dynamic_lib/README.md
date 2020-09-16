# xyz_new_dynamic_lib

This is an example carbon dynamic module. Use directory contents to quickly
create a new dynamic module for carbon using copy/paste/search/replace.

## Creating a new dynamic library

Steps to create a new dynamic library (e.g. named `abc_dynamic_lib`)
1. Make `abc_dynamic_lib` directory under `carbon/source/core`
2. Copy `xyz_new_dynamic_lib` contents to `carbon/source/core/abc_dynamic_lib`
3. Update `carbon/source/core/abc_dynamic_lib/CMakeLists.txt` by...
   - Replacing all occurrences of `xyz_new_dynamic_lib` with `abc_dynamic_lib`
4. Update `carbon/source/core/CMakeLists.txt` for `xyz_new_dynamic_lib` by...
   - Duplicating each line containing `xyz_new_dynamic_lib` (move in sorted order)
   - Replacing `xyz_new_dynamic_lib` with `abc_dynamic_lib` in each duplicated line
5. Update `carbon/source/*.cpp;*.h` by...
   - Duplicating each line containing `XyzNewDynamicLib_CreateModuleObject`
   - Replacing `XyzNewDynamicLib_CreateModuleObject` with `AbcDynamicLib_CreateModuleOjbect`
6. Rebuild

## Exposing new static libraries to this new dynamic library

Steps to expose a static library (e.g. named `QprStaticLib`)
1. Update `abc_dynamic_lib/CMakeLists.txt` by...
   - Duplicating each line containing `xyz_new_static_lib` (move in sorted order)
   - Replacing `xyz_new_static_lib` with `qpr_static_lib`
   - Uncommenting each new line as required
2. Update `abc_dynamic_lib/create_module_object.cpp` by...
   - Duplicating each line containing `SomeOtherStaticLib1_CreateModuleObject` (move in sorted order)
   - Replacing `SomeOtherStaticLib1` with `QprStaticLib` in each duplicated line
   - Uncommenting each new line as required

## Adding new classes to this new dynamic library

NOTE: Strongly consider creating a new static library and expose via above, instead of adding
classes directly to this dynamic library module. This enables more agility in changing which
classes are exposed via which dynamic library modules.

Steps to create a new class (e.g. named `CSpxSomeOtherClass`)
1. Copy `carbon/source/core/abc_dynamic_lib/new_class_1.*` as `some_other_class.*`
2. Update  `some_other_class.*` by...
   - Replacing all occurrences of `CSpxNewClass1` with `CSpxSomeOtherClass`
3. Update `carbon/source/core/abc_dynamic_lib/create_module_object.cpp` by...
   - Duplicating each line containing `CSpxNewClass1`, and
   - Replacing `CSpxNewClass1` with `CSpxSomeOtherClass`, and
   - Uncommenting each new line as required