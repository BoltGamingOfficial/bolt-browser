# BOLT Browser standalone proof of concept

This directory builds a separate native desktop application with Qt Widgets and Qt WebEngine. The content view is an embedded Chromium-based engine. The application does not invoke Firefox, modify Firefox policies, or reuse a Firefox profile.

Windows is the first install target. GitHub Actions checks the standalone source on Windows 11 x64 and Ubuntu 24.04 using Qt WebEngine 6.10.3; it builds, runs the address resolver tests, stages each deployment, and checks for key WebEngine files. These CI checks have not yet completed for this branch, and they do not replace a clean-machine launch or real-site test. macOS and other operating systems are not currently claimed as supported. No finished installer or release package is available yet.

The current scope is deliberately small: one window, one page, address/search entry, back/forward/reload/home, loading feedback, basic failure feedback, keyboard shortcuts, a fresh BOLT-owned persistent profile, and a bundled offline test page. There is no tab system, download manager, extension support, sync, custom tracking protection, or settings framework.

## Requirements

- Qt 6.8 or newer with the **Qt WebEngine Widgets** component.
- CMake 3.21 or newer and a supported C++17 compiler.
- Windows: 64-bit Windows 10/11, Visual Studio 2022 x64 toolchain, and the matching Qt `msvc2022_64` build. Qt WebEngine does not support a MinGW build.
- Linux: 64-bit Ubuntu 22.04/24.04 or a compatible distribution with the matching Qt `gcc_64` build. Runtime libraries, Qt plugins, WebEngine process/resources, ICU data, and codecs must be available. Linux sandbox operation also depends on kernel/user-namespace configuration; this app does not turn it off.

