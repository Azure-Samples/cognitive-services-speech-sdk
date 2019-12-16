%module(directors="1") carbon_java

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

#if defined(ANDROID) || defined(__ANDROID__)
#define __SPX_DO_TRACE_IMPL __swig_spx_do_trace_message
void __swig_spx_do_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...) throw();
#endif
%}

%{
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_string_helpers.h>
#include "speechapi_cxx_audio_stream.h"
%}

%feature("director") PullAudioInputStreamCallback;
%feature("director") PushAudioOutputStreamCallback;
%apply (char *STRING, int LENGTH) { (uint8_t* dataBuffer, uint32_t size) };
%apply (int ID, char *STRING, int LENGTH) { (uint32_t id, uint8_t* value, uint32_t size) };

%feature("director") PullAudioOutputStreamCallback;
%apply (char *STRING, int LENGTH) { (uint8_t* buffer, uint32_t bufferSize) };
%apply (int POS, char *STRING, int LENGTH) { (uint32_t pos, uint8_t* buffer, uint32_t bufferSize) };

%include "std_string.i"

%typemap(javainterfaces) StdMapStringStringMapIterator "java.util.Iterator<String>"
%typemap(javacode) StdMapStringStringMapIterator %{
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

%javamethodmodifiers StdMapStringStringMapIterator::nextImpl "private";
%inline %{
#include <map>
#include <algorithm>
#include <stdlib.h>

void SetTempDirectory(std::string tmpDir)
{
#ifndef _MSC_VER
    if(tmpDir.length() > 0)
    {
      setenv("TMPDIR", tmpDir.c_str(), 0);
    }
#endif
}

struct StdMapStringStringMapIterator {
    typedef std::map<std::string,std::string> StringStringMap;
    StdMapStringStringMapIterator(const StringStringMap& m) : it(m.begin()), map(m) {
    }

    bool hasNext() const {
      return it != map.end();
    }

    const std::string& nextImpl() {
      const std::pair<std::string,std::string>& ret = *it++;
      currentKey = ret.first; //get the key
      return currentKey;
    }
  private:
    std::string currentKey;
    StringStringMap::const_iterator it;
    const StringStringMap& map;
  };
%}

%typemap(javainterfaces) std::map<std::string,std::string> "Iterable<String>"

%newobject std::map<std::string,std::string>::iterator() const;
%extend std::map<std::string,std::string> {
  StdMapStringStringMapIterator *iterator() const {
    return new StdMapStringStringMapIterator(*$self);
  }
}

%include "carbon.i"
%template(UInt8Vector) std::vector<uint8_t>;

%{
// Note: in case of android, log to logcat
#if defined(ANDROID) || defined(__ANDROID__)
#include <android/log.h>
#endif

void __swig_spx_do_trace_message(int level, const char* pszTitle, const char* fileName, const int lineNumber, const char* pszFormat, ...) throw()
{
    UNUSED(level);
    try
    {
        va_list argptr;
        va_start(argptr, pszFormat);

        std::string format;
        while (*pszFormat == '\n' || *pszFormat == '\r')
        {
            if (*pszFormat == '\r')
            {
                pszTitle = nullptr;
            }

            format += *pszFormat++;
        }

        if (pszTitle != nullptr)
        {
            format += pszTitle;
        }

        std::string fileNameOnly(fileName);
        std::replace(fileNameOnly.begin(), fileNameOnly.end(), '\\', '/');

        std::string fileNameLineNumber = " " + fileNameOnly.substr(fileNameOnly.find_last_of('/', std::string::npos) + 1) + ":" + std::to_string(lineNumber) + " ";

        format += fileNameLineNumber;
        format += pszFormat;

        if (format.length() < 1 || format[format.length() - 1] != '\n')
        {
            format += "\n";
        }

// In current NDK, static libc does not provide
// symbols for stderr. In case, the dynamic libc does
// not provide them as well, we may receive a unresolved
// symbol dlopen error.
// Thus, we drop all logging into logcat if we are in debug mode or
// drain it if in release mode)
#if defined(ANDROID) || defined(__ANDROID__)
        int androidPrio = ANDROID_LOG_ERROR;
        switch (level)
        {
            case __SPX_TRACE_LEVEL_INFO:    androidPrio = ANDROID_LOG_INFO;     break; // Trace_Info
            case __SPX_TRACE_LEVEL_WARNING: androidPrio = ANDROID_LOG_WARN;     break; // Trace_Warning
            case __SPX_TRACE_LEVEL_ERROR:   androidPrio = ANDROID_LOG_ERROR;    break; // Trace_Error
            case __SPX_TRACE_LEVEL_VERBOSE: androidPrio = ANDROID_LOG_VERBOSE;  break; // Trace_Verbose
            default: androidPrio = ANDROID_LOG_FATAL; break;
        }

        __android_log_vprint(androidPrio, "SpeechSDKJavaBinding", format.c_str(), argptr);

#else
        vfprintf(stderr, format.c_str(), argptr);
#endif

        va_end(argptr);
    }
    catch(...)
    {
    }
}
%}
