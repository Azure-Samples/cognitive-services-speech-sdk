# Microsoft Cognitive Services Speech SDK for JavaScript

Internal package for docs.

Visit https://aka.ms/csspeech.

## Build the project

* Our CI build restore build packages with the following command, to exactly match the package configuration from `package-lock.json`:

  ```
  npm ci
  ```

* In dev box scenario, you can run the following, which will also implictly run our build:

  ```
  npm install
  ```

* To run the build when dependencies are already restored:

  ```
  npm run prepare
  ```

  or

  ```
  npx gulp bundle
  ```

  > Note: `npx` is packaged with NPM 5.2 or above. Update NPM / Node if you
  > don't have it or install it globally with `npm install -g npx` (less
  > preferable).
