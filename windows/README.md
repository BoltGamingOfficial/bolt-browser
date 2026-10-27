# Windows 11 testing

BOLT Browser is currently a Firefox customization prototype, not a standalone browser engine. This Windows path lets you test the start page using Firefox for Windows without changing your existing browser settings.

## Run the prototype
1. Install Firefox for Windows from [Mozilla](https://www.mozilla.org/firefox/) if it is not already installed.
2. Download or clone this repository, then extract it to a folder you can write to.
3. Open PowerShell in the repository folder.
4. Run:
   ```powershell
   Set-ExecutionPolicy -Scope Process Bypass
   .\windows\launch-bolt-browser.ps1
   ```

The execution-policy command applies only to the current PowerShell process. If your system policy already allows local scripts, it may not be needed.

The script locates Firefox and opens `startpage/index.html` in a new tab. It does **not** edit Firefox policies, create a separate profile, change the default browser, or alter privacy settings. The start page's search form sends queries to DuckDuckGo.

## What this does and does not test
- It tests the local start page, its links, and the basic Windows launch path.
- It uses your normal Firefox profile, so existing extensions and settings remain in effect.
- It does not yet install BOLT-specific Firefox policies or create a separately branded Firefox application.
- Firefox private windows and tracking protection are Firefox features; test them in Firefox's own menus.
- Do not compare performance yet unless you record the same startup and tab conditions for ordinary Firefox and the BOLT start page.

After the Windows smoke test, Linux packaging and policy integration must still be built and tested separately on Arch Linux.
