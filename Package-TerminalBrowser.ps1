param([string]$Destination = (Join-Path $PSScriptRoot '../TerminalBrowser-MVP'))
$ErrorActionPreference='Stop'
$hostOutput=Join-Path $PSScriptRoot 'bin/x64/Release/WindowsTerminal'
if (!(Test-Path "$hostOutput/TerminalBrowser.exe")) { throw 'Build TerminalBrowser first.' }
if (!(Test-Path "$hostOutput/Microsoft.Web.WebView2.Core.dll")) { throw 'WebView2 WinRT runtime is missing from the build output.' }
if (!(Test-Path "$hostOutput/resources.pri") -or (Get-Item "$hostOutput/resources.pri").Length -lt 1000000) { throw 'Build the complete unpackaged resource layout first.' }
New-Item $Destination -ItemType Directory -Force | Out-Null
Get-ChildItem $hostOutput -File | Where-Object Extension -In '.exe','.dll','.pri','.winmd','.json' | Copy-Item -Destination $Destination -Force
Get-ChildItem $hostOutput -Directory | Where-Object Name -In 'TerminalApp','Microsoft.Terminal.Control','Microsoft.Terminal.Settings.Editor','Microsoft.Terminal.UI.Markdown' | Copy-Item -Destination $Destination -Recurse -Force
Copy-Item "$hostOutput/_xaml/Microsoft.UI.Xaml" -Destination $Destination -Recurse -Force
New-Item (Join-Path $Destination '.portable') -ItemType File -Force | Out-Null
New-Item (Join-Path $Destination 'settings') -ItemType Directory -Force | Out-Null
if (!(Test-Path "$Destination/settings/settings.json")) {
    Copy-Item "$PSScriptRoot/browser-settings.json" "$Destination/settings/settings.json"
}
Copy-Item "$PSScriptRoot/LICENSE" "$Destination/LICENSE"
Copy-Item "$PSScriptRoot/NOTICE.md" "$Destination/NOTICE.md"
Copy-Item "$PSScriptRoot/README-BROWSER.md" "$Destination/README.md"
Write-Host "Ready to launch: $Destination\TerminalBrowser.exe"