The repository does not vendor Qt. A successful development build therefore requires installing Qt WebEngine first. See [Qt's installation guide](https://doc.qt.io/qt-6/get-and-install-qt.html), [supported configurations](https://doc.qt.io/qt-6/supported-platforms.html), and [WebEngine platform notes](https://doc.qt.io/qt-6/qtwebengine-platform-notes.html).

## Build and run on Windows

Open a Visual Studio 2022 x64 developer PowerShell with a matching Qt installation available. Adjust the Qt path to the installed version:

```powershell
cmake -S browser -B build\browser -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.12.0\msvc2022_64"
cmake --build build\browser --config Release
cmake --install build\browser --config Release --prefix build\browser\package
& build\browser\package\bin\BoltBrowser.exe
```

The install step invokes Qt's generated deployment script, which uses the Qt deployment tooling for the supported install layout. Qt documents `windeployqt` without a `--webengine` option. Before distributing the resulting directory, inspect it for the WebEngine subprocess, resource packs, locales, ICU data, codecs, plugins, and required Visual C++ runtime; then launch it on a clean Windows machine without Qt installed. That package audit and clean-machine launch have not been done.

## Build and run on Linux

Install Qt 6.8+ with the WebEngine Widgets module and the CMake/C++ build tools for the distribution. Adjust `CMAKE_PREFIX_PATH` to your Qt installation:

```sh
cmake -S browser -B build/browser -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_PREFIX_PATH="$HOME/Qt/6.12.0/gcc_64"
cmake --build build/browser --parallel
cmake --install build/browser --prefix build/browser/package
./build/browser/package/bin/BoltBrowser
```

The install step invokes Qt's generated deployment script for the standard Linux layout. A portable Linux bundle is not yet verified: inspect that it includes Qt libraries/plugins, `QtWebEngineProcess`, resource `.pak` files, locale data, ICU and codec assets, then run it on each target distribution with Qt absent. Confirm the Chromium sandbox works on the target kernel and user-namespace configuration. Do not add `--no-sandbox`, `QTWEBENGINE_DISABLE_SANDBOX`, or similar workarounds to a release build. See [Qt WebEngine deployment](https://doc.qt.io/qt-6/qtwebengine-deploying.html), [Qt application deployment](https://doc.qt.io/qt-6/deployment.html), and [sandbox platform notes](https://doc.qt.io/qt-6/qtwebengine-platform-notes.html#sandboxing-support).

## Automated address handling tests

With Qt installed, configure and run the resolver test target using `ctest --test-dir build/browser -C Release --output-on-failure` on Windows or `ctest --test-dir build/browser --output-on-failure` on Linux. Cases cover hostnames with ports, loopback HTTP defaults, IPv4, IPv6, explicit HTTPS, search phrases, and malformed or unsupported inputs.

## Manual verification

Run these checks on each operating system after building:

1. Launch `BoltBrowser` and confirm the local **BOLT Browser is running** page appears while offline.
2. Type `example.com` in the address bar and confirm HTTPS navigation and the page render. Type a phrase such as `bolt browser test` and confirm it opens DuckDuckGo results. Automated tests cover `localhost:3000`, `example.com:8080`, `[::1]:3000`, bare `::1`, unsupported schemes, and invalid ports; manually check local HTTP services if available.
3. Navigate from the page to `https://example.com/`, then use Back, Forward, Reload, and Home. Confirm the address field and window title follow navigation.
4. Test `Ctrl+L`, `Ctrl+R`, `F5`, `Alt+Left`, `Alt+Right`, `Alt+Home`, `Esc`, and `Ctrl+W`.
5. Enter an invalid host and a known invalid-certificate test endpoint such as `https://expired.badssl.com/`. Confirm the status bar reports rejection, the page title/address do not show a successfully loaded destination page, and no page content from that site is rendered as trusted. The rejection handler has only been source-reviewed so far; this runtime check remains outstanding.
6. Close and relaunch. Confirm the application closes normally, its profile is under the OS-specific BOLT application data directory, and Firefox was not started.
7. Record OS/version, Qt and WebEngine versions, build logs, launch result, test result, and any sandbox/runtime warning. For an initial performance baseline, record machine CPU/RAM, cold launch time to the local page, idle process memory after 60 seconds, and memory while `example.com` is loaded. No performance data is claimed until measured.

## Security boundaries and limitations

- No certificate-error bypass is implemented. The page handler explicitly calls `rejectCertificate()`; normal certificate validation remains enabled. This is not a substitute for the invalid-certificate runtime test above.
- No WebChannel, JavaScript bridge, custom URL scheme, native filesystem bridge, or application privilege is exposed to website content.
- WebEngine's Chromium sandbox is controlled by the engine/platform build and runtime. This project has not independently audited it; Linux user-namespace/seccomp support must be confirmed on the target machine.
- Engine updates are not automatic. BOLT must monitor the selected Qt WebEngine release/support window, update Qt promptly for Chromium fixes, rebuild, test and redistribute the application bundle. No updater is included.
- Qt WebEngine uses Chromium and third-party components. Review the applicable Qt/Chromium license texts and ship required notices/source offers before distribution. The repository's own license is currently unspecified.
- The profile and cookies are persistent by design. There is no private browsing mode or UI for clearing cookies/site data or deleting the profile, so this prototype is not privacy-hardened.
- GPU acceleration, codecs/video, screen capture, downloads, permissions, file upload, proxy/cert store behavior and broad website compatibility have not been validated in this prototype.

## Verification status for this checkout

- Implemented: native-window source, Qt WebEngine view, app-owned persistent profile, basic navigation/search, local page, progress/error feedback, keyboard shortcuts, and explicit certificate rejection handler.
- Static checks: run `bash tests/check-browser.sh` from the repository root.
- Native build/runtime: not yet run. This assessment environment has no Qt installation or VS 2022 toolchain; it has VS 18/2026, which does not match the documented `msvc2022_64` kit. C: had 281.4 GB free, but the exact Qt package footprint could not be confirmed from an installer catalog, so no compiler/Qt installation was attempted. Build, launch, TLS endpoint, GUI, package contents, clean-machine deployment, and performance checks remain unverified here.
- Cross-platform support: source/build instructions target Windows and Linux, but neither target has been runtime-verified in this environment.
