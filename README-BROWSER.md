# Terminal Browser

A native Windows browser built directly from Microsoft's Windows Terminal
`v1.23.13503.0` (`e1cbaa5d83dcf6976c53e749b08235c9e358e8ab`).

## Run

Open `TerminalBrowser-MVP/TerminalBrowser.exe` in the adjacent portable distribution,
or use the adjacent `Launch-TerminalBrowser.cmd` launcher. No package registration
or replacement of the installed Windows Terminal is required.

Click **+** to open a browser tab. Enter a URL in the address field and press
**Enter**. Bare domains use HTTPS; localhost addresses use HTTP. The original
dropdown offers Browser, Microsoft Learn and GitHub starting pages. Website
titles appear in the original Terminal tab headers.

Click the address field for reliable URL entry. Some Terminal keyboard shortcuts
are consumed while the web page has focus. Native shell focus returns to the
address field so the original flyouts continue to work correctly.

The distribution uses the installed Microsoft Edge WebView2 Evergreen Runtime.
Keep all its files together in a writable directory. Settings are stored in
`settings/settings.json`; browsing data is stored in `BrowserData` beside the EXE.

## What is reused

The upstream XAML files are unchanged. The application uses Terminal's actual
`TerminalPage`, `TerminalTab`, `Pane`, tab/titlebar controls, menus, settings editor,
themes, resources, animations and native `IslandWindow`/`NonClientIslandWindow` host.

`BrowserPaneContent` implements the existing `IPaneContent` extension point. A
WinUI WebView2 occupies the content area, with a standard XAML address input above
it. Normal tab creation does not create a terminal connection. Each browser tab
owns one WebView2; switching tabs retains that page instance. User-initiated popup
links create tabs. Split requests create another browser tab.

The executable, unpackaged window identity, settings and browser data are separate
from installed Windows Terminal. Shared upstream terminal libraries remain build
dependencies because the unchanged shell and settings editor reference their types.

## Build

Run `pwsh -NoProfile -File ./Build-TerminalBrowser.ps1`, followed by
`pwsh -NoProfile -File ./Package-TerminalBrowser.ps1`.

This workspace includes an adjacent `work` directory with a local UWP build-tool
overlay and SDK 22621 XAML compiler. The build script detects it automatically.
Retain that directory to rebuild on this machine. A separate development machine
needs VS 2022 C++/UWP build tools, Windows SDK 22621, PowerShell 7 and the upstream
native build prerequisites. NuGet dependencies are restored by the script; the
ordinary registered Visual Studio setup uses the upstream vcpkg integration.

## MVP boundaries

The original settings and command menus still include terminal-specific options;
many do not apply to browser content. This milestone does not provide a downloads
manager, extensions, bookmarks, search-engine input, or full browser session
restoration. Navigation accepts HTTP, HTTPS and `about:blank`. Browser tabs share
one isolated browser profile. Terminal profile `commandline` values are starting
URLs in this fork. Move-pane commands are disabled for browser content; use the
original tab strip to switch or reorder tabs. The supplied build is Windows x64.

The original MIT license and third-party notices are retained.
