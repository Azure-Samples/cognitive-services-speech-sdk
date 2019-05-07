# Speech SDK Java Unit Tests

This folder contains our Speech SDK Java tests. In Azure DevOps, these are run
in Java desktop environments (Windows, Linux, macOs), as well as on Android
devices.

There are two ways to build this project:

* As a stand-alone Maven project. Use an appropriate IDE, for example [Visual
  Studio Code](https://code.visualstudio.com) with [Java
  support](https://devblogs.microsoft.com/visualstudio/java-on-visual-studio-code-april-update/),
  and open the project in this folder.

  Normally, for this type of project, application code would be located under
  `src/main/java`. For this project, however, there isn't any application code.
  Instead, this project takes a reference on our Speech SDK (see the
  `<repositories/>` and `<dependencies/>` tag in [pom.xml](pom.xml)) to test
  against.

  All test code is located in the usual place, `src/test/java`.

* Through our normal CMake build via
  [src/test/java/CMakeLists.txt](src/test/java/CMakeLists.txt). This builds all
  test code into a JAR, `com.microsoft.cognitiveservices.speech.tests.jar`.
  This JAR, together with the JUnit and Hamcrest dependencies located in the
  `../../../external/junit/` is used to run the Speech SDK tests for Java in
  Azure DevOps.

These two build configurations ([pom.xml](pom.xml) and
[src/test/java/CMakeLists.txt](src/test/java/CMakeLists.txt)) must be kept in
sync. For example, the JUnit version used in the Maven project should match the
JUnit version used via CMake.
