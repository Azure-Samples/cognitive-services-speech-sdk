# xyz_new_static_lib

This is an example carbon static library module. Use directory contents to quickly
create a new static library module for carbon using copy/paste/search/replace.

## Creating a new static library

Steps to create a new static library (e.g. named `abc_static_lib`)
1. Make `abc_static_lib` directory under `carbon/source/core`
2. Copy `xyz_new_static_lib` contents to `carbon/source/core/abc_static_lib`
3. Update `carbon/source/core/abc_static_lib/CMakeLists.txt` by...
   - Replacing all occurrences of `xyz_new_static_lib` with `abc_static_lib`
4. Update `carbon/source/core/CMakeLists.txt` for `xyz_new_static_lib` by...
   - Duplicating each line containing `xyz_new_static_lib` (move in sorted order)
   - Replacing `xyz_new_static_lib` with `abc_static_lib` in each duplicated line
5. Update `carbon/source/*.cpp;*.h` by...
   - Duplicating each line containing `XyzNewStaticLib_CreateModuleObject`
   - Replacing `XyzNewStaticLib_CreateModuleObject` with `AbcStaticLib_CreateModuleOjbect`
6. Rebuild

## Adding new classes to library

Steps to create a new class (e.g. named `CSpxSomeOtherClass`)
1. Copy `carbon/source/core/abc_static_lib/new_class_1.*` as `some_other_class.*`
2. Update  `some_other_class.*` by...
   - Replacing all occurrences of `CSpxNewClass1` with `CSpxSomeOtherClass`
3. Update `carbon/source/core/abc_static_lib/create_module_object.cpp` by...
   - Duplicating each line containing `CSpxNewClass1`, and
   - Replacing `CSpxNewClass1` with `CSpxSomeOtherClass`, and
   - Uncommenting each new line as required