# Design Document

## Overview

This design transforms the existing single-configuration Tests workflow into a comprehensive matrix-based testing system that validates the crc-fast PHP extension across multiple operating systems (Linux, macOS, Windows), PHP versions (8.1-8.4), CPU architectures (x86_64, aarch64), and thread safety configurations (TS, NTS).

The design leverages GitHub Actions' matrix strategy to run tests in parallel across all valid combinations while excluding invalid configurations (e.g., Windows + aarch64). The workflow will adapt its build process based on the target platform, using Unix-style tools (phpize/configure/make) for Linux and macOS, and the php-windows-builder action for Windows.

## Architecture

### Matrix Strategy

The workflow uses a two-dimensional approach:

1. **Matrix Definition**: Define all possible combinations of OS, PHP version, architecture, and thread safety
2. **Matrix Exclusions**: Remove invalid combinations (Windows + aarch64)

```yaml
strategy:
  fail-fast: false
  matrix:
    os: [ubuntu-22.04, ubuntu-22.04-arm, macos-14, windows-2022]
    php-version: ['8.1', '8.2', '8.3', '8.4']
    ts: [ts, nts]
```

### Platform and Architecture Identification

The workflow needs to map GitHub runner OS identifiers to platform strings and architecture identifiers used in artifact names:

| Runner OS | Platform | Architecture |
|-----------|----------|--------------|
| ubuntu-22.04 | linux | x86_64 |
| ubuntu-22.04-arm | linux | aarch64 |
| macos-14 | macos | aarch64 |
| windows-2022 | windows | x86_64 |

This mapping will be implemented using conditional steps that set environment variables based on `matrix.os`.

This mapping will be done using a conditional step that sets environment variables based on `runner.os`.

### Conditional Build Processes

The workflow branches into platform-specific build paths:

**Unix-like (Linux, macOS)**:
1. Download and extract tar.gz library artifact
2. Run phpize
3. Run configure with library path
4. Run make
5. Run make test

**Windows**:
1. Download and extract zip library artifact
2. Use php-windows-builder/extension action with library path (includes building and testing)

## Components and Interfaces

### Job: test

The main test job runs for each matrix combination.

**Inputs** (from matrix):
- `matrix.os`: GitHub runner OS identifier (ubuntu-22.04, ubuntu-22.04-arm, macos-14, or windows-2022)
- `matrix.php-version`: PHP version string (e.g., '8.1')
- `matrix.ts`: Thread safety configuration (ts or nts)

**Derived Values** (from matrix.os):
- Platform identifier for artifact names (linux, macos, or windows)
- Architecture identifier for artifact names (x86_64 or aarch64)

**Outputs**:
- Test results (pass/fail)
- Test logs (on failure)

### Step: Setup PHP

Uses `shivammathur/setup-php` action to install the specified PHP version with the correct thread safety configuration.

