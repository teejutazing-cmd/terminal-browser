param([string]$LocalToolsRoot = (Join-Path $PSScriptRoot '../../work'))
$ErrorActionPreference = 'Stop'

# The supplied workspace includes a local build-tool overlay for machines with
# C++ tools but without registered Visual Studio UWP build components.
$localBuild = Join-Path $LocalToolsRoot 'build.ps1'
if (Test-Path $localBuild) {
    & pwsh -NoProfile -File $localBuild -ExtraArgs '/t:Build;_WTPrepareUnpackagedLayoutForRun'
    if ($LASTEXITCODE) { throw "Build failed: $LASTEXITCODE" }
} else {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vs = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (!$vs) { throw 'Install VS 2022 C++ and Universal Windows Platform build tools, including Windows SDK 22621.' }
    $devEnvironment = cmd /c "`"$vs\VC\Auxiliary\Build\vcvars64.bat`" >nul && set"
    foreach ($line in $devEnvironment) {
        if ($line -match '^([^=]+)=(.*)$') { [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process') }
    }
    Push-Location $PSScriptRoot
    try {
        & ./dep/nuget/nuget.exe install ./dep/nuget/packages.config -OutputDirectory packages -NonInteractive
        if ($LASTEXITCODE) { throw 'NuGet restore failed.' }
        & "$vs/MSBuild/Current/Bin/MSBuild.exe" ./src/cascadia/WindowsTerminal/WindowsTerminal.vcxproj '/t:Build;_WTPrepareUnpackagedLayoutForRun' /m:1 /nr:false /p:CL_MPCount=2 /p:Configuration=Release /p:Platform=x64 /p:XamlLanguage=CppWinRT "/p:SolutionDir=$PSScriptRoot\"
        if ($LASTEXITCODE) { throw 'Build failed.' }
    } finally { Pop-Location }
}
Write-Host "Built: $PSScriptRoot\bin\x64\Release\WindowsTerminal\TerminalBrowser.exe"
