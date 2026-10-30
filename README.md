# BOLT Browser

BOLT Browser is a standalone desktop browser built with Qt WebEngine (Chromium). The current application is a small, single-window foundation with an address/search bar, basic navigation, persistent BOLT-owned profile, certificate rejection, and an offline BOLT home screen with a large search field, research shortcuts, and gently animated lightning. Its visual direction uses a dark canvas with gold light and black bolt forms.

This is an early browser foundation, not a finished research suite. It does not yet provide tabs, downloads, extension support, private mode, site-data controls, sync, or specialized research-source integrations. “Deep web” research means helping users reach lawful, non-indexed resources such as library databases, archives, and authenticated sites; it does not bypass access controls or provide anonymity. Tor and onion routing are not implemented.

## Build targets

The intended desktop targets are Windows x64, Linux x86_64 (including Ubuntu and Arch Linux), and macOS x86_64. GitHub Actions builds and runs the address resolver tests on a Windows Server 2022 x64 runner with Visual Studio 2022, Ubuntu 24.04 x86_64, Arch Linux x86_64, and macOS x86_64. Apple Silicon remains unverified. The Windows server build does not replace a Windows 10/11 desktop launch. These are compile/test targets; they do not prove GUI launch, website compatibility, or portable deployment on every distribution or OS release. Consult `browser/README.md` for exact verification limits.

Qt WebEngine embeds Chromium. BOLT will need regular Qt WebEngine security updates and rebuilt releases. No custom browser engine is planned; maintaining a Chromium or Firefox fork would make security maintenance and cross-platform delivery far less practical for this project.

## Development

See [`browser/README.md`](browser/README.md) for build requirements, commands, current test coverage, and unresolved deployment work. The repository has no selected project license yet. Do not redistribute binaries until the project license and Qt/Chromium notices and obligations are resolved.

## Releases

No finished installer is published. A release should only be made after the app has been launched and exercised on supported systems, deployment contents are verified on clean machines, and licensing is resolved.
