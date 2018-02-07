%module carbon_py

// rename all functions/methods/properties to match Python's naming convention 
// https://www.python.org/dev/peps/pep-0008/#method-names-and-instance-variables
%rename("%(undercase)s", %$isfunction) "";
%rename("%(undercase)s", %$ismember) "";

%include "carbon.i"