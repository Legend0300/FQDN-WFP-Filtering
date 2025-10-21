# Download nlohmann/json library
# Run this script from the project root directory

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " Downloading nlohmann/json library..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

$url = "https://github.com/nlohmann/json/releases/latest/download/json.hpp"
$output = "lib/json.hpp"

# Check if lib directory exists
if (-not (Test-Path "lib")) {
    Write-Host "Creating lib directory..." -ForegroundColor Yellow
    New-Item -ItemType Directory -Path "lib" | Out-Null
}

# Download the file
try {
    Write-Host "Downloading from: $url" -ForegroundColor Green
    Invoke-WebRequest -Uri $url -OutFile $output -UseBasicParsing
    
    if (Test-Path $output) {
        $fileSize = (Get-Item $output).Length / 1KB
        Write-Host ""
        Write-Host "✓ Successfully downloaded json.hpp" -ForegroundColor Green
        Write-Host "  Location: $output" -ForegroundColor Gray
        Write-Host "  Size: $([math]::Round($fileSize, 2)) KB" -ForegroundColor Gray
        Write-Host ""
        Write-Host "You can now build the project with CMake!" -ForegroundColor Cyan
    }
    else {
        Write-Host "✗ Download failed - file not created" -ForegroundColor Red
    }
}
catch {
    Write-Host ""
    Write-Host "✗ Error downloading json.hpp:" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Write-Host "Manual download instructions:" -ForegroundColor Yellow
    Write-Host "1. Visit: https://github.com/nlohmann/json/releases/latest/download/json.hpp" -ForegroundColor Yellow
    Write-Host "2. Save the file as 'json.hpp' in the 'lib/' folder" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
