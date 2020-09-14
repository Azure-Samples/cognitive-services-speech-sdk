# xyz_new_lib

This is an example carbon static library module. Use directory contents to quickly
create a new static library module for carbon using copy/paste/search/replace.

## Creating a new library

Steps to create a new library (e.g. named `abc_new_lib`)
1. Make `abc_new_lib` directory under `carbon/source/core`
2. Copy `xyz_new_lib` contents to `carbon/source/core/abc_new_lib`
3. Update `carbon/source/core/abc_new_lib/CMakeLists.txt` by...
   - Replacing all occurrences of `xyz_new_lib` with `abc_new_lib`
4. Update `carbon/source/core/CMakeLists.txt` for `xyz_new_lib` by...
   - Duplicating each line containing `xyz_new_lib` (move in sorted order)
   - Replacing `xyz_new_lib` with `abc_new_lib` in each duplicated line
5. Update `carbon/source/*.cpp;*.h` by...
   - Duplicating each line containing `XyzNewLib_CreateModuleObject`
   - Replacing `XyzNewLib_CreateModuleObject` with `AbcNewLib_CreateModuleOjbect`
6. Rebuild

## Adding new classes to library

Steps to create a new class (e.g. named `CSpxSomeOtherClass`)
1. Copy `carbon/source/core/abc_new_lib/new_class_1.*` as `some_other_class.*`
2. Update  `some_other_class.*` by...
   - Replacing all occurrences of `CSpxNewClass1` with `CSpxSomeOtherClass`
3. Update `carbon/source/core/abc_new_lib/create_module_object.cpp` by...
   - Duplicating each line containing `CSpxNewClass1`, and
   - Replacing `CSpxNewClass1` with `CSpxSomeOtherClass`, and
   - Uncommenting each new line as required