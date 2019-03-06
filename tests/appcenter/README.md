# Readme.md

## Setting up App Center

* Install [Node.js](https://nodejs.org/) if you don't have it yet.

* Set up App Center cli tool:

  ``` shell
  npm -g i appcenter-cli
  appcenter login
  ```

* Set up Tester Application
  * Start https://appcenter.ms.
  * Click on "Add a new app" and follow steps. Use these settings:
    * Name: SpeechSDKTester
    * OS: Android
    * Platform: Java/Kotlin

* Set up the test queue
  * Click on "SpeechSDKTester" (the app you created).
  * Click on "Test" in the left menu. This will open the test submenu.
  * Click on "Test runs" in the left menu.
    * Click on "Test series", create a new series with name "master".
    * Click on "Device sets", create a new device set with name "master".

## Running the Speech SDK Tests

* Configure subscription keys and other test settings
  * Create or edit %SPEECHSDK_BUILD_ROOT%\MainActivity.properties and update it with the subscription key(s) and region data. Enter at least:

    ```shell
    # test properties
    SpeechSubscriptionKey = YourSubscriptionKey
    SpeechRegion = YourRegion
    LuisSubscriptionKey = enter your subscription info here
    LuisRegion = YourLuisRegion
    LuisAppId = enter your LUIS AppId
    Keyword = Computer
    KeywordModel = kws-computer.zip
    ```

  * Edit .\run_tests.bat and replace APPCENTER_USER_ID with your App Center user-id.

* Prepare a command window by starting a cmd shell in the repository root

  ``` shell
  cmd
  cd java-android
  ```

* Build the Speech SDK

  ``` shell
  md build && cd build
  cmake -G "NMake Makefiles" -DSPEECHSDK_TARGET_PLATFORM=Android-arm32 ^
        -DCMAKE_ANDROID_NDK=/android-ndk-r16b -DCMAKE_BUILD_TYPE=Debug ..
  cmake --build .
  ```

* Submit the test application

  Note: you need to pass the full path to the build directory of the carbon build as your first parameter.

  ``` shell
  .\run_tests.bat %SPEECHSDK_BUILD_ROOT%
  ```

* Evaluate the results
  * Navigate to "tests" in appcenter.ms to see your test run.

  * In case you want to get the status of a particular run from the command line, use

    ```shell
    appcenter test status --test-run-id APPCENTER_TESTRUN_GUID --app "%APPCENTER_USER_ID%/%APPCENTER_APPID%"
    ```
