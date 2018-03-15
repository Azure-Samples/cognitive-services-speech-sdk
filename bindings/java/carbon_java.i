%module(directors="1") carbon_java

%rename("%(firstlowercase)s", %$isfunction) "";
%rename("%(firstlowercase)s", %$ismember) "";


%include "carbon.i"
