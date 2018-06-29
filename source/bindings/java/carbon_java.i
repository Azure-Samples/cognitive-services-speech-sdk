%module(directors="1") carbon_java

%rename("%(firstlowercase)s", %$isfunction) "";
%rename("%(firstlowercase)s", %$ismember) "";

%typemap(directorin, descriptor="[B") (char *STRING, unsigned int LENGTH) {
   jbyteArray jb = (jenv)->NewByteArray($2);
   (jenv)->SetByteArrayRegion(jb, 0, $2, (jbyte *)$1);
   $input = jb;
}

%typemap(directorargout) (char *STRING, unsigned int LENGTH)
%{
    (jenv)->GetByteArrayRegion($input, 0, $2, (jbyte *)$1);
%}

%{
#ifdef _MSC_VER
#pragma warning(disable : 4267) //warning C4267: 'initializing': conversion from 'size_t' to 'jsize', possible loss of data
// disable: error 6011:  : Dereferencing NULL pointer 'arg1'.
#pragma warning( disable : 6011 )
#endif
%}

%{
#include <speechapi_cxx_common.h>
#include "speechapi_cxx_audioinputstream.h"
%}

%feature("director") AudioInputStream;
%apply (char *STRING, int LENGTH) { (char* dataBuffer, size_t size) };

%include "std_wstring.i"

%typemap(javainterfaces) StdMapWStringWStringMapIterator "java.util.Iterator<String>"
%typemap(javacode) StdMapWStringWStringMapIterator %{
  public void remove() throws UnsupportedOperationException {
    throw new UnsupportedOperationException();
  }

  public String next() throws java.util.NoSuchElementException {
    if (!hasNext()) {
      throw new java.util.NoSuchElementException();
    }

    return nextImpl();
  }
%}

%javamethodmodifiers StdMapWStringWStringMapIterator::nextImpl "private";
%inline %{
#include <map>
#include <stdlib.h>

  void SetupNativeLibraries(const std::wstring& config)
  {
#ifndef _MSC_VER
    std::string s(config.begin(), config.end());
    int overwrite = 1;
    if(s.length() < 1) { overwrite = 0; s = "/system/etc/ssl/certs/"; }

    setenv("SSL_CERT_DIR", s.c_str(), overwrite);
#endif
  }


  struct StdMapWStringWStringMapIterator {
    typedef std::map<std::wstring,std::wstring> WStringWStringMap;
    StdMapWStringWStringMapIterator(const WStringWStringMap& m) : it(m.begin()), map(m) {
    }

    bool hasNext() const {
      return it != map.end();
    }

    const std::wstring& nextImpl() {
      const std::pair<std::wstring,std::wstring>& ret = *it++;
      currentKey = ret.first; //get the key
      return currentKey;
    }
  private:
    std::wstring currentKey;
    WStringWStringMap::const_iterator it;
    const WStringWStringMap& map;
  };
%}

%typemap(javainterfaces) std::map<std::wstring,std::wstring> "Iterable<String>"

%newobject std::map<std::wstring,std::wstring>::iterator() const;
%extend std::map<std::wstring,std::wstring> {
  StdMapWStringWStringMapIterator *iterator() const {
    return new StdMapWStringWStringMapIterator(*$self);
  }
}

%include "carbon.i"
