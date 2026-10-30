# Standalone BOLT Browser

This directory builds BOLT as a separate native desktop application using Qt Widgets and Qt WebEngine (Chromium). The app does not invoke Firefox or modify a Firefox profile. Its current foundation includes one browser window, address/search resolution, back/forward/reload/home, loading feedback, explicit certificate rejection, an app-owned persistent profile, and a bundled offline home screen. The home screen combines BOLT's gold-and-black identity, a prominent DuckDuckGo search field, slow-glowing black lightning, and direct links to public research sources. Animation honors the operating system's reduced-motion preference.

## Build and CI targets

The intended targets are Windows x64, Linux x86_64, and macOS x86_64. CI build/test targets are Windows Server 2022 x64 with Visual Studio 2022, Ubuntu 24.04 x86_64, Arch Linux x86_64, and macOS x86_64. Apple Silicon remains unverified. CI builds and runs CTest and inspects staged WebEngine files where the platform deployment tools support it. A Windows Server build does not replace a Windows 10/11 desktop test. A passing CI build does not demonstrate a graphical launch, real-site navigation, certificate rejection at runtime, or a clean-machine install. OS versions and distributions outside those runners remain unverified.

Requirements: CMake 3.21+, C++17, and Qt 6.8+ with Qt WebEngine Widgets. CI currently uses Qt 6.10.3 because the selected Qt installer action could not retrieve Qt 6.11 packages from the revised Qt repository layout. This is a validated CI version, not a claim that it is the newest Qt release.

### Windows

Use Visual Studio 2022 with the Desktop development with C++ workload, Windows SDK, and matching Qt `msvc2022_64` WebEngine kit:

```powershell
cmake -S browser -B build\browser -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.10.3\msvc2022_64"
cmake --build build\browser --config Release
ctest --test-dir build\browser -C Release --output-on-failure
cmake --install build\browser --config Release --prefix build\browser\package
```

The CI staging check looks for the app executable, WebEngine subprocess, resource pack, and Windows platform plugin. This is not an installer and is not yet clean-machine validated; inspect runtime libraries, locales, ICU, codecs, plugins, and the Visual C++ runtime before distribution.

### Ubuntu

Install Qt 6 WebEngine Widgets, CMake, and a C++ compiler from the distribution or Qt installer, then configure with the matching Qt prefix:

```sh
cmake -S browser -B build/browser -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$HOME/Qt/6.10.3/gcc_64"
cmake --build build/browser --parallel
ctest --test-dir build/browser --output-on-failure
cmake --install build/browser --prefix build/browser/package
```

### Arch Linux x86_64

CI uses Arch Linux's `qt6-base` and `qt6-webengine` packages and builds/tests against the distribution Qt. A successful Arch CI run verifies compilation and address resolver tests on that runner; it is not an AUR package or a tested portable bundle. The generated install tree uses system Qt (`-DBOLT_DEPLOY_RUNTIME=OFF`); install runtime dependencies through the system package manager.

### macOS x86_64

CI uses a Qt `clang_64` WebEngine kit, builds the app bundle, and runs CTest on an Intel macOS runner. This is a build target only until GUI launch and clean-machine deployment are exercised. Apple Silicon has not been tested. The generated app bundle is staged by CMake; do not distribute it until Qt deployment contents, signing/notarization needs, and runtime launch are validated.

## Tests and manual validation

CTest currently covers address input resolution, including hostnames with ports, loopback, IPv4/IPv6, explicit HTTPS, searches, unsupported schemes, and malformed ports. Static checks confirm the application structure and reject unsafe certificate/sandbox overrides.

Before calling a release ready, run the following on each claimed OS: launch offline and render the local page; visit an HTTPS site; submit a search; exercise URL edge cases; verify an invalid TLS certificate is rejected and its content is not rendered as trusted; close/reopen and inspect profile behavior; and test the staged installation on a clean machine without a development Qt install. Record OS, Qt/WebEngine version, results, and deployment warnings. Measure idle and loaded memory before making resource-efficiency claims.

## Security, licensing, and scope

Certificate errors are rejected in source, but runtime behavior has not been manually verified yet. Profile data and cookies persist; there is no private window or control to clear site data. Linux Chromium sandbox operation depends on system configuration and must be checked without disabling the sandbox. Engine updates are not automatic; the project must track Qt WebEngine/Chromium security updates and rebuild promptly.

The repository's project license is unresolved. Qt WebEngine includes Chromium and third-party components. Choose a project license and verify all Qt/Chromium redistribution notices, source obligations, and platform-specific requirements before publishing binaries.