**Configuration**:
- PHP version from matrix
- Extensions: none (we're building our own)
- Tools: phpize, php-config (Unix only)

For Windows, this step is handled by php-windows-builder.

### Step: Set Platform Variables

Sets environment variables for platform and architecture based on the runner OS.

**Logic**:
```yaml
- name: Set platform and architecture
  run: |
    case "${{ matrix.os }}" in
      ubuntu-22.04)
        echo "PLATFORM=linux" >> $GITHUB_ENV
        echo "ARCH=x86_64" >> $GITHUB_ENV
        ;;
      ubuntu-22.04-arm)
        echo "PLATFORM=linux" >> $GITHUB_ENV
        echo "ARCH=aarch64" >> $GITHUB_ENV
        ;;
      macos-14)
        echo "PLATFORM=macos" >> $GITHUB_ENV
        echo "ARCH=aarch64" >> $GITHUB_ENV
        ;;
      windows-2022)
        echo "PLATFORM=windows" >> $GITHUB_ENV
        echo "ARCH=x86_64" >> $GITHUB_ENV
        ;;
    esac
```

### Step: Download Library

Downloads the appropriate crc_fast library release artifact based on platform and architecture.

**Artifact Name Pattern**: `crc-fast-{version}-{platform}-{arch}.{ext}`
- `{version}`: Latest release tag from GitHub API
- `{platform}`: From PLATFORM environment variable
- `{arch}`: From ARCH environment variable
- `{ext}`: tar.gz for Unix, zip for Windows

**Authentication**:
To avoid GitHub API rate limiting, all API calls must include authentication using the `GITHUB_TOKEN`:
- Unix (bash/curl): Use `-H "Authorization: Bearer ${{ secrets.GITHUB_TOKEN }}"`
- Windows (PowerShell): Use `-Headers @{ "Authorization" = "Bearer ${{ secrets.GITHUB_TOKEN }}" }`

The `GITHUB_TOKEN` is automatically provided by GitHub Actions and doesn't need to be configured.

**Implementation Details**:

**Unix (Linux, macOS)**:
```bash
# Fetch latest release
LATEST_RELEASE=$(curl -sL -H "Authorization: Bearer ${{ secrets.GITHUB_TOKEN }}" \
  https://api.github.com/repos/awesomized/crc-fast-rust/releases/latest | jq -r .tag_name)

# Construct download URL
ARTIFACT_NAME="crc-fast-${LATEST_RELEASE}-${PLATFORM}-${ARCH}.tar.gz"
DOWNLOAD_URL="https://github.com/awesomized/crc-fast-rust/releases/download/${LATEST_RELEASE}/${ARTIFACT_NAME}"

# Download artifact
curl -sL -H "Authorization: Bearer ${{ secrets.GITHUB_TOKEN }}" -o crc-fast.tar.gz "${DOWNLOAD_URL}"
```

**Windows**:
The Windows implementation follows the proven pattern from the windows-release.yaml workflow:

```powershell
# Get latest release with proper headers
$releaseUrl = "https://api.github.com/repos/awesomized/crc-fast-rust/releases/latest"
$release = Invoke-RestMethod -Uri $releaseUrl -Headers @{
  "Accept" = "application/vnd.github+json"
  "User-Agent" = "GitHub-Actions"
  "Authorization" = "Bearer ${{ secrets.GITHUB_TOKEN }}"
}

$version = $release.tag_name

# Find the specific asset by name (validates artifact exists)
$artifactName = "crc-fast-$version-$env:PLATFORM-$env:ARCH.zip"
$asset = $release.assets | Where-Object { $_.name -eq $artifactName }

if (-not $asset) {
  # List available assets to help debugging
  Write-Error "ERROR: Could not find artifact '$artifactName' in release $version"
  Write-Host "Available assets:"
  $release.assets | ForEach-Object { Write-Host "  - $($_.name)" }
  exit 1
}

# Use browser_download_url from asset (more reliable than constructing URL)
$downloadUrl = $asset.browser_download_url

# Download with proper headers
Invoke-WebRequest -Uri $downloadUrl -OutFile "crc_fast_lib.zip" -Headers @{
  "Accept" = "application/octet-stream"
  "User-Agent" = "GitHub-Actions"
  "Authorization" = "Bearer ${{ secrets.GITHUB_TOKEN }}"
}

# Verify download and report size
if (-not (Test-Path "crc_fast_lib.zip")) {
  Write-Error "ERROR: Failed to download crc_fast library"
  exit 1
}

Write-Host "Successfully downloaded crc_fast library ($([math]::Round((Get-Item "crc_fast_lib.zip").Length / 1MB, 2)) MB)"
```

**Key Differences in Windows Implementation**:
1. **Asset Validation**: Searches release assets by name instead of constructing URL, providing better error messages
2. **Proper Headers**: Uses `Accept` and `User-Agent` headers in addition to `Authorization`
3. **browser_download_url**: Uses the URL from the asset object rather than constructing it manually
4. **Download Verification**: Checks file exists and reports file size for confirmation
5. **Error Messages**: Lists available assets when expected artifact is not found

**Error Handling**:
- Fail if latest release cannot be fetched
- Fail if artifact doesn't exist for the platform/arch combination (Windows lists available assets)
- Fail if download fails
- Fail if downloaded file doesn't exist (Windows only)

### Step: Extract Library

Extracts the downloaded artifact and verifies required files exist.

**Unix (Linux, macOS)**:
```bash
tar -xzf crc-fast.tar.gz
EXTRACT_DIR="crc-fast-${CRC_FAST_VERSION}-${PLATFORM}-${ARCH}"

# Verify include/libcrc_fast.h exists
if [ ! -f "${EXTRACT_DIR}/include/libcrc_fast.h" ]; then
  echo "Error: Required header file not found"
  exit 1
fi

# Verify lib/libcrc_fast.so (or .dylib on macOS) exists
if [ -f "${EXTRACT_DIR}/lib/libcrc_fast.so" ]; then
  LIB_FILE="${EXTRACT_DIR}/lib/libcrc_fast.so"
elif [ -f "${EXTRACT_DIR}/lib/libcrc_fast.dylib" ]; then
  LIB_FILE="${EXTRACT_DIR}/lib/libcrc_fast.dylib"
else
  echo "Error: Required library file not found"
  exit 1
fi

# Copy files to expected locations
mkdir -p lib include
cp "${LIB_FILE}" lib/
cp "${EXTRACT_DIR}/include/libcrc_fast.h" include/
```

**Windows**:
The Windows implementation follows the proven pattern from the windows-release.yaml workflow, which handles the versioned subdirectory structure:

```powershell
$extractPath = "C:\crc_fast"
$tempExtractPath = "C:\crc_fast_temp"

# Create temporary extraction directory
New-Item -ItemType Directory -Force -Path $tempExtractPath | Out-Null

# Extract to temp location first
Expand-Archive -Path "crc_fast_lib.zip" -DestinationPath $tempExtractPath -Force

# Find the versioned subdirectory (e.g., crc-fast-1.7.0-windows-x86_64)
$dirs = Get-ChildItem -Path $tempExtractPath -Directory

if ($dirs.Count -eq 0) {
  Write-Error "ERROR: No subdirectory found after extraction"
  exit 1
} elseif ($dirs.Count -gt 1) {
  Write-Error "ERROR: Multiple subdirectories found after extraction. Expected only one."
  $dirs | ForEach-Object { Write-Host "  $($_.FullName)" }
  exit 1
}

$versionedDir = $dirs[0]
Write-Host "Found versioned directory: $($versionedDir.Name)"

# Move contents from versioned directory to final location
New-Item -ItemType Directory -Force -Path $extractPath | Out-Null
Move-Item -Path "$($versionedDir.FullName)\*" -Destination $extractPath -Force

# Clean up temp directory
Remove-Item -Recurse -Force $tempExtractPath

# Verify include\libcrc_fast.h exists
if (-not (Test-Path "$extractPath\include\libcrc_fast.h")) {
  Write-Error "Error: Required header file not found"
  exit 1
}

# Verify lib\crc_fast.lib exists
if (-not (Test-Path "$extractPath\lib\crc_fast.lib")) {
  Write-Error "Error: Required library file not found"
  exit 1
}
```

**Key Differences in Windows Implementation**:
1. **Two-Stage Extraction**: Extracts to temporary directory first, then moves contents to final location
2. **Versioned Subdirectory Handling**: Automatically finds and handles the versioned subdirectory (e.g., `crc-fast-1.7.0-windows-x86_64`)
3. **Validation**: Ensures exactly one subdirectory exists after extraction
4. **Cleanup**: Removes temporary extraction directory after moving files
5. **File Verification**: Validates both required files exist at expected paths

### Step: Build Extension (Unix)

Runs the standard PHP extension build process.

```bash
phpize
./configure --with-crc-fast={library_path}
make
```

The library path points to the extracted library directory.

### Step: Build Extension (Windows)

Uses the php-windows-builder action which handles the entire build process.

```yaml
- uses: php/php-windows-builder/extension@v1
  with:
    php-version: ${{ matrix.php-version }}
    arch: x64
    ts: ${{ matrix.ts }}
    args: --with-crc-fast=C:\crc_fast
```

Note: Windows only runs on x86_64, which maps to 'x64' for the Windows builder.

### Step: Run Tests

Executes the test suite.

**Unix (Linux, macOS)**:
```bash
NO_INTERACTION=1 make test
```

**Windows**:
The php-windows-builder action runs tests automatically as part of the build process, so no separate test step is needed.

## Data Models

### Matrix Configuration Object

```typescript
interface MatrixConfig {
  os: 'ubuntu-22.04' | 'ubuntu-22.04-arm' | 'macos-14' | 'windows-2022';
  'php-version': '8.1' | '8.2' | '8.3' | '8.4';
  ts: 'ts' | 'nts';
}
```

### Platform and Architecture Mapping

```typescript
interface PlatformArchMap {
  'ubuntu-22.04': { platform: 'linux', arch: 'x86_64' };
  'ubuntu-22.04-arm': { platform: 'linux', arch: 'aarch64' };
  'macos-14': { platform: 'macos', arch: 'aarch64' };
  'windows-2022': { platform: 'windows', arch: 'x86_64' };
}
```

### Library File Paths

```typescript
interface LibraryPaths {
  unix: {
    header: 'include/libcrc_fast.h';
    library: 'lib/libcrc_fast.so' | 'lib/libcrc_fast.dylib';
  };
  windows: {
    header: 'include\\libcrc_fast.h';
    library: 'lib\\crc_fast.lib';
  };
}
```

## Error Handling

### Library Download Failures

**Scenario**: Latest release API call fails
- **Action**: Fail job with error message indicating API issue
- **Message**: "Error: Failed to fetch latest release version"
- **Note**: Ensure GITHUB_TOKEN is used in API calls to avoid rate limiting

**Scenario**: Artifact doesn't exist for platform/arch
- **Action**: Fail job with error message listing available artifacts
- **Message**: "Error: Could not find artifact 'crc-fast-{version}-{platform}-{arch}.{ext}'"

**Scenario**: Download fails (network issue, permissions)
- **Action**: Fail job with error message
- **Message**: "Error: Failed to download crc_fast library release"

### Library Extraction Failures

**Scenario**: Required header file missing after extraction
- **Action**: Fail job with error message
- **Message**: "Error: Required header file not found: {path}"

**Scenario**: Required library file missing after extraction
- **Action**: Fail job with error message
- **Message**: "Error: Required library file not found: {path}"

### Build Failures

**Scenario**: phpize fails (Unix)
- **Action**: Fail job, GitHub Actions automatically captures output
- **Recovery**: None, indicates environment issue

**Scenario**: configure fails (Unix)
- **Action**: Fail job with configure output
- **Recovery**: None, indicates library path or dependency issue

**Scenario**: make fails (Unix)
- **Action**: Fail job with compilation errors
- **Recovery**: None, indicates code or dependency issue

**Scenario**: php-windows-builder fails (Windows)
- **Action**: Fail job with builder output
- **Recovery**: None, handled by builder action

### Test Failures

**Scenario**: Tests fail
- **Action**: Fail job, preserve test output files
- **Artifacts**: .out, .log, .diff files from failed tests
- **Recovery**: None, indicates functional regression

## Testing Strategy

### Matrix Coverage

The matrix will generate the following number of test jobs:

**Linux**: 4 PHP versions × 2 architectures × 2 TS configs = 16 jobs
**macOS**: 4 PHP versions × 2 architectures × 2 TS configs = 16 jobs
**Windows**: 4 PHP versions × 1 architecture × 2 TS configs = 8 jobs

**Total**: 40 test jobs per workflow run

### Validation Approach

Each test job validates:
1. Library download and extraction for the specific platform/arch
2. Extension compilation with the correct PHP version and TS configuration
3. All existing test cases pass (via make test)

### Performance Considerations

- **Parallel Execution**: GitHub Actions runs matrix jobs in parallel (subject to runner availability)
- **fail-fast: false**: Ensures all combinations are tested even if some fail
- **Caching**: Consider caching downloaded library artifacts (future enhancement)

### Test Execution

The existing test suite (`.phpt` files in `tests/` directory) will run on each matrix combination. No changes to individual tests are required.

### Monitoring and Reporting

- Each matrix job appears as a separate entry in the workflow run UI
- Job names include matrix parameters for easy identification
- Failed jobs preserve logs and test output for debugging
- Overall workflow status reflects all matrix jobs (fails if any job fails)

## Implementation Notes

### Runner Availability

- **Linux x86_64**: ubuntu-22.04
- **Linux aarch64**: ubuntu-22.04-arm
- **macOS aarch64**: macos-14 (Apple Silicon)
- **Windows x86_64**: windows-2022

### PHP Setup Considerations

The `shivammathur/setup-php` action supports:
- All required PHP versions (8.1-8.4)
- Thread safety configuration via `ini-values: zend.assertions=-1` or similar
- Cross-platform support (Linux, macOS, Windows)

For Windows, the php-windows-builder handles PHP setup internally.

### Matrix Coverage Summary

The simplified matrix (without explicit arch dimension) generates:

**4 OS runners** × **4 PHP versions** × **2 TS configs** = **32 test jobs per workflow run**

Breaking down by platform:
- **Linux x86_64 (ubuntu-22.04)**: 8 jobs
- **Linux aarch64 (ubuntu-22.04-arm)**: 8 jobs
- **macOS ARM (macos-14)**: 8 jobs
- **Windows x86_64 (windows-2022)**: 8 jobs

This configuration provides comprehensive coverage across all major platforms and architectures that PHP supports, with a cleaner matrix definition since each runner is inherently tied to a specific architecture.
