# Running Speech SDK unit tests for Java in App Center

## Setting up App Center

* Install [Node.js](https://nodejs.org/) if you don't have it yet.

* Set up the App Center CLI:

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

* We assume the environment variable `%SPEECHSDK_BUILD_ROOT%` points to your build directory for the Speech SDK for Android.
* Configure subscription keys and other test settings
  * Create or edit `%SPEECHSDK_BUILD_ROOT%\MainActivity.properties` and update it with the subscription key(s) and region data. Enter at least:

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

  * Optionally, if you want the test suite to upload the test results (JUnit XML) to an Azure Blob Storage container, also add these two settings:

    ```shell
    SasToken = YourSasToken
    SasContainerPrefix = https://YourStorageAccountName.blob.core.windows.net/YourContainer/Path/To/Results/
    ```

    Make sure that the container exists and the SAS token is valid at the time the test is run in App Center
    (the [Azure CLI](https://docs.microsoft.com/cli/azure/install-azure-cli) can be used to perform all of these tasks).

  * Set up environment to configure your App Center user ID (your account) and app ID (should be `SpeechSDKTester` if you followed above instructions):

    ```shell
    set APPCENTER_USER_ID=YourUser
    set APPCENTER_APPID=SpeechSDKTester
    ```

* Build the Speech SDK for Android from a command window in the repository root:

  ``` shell
  set SPEECHSDK_REPO_ROOT=%CD%
  md "%SPEECHSDK_BUILD_ROOT%" && pushd "%SPEECHSDK_BUILD_ROOT"
  cmake -G "NMake Makefiles" -DSPEECHSDK_TARGET_PLATFORM=Android-arm32 ^
        -DCMAKE_ANDROID_NDK=/android-ndk-r16b -DCMAKE_BUILD_TYPE=Debug "%SPEECHSDK_REPO_ROOT%"
  cmake --build .
  popd
  ```

* From your repository root, go to the `java-android` directory:

  ``` shell
  cd tests\appcenter\java-android
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
