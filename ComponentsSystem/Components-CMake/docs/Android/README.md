# Android package sources

`COMPONENTS_CCR_CONFIGURE_ANDROID_APP` uses the Android package source
directory to customize the generated APK. By default it looks for:

```
<app-root>/android
    AndroidManifest.xml
    build.gradle
    res/
        values/
            libs.xml
            strings.xml
```

The same directory can be passed explicitly:

```cmake
COMPONENTS_CCR_CONFIGURE_ANDROID_APP(
    ${APP_TARGET_NAME}
    ${CMAKE_CURRENT_LIST_DIR}
    PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/android"
    LIBS Qt5::Widgets Qt5::Concurrent Qt5::Sql
)
```

If `<app-root>/android` does not exist, `androiddeployqt` uses the default
package template shipped with Qt. The files in this directory are only an
example of the expected layout and placeholders. They are copied into
`<build-dir>/android-build` by `androiddeployqt`; edit the sources here, not
the generated copies.

## Files

- `AndroidManifest.xml` - package name, activities, permissions and features.
  `androiddeployqt` replaces `%%INSERT_*%%` placeholders and fills default
  permissions/features based on the linked Qt modules.
- `build.gradle` - Gradle build script copied from the Qt 5.15 template.
- `res/values/libs.xml` - arrays filled by `androiddeployqt` with Qt and extra
  libraries. Do not remove the `%%INSERT_*%%` comments.
- `res/values/strings.xml` - strings referenced by the Qt Android template.

The application library name (`%%INSERT_APP_LIB_NAME%%`) is filled from the
`application-binary` value in `android_deployment_settings.json`. The
`COMPONENTS_CCR_CONFIGURE_ANDROID_APP` function sets it to the CMake target
name. The generated library is named `lib<target>_<abi>.so`.

## Build

```sh
cmake -S . -B build-android \
    -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK/build/cmake/android.toolchain.cmake" \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-21 \
    -DCMAKE_PREFIX_PATH="$QT_ANDROID"

cmake --build build-android --target apk
```

The `apk` target is provided by Qt 5.15 (`Qt5AndroidSupport.cmake`) and builds
a debug-signed, installable APK. Use the Gradle/Android Studio tooling or
`androiddeployqt --release --sign ...` for a release-signed package.

Make sure `androiddeployqt` is available in `PATH` (Qt Creator does this
automatically). Qt 5.15's CMake support locates it with `find_program` when Qt
is found.

## Notes

- The Android application target is a `SHARED` CMake library, not an
  executable. `QtActivity` loads it at runtime.
- Project libraries are built as `STATIC` on Android (see
  `COMPONENTS_CCR_CONFIGURE_ANDROID_LIB`). They are linked into the application
  library, so they do not require separate `ANDROID_EXTRA_LIBS` entries.
- `MIN_SDK_VERSION` and `TARGET_SDK_VERSION` use the Qt/NDK values
  (`ANDROID_MIN_SDK_VERSION`, `ANDROID_TARGET_SDK_VERSION`) when they are
  numeric, otherwise 21 and 30. Override them with the function arguments when
  the Qt/Android SDK requires other values.
