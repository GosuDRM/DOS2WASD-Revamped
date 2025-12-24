Remove-Item $PSScriptRoot/build -Recurse -Force -ErrorAction:SilentlyContinue -Confirm:$False | Out-Null
& cmake -B $PSScriptRoot/build -S $PSScriptRoot --preset=REL -DPLUGIN_MODE:BOOL=TRUE
& cmake --build $PSScriptRoot/build --config Release

# Copy required DLLs to temp folder and create zip
if ($LASTEXITCODE -eq 0) {
    Write-Host "Creating release package..."
    
    # Create folder structure matching game layout
    $tempDir = "$PSScriptRoot\build\Release\package"
    $binDir = "$tempDir\bin"
    $nativeModsDir = "$tempDir\bin\NativeMods"
    
    Remove-Item $tempDir -Recurse -Force -ErrorAction:SilentlyContinue
    New-Item -ItemType Directory -Path $nativeModsDir -Force | Out-Null
    
    # Copy DLLs to bin/
    Copy-Item $PSScriptRoot\extern\SDL2\SDL2.dll $binDir\
    Copy-Item $PSScriptRoot\extern\SDL2\SDL2_original.dll $binDir\
    Copy-Item $PSScriptRoot\extern\NativeModLoader\bink2w64.dll $binDir\
    Copy-Item $PSScriptRoot\extern\NativeModLoader\bink2w64_original.dll $binDir\
    
    # Copy mod files to bin/NativeMods/
    Copy-Item $PSScriptRoot\build\Release\DOS2WASD.dll $nativeModsDir\
    Copy-Item "$PSScriptRoot\package\DOS2WASD.toml" $nativeModsDir\ -Force
    
    # Create zip file
    $zipPath = "$PSScriptRoot\build\Release\DOS2WASD_Revamped.zip"
    Remove-Item $zipPath -Force -ErrorAction:SilentlyContinue
    Compress-Archive -Path "$tempDir\*" -DestinationPath $zipPath -Force
    
    # Clean up temp folder
    Remove-Item $tempDir -Recurse -Force
    
    Write-Host "Build complete!"
    Write-Host "Zip file: $zipPath"
    Write-Host ""
    Write-Host "Zip structure:"
    Write-Host "  bin/"
    Write-Host "    SDL2.dll"
    Write-Host "    SDL2_original.dll"
    Write-Host "    bink2w64.dll"
    Write-Host "    bink2w64_original.dll"
    Write-Host "    NativeMods/"
    Write-Host "      DOS2WASD.dll"
    Write-Host "      DOS2WASD.toml"
}