# Design Document

## Overview

This design implements automated Windows release building for the crc_fast PHP extension using GitHub Actions. The solution consists of two main components:

1. **Updated Test Workflow**: Modify the existing test workflow to use immutable releases of the crc_fast library instead of building from the main branch
2. **New Release Workflow**: Create a new workflow that triggers on version tags, builds Windows DLLs for multiple PHP configurations, and creates draft releases with artifacts

The design leverages the existing `php-windows-builder` GitHub Action infrastructure and follows GitHub Actions best practices for artifact management and release creation.

## Architecture

### Workflow Dependency Chain

```
Push Tag (N.N.N)
    ↓
Test Workflow (tests.yaml)
    ↓ (on success)
Release Workflow (windows-release.yaml)
    ↓
Draft Release Created with DLL Artifacts
```

### Component Interaction

```
┌─────────────────────────────────────────────────────────────┐
│                     GitHub Actions Trigger                   │
│                  (Tag push matching N.N.N)                   │
└────────────────────────────┬────────────────────────────────┘
                             │
                             ↓
┌─────────────────────────────────────────────────────────────┐
│                      Test Workflow                           │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ 1. Download crc_fast library release (v1.7.0+)        │ │
│  │ 2. Extract library files                              │ │
│  │ 3. Build PHP extension                                │ │
│  │ 4. Run tests                                          │ │
│  └────────────────────────────────────────────────────────┘ │
└────────────────────────────┬────────────────────────────────┘
                             │ (on success)
                             ↓
┌─────────────────────────────────────────────────────────────┐
│                   Release Workflow                           │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Job 1: Generate Extension Matrix                      │ │
│  │  - Read composer.json/package.xml                     │ │
│  │  - Output matrix of PHP versions/arch/ts             │ │
│  └────────────────────────────────────────────────────────┘ │
│                             ↓                                │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Job 2: Build (Matrix)                                 │ │
│  │  - Download crc_fast Windows library release          │ │
│  │  - Extract to build directory                         │ │
│  │  - Build extension using php-windows-builder          │ │
│  │  - Upload DLL artifacts                               │ │
│  └────────────────────────────────────────────────────────┘ │
│                             ↓                                │
│  ┌────────────────────────────────────────────────────────┐ │
│  │ Job 3: Create Draft Release                           │ │
│  │  - Download all build artifacts                       │ │
│  │  - Create draft GitHub release                        │ │
│  │  - Upload DLL files to release                        │ │
│  └────────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

## Components and Interfaces

### 1. Test Workflow Updates (.github/workflows/tests.yaml)

**Purpose**: Modify the existing test workflow to use immutable crc_fast library releases

**Changes Required**:
- Replace the `Checkout crc_fast library` step that clones the repository
- Add a step to download the latest release artifact from `awesomized/crc-fast-rust`
- Add a step to extract the downloaded artifact to the expected directory structure
- Remove the Rust toolchain installation and cargo build steps
- Update the library copy steps to work with pre-built artifacts

**Interface with crc_fast Releases**:
- GitHub API endpoint: `https://api.github.com/repos/awesomized/crc-fast-rust/releases/latest`
- Expected artifact naming: `libcrc_fast-{version}-{platform}-{arch}.tar.gz` or similar
- Required files after extraction:
  - `libcrc_fast.so` (Linux shared library)
  - `libcrc_fast.h` (C header file)

### 2. Release Workflow (.github/workflows/windows-release.yaml)

**Purpose**: Build Windows DLLs and create draft releases

**Job 1: get-extension-matrix**
- **Condition**: Only runs if workflow was triggered by a tag push matching N.N.N pattern
- **Input**: Repository files (composer.json or package.xml)
- **Action**: `php/php-windows-builder/extension-matrix@v1`
- **Output**: JSON matrix of build configurations
- **Matrix dimensions**:
  - `php-version`: Array of PHP versions (e.g., ["8.1", "8.2", "8.3"])
  - `arch`: Array of architectures (e.g., ["x64", "x86"])
  - `ts`: Array of thread safety options (e.g., ["ts", "nts"])
  - `os`: Windows runner OS (pinned version for stability, e.g., "windows-2022")

**Job 2: build**
- **Dependencies**: Requires `get-extension-matrix` job completion
- **Runs on**: Windows runners (pinned to windows-2022 for stability and backwards compatibility)
- **Steps**:
  1. Checkout repository
  2. Download crc_fast Windows library release
     - Use GitHub API or direct download from releases
     - Target: Latest release or specific version (1.7.0+)
     - Artifact pattern: `crc-fast-{version}-windows-{arch}.zip`
     - Map PHP arch to library arch: x64 → x86_64, x86 → x86_64 (32-bit uses 64-bit lib), arm64 → aarch64
  3. Extract library files
     - Extract to a known location (e.g., `C:\crc_fast`)
     - Verify presence of required files:
       - `include/libcrc_fast.h`
       - `lib/crc_fast.lib` (static library)
  4. Build extension
     - Use `php/php-windows-builder/extension@v1`
     - Pass matrix parameters: php-version, arch, ts
     - Pass crc_fast library path via build arguments
  5. Upload artifacts
     - Artifact name pattern: `php_crc_fast-{php-version}-{arch}-{ts}.dll`
     - Retention: Until release job completes

**Job 3: release**
- **Dependencies**: Requires `build` job completion
- **Steps**:
  1. Download all build artifacts
  2. Create draft release
     - Use `php/php-windows-builder/release@v1` with `draft: true`
     - Release name: Tag name (e.g., "1.0.0")
     - Tag: `github.ref`
  3. Upload DLL files to release
     - All artifacts from build job
     - Maintain naming convention for easy identification

### 3. Integration with php-windows-builder

**Extension Matrix Action**:
- Reads PHP version constraints from `composer.json`
- Generates matrix based on supported PHP versions
- Outputs JSON for use in build job matrix strategy

**Extension Build Action**:
- Inputs:
  - `php-version`: PHP version to build against
  - `arch`: Architecture (x64 or x86)
  - `ts`: Thread safety (ts or nts)
  - Additional inputs for library paths (if needed)
- Process:
  - Sets up PHP SDK build environment
  - Runs `buildconf` in PHP source
  - Runs `configure` with extension enabled
  - Runs `nmake` to build
  - Collects DLL artifacts
- Outputs:
  - Built DLL file as GitHub Actions artifact

**Release Action**:
- Inputs:
  - `release`: GitHub ref (tag)
  - `token`: GitHub token for API access
  - `draft`: Set to 'true' for draft releases
- Process:
  - Downloads all artifacts from previous jobs
  - Creates GitHub release via API
  - Uploads artifacts to release
- Outputs:
  - Release URL

## Data Models

### Workflow Trigger Event
```yaml
on:
  workflow_run:
    workflows: ["Tests"]
    types: [completed]

# The workflow checks if the triggering event was a tag push
# matching N.N.N pattern before executing any jobs
```

### Build Matrix Structure
```json
{
  "include": [
    {
      "php-version": "8.1",
      "arch": "x64",
      "ts": "ts",
      "os": "windows-2022"
    },
    {
      "php-version": "8.1",
      "arch": "x64",
      "ts": "nts",
      "os": "windows-2022"
    },
    // ... additional combinations
  ]
}
```

### Artifact Naming Convention
```
php_crc_fast-{php-version}-{arch}-{ts}.dll

Examples:
- php_crc_fast-8.1-x64-ts.dll
- php_crc_fast-8.2-x64-nts.dll
- php_crc_fast-8.3-x86-ts.dll
```

### crc_fast Library Release Structure
```
crc-fast-{version}-windows-{arch}.zip

Examples:
- crc-fast-1.7.0-windows-x86_64.zip
- crc-fast-1.7.0-windows-aarch64.zip

Expected contents after extraction:
├── include/
│   └── libcrc_fast.h
└── lib/
    └── crc_fast.lib
```

## Error Handling

### Test Workflow Error Scenarios

1. **Library Download Failure**
   - **Detection**: HTTP error or missing release
   - **Handling**: Fail workflow with clear error message
   - **Message**: "Failed to download crc_fast library release v{version}"

2. **Library Extraction Failure**
   - **Detection**: Missing expected files after extraction
   - **Handling**: Fail workflow with file listing
   - **Message**: "Required library files not found: {missing_files}"

3. **Build Failure**
   - **Detection**: Non-zero exit code from make
   - **Handling**: Fail workflow, preserve build logs
   - **Message**: Standard make error output

### Release Workflow Error Scenarios

1. **Matrix Generation Failure**
   - **Detection**: Empty or invalid matrix output
   - **Handling**: Fail workflow before build job starts
   - **Message**: "Failed to generate build matrix from composer.json"

2. **Library Download Failure (Windows)**
   - **Detection**: HTTP error or missing Windows release artifact
   - **Handling**: Fail specific matrix build, continue others
   - **Message**: "Failed to download Windows crc_fast library for {arch}"

3. **Build Failure (Specific Configuration)**
   - **Detection**: Non-zero exit code from php-windows-builder
   - **Handling**: Mark matrix item as failed, continue others
   - **Message**: Build action error output
   - **Result**: Release created with available DLLs, failed builds noted

4. **Release Creation Failure**
   - **Detection**: GitHub API error
   - **Handling**: Fail release job, preserve artifacts
   - **Message**: "Failed to create GitHub release: {api_error}"
   - **Recovery**: Artifacts remain available for manual release creation

5. **Artifact Upload Failure**
   - **Detection**: Upload error for specific DLL
   - **Handling**: Log error, continue with other artifacts
   - **Message**: "Failed to upload {dll_name} to release"

### Workflow Dependency Failures

1. **Test Workflow Failure**
   - **Detection**: `workflow_run.conclusion != 'success'`
   - **Handling**: Skip release workflow entirely
   - **Message**: Workflow not triggered (no error message needed)

2. **Non-Tag Push**
   - **Detection**: Workflow triggered but not from a tag push matching N.N.N
   - **Handling**: Skip entire workflow (no jobs execute)
   - **Message**: No message (expected behavior)

## Testing Strategy

### Test Workflow Validation

1. **Unit-level Testing**
   - Verify library download step works with mock releases
   - Verify extraction produces expected directory structure
   - Verify existing tests still pass with pre-built library

2. **Integration Testing**
   - Test complete workflow on a feature branch
   - Verify all test cases pass
   - Compare performance with previous build-from-source approach

3. **Regression Testing**
   - Ensure all existing tests continue to pass
   - Verify test output matches previous results
   - Check that test execution time is reduced

### Release Workflow Validation

1. **Matrix Generation Testing**
   - Verify matrix includes all expected PHP versions
   - Verify matrix includes both architectures (x64, x86)
   - Verify matrix includes both thread safety options (ts, nts)
   - Test with different composer.json constraints

2. **Build Testing**
   - Test build for each matrix combination individually
   - Verify DLL artifacts are created
   - Verify DLL files are valid Windows PE files
   - Test loading DLL in corresponding PHP version

3. **Release Creation Testing**
   - Test draft release creation with mock artifacts
   - Verify all artifacts are attached to release
   - Verify release is marked as draft
   - Verify release can be edited and published manually

4. **End-to-End Testing**
   - Create a test tag (e.g., 0.0.1-test)
   - Verify test workflow runs and passes
   - Verify release workflow triggers after test success
   - Verify draft release is created with all DLLs
   - Verify DLLs can be downloaded and loaded
   - Clean up test release

### Error Scenario Testing

1. **Library Download Failures**
   - Test with invalid version number
   - Test with missing release artifact
   - Verify error messages are clear

2. **Build Failures**
   - Test with intentionally broken code
   - Verify partial failures don't block other builds
   - Verify artifacts from successful builds are still uploaded

3. **Release Failures**
   - Test with invalid GitHub token
   - Test with duplicate release tag
   - Verify artifacts are preserved for manual recovery

### Validation Criteria

1. **Test Workflow Success Criteria**
   - All tests pass with pre-built library
   - Workflow execution time is reduced (no Rust compilation)
   - Library version is clearly logged in workflow output

2. **Release Workflow Success Criteria**
   - Draft release is created for version tags
   - All expected DLL variants are attached
   - DLLs are correctly named and organized
   - Release is not automatically published

3. **Quality Criteria**
   - Built DLLs match locally-built versions (same functionality)
   - DLLs load successfully in target PHP versions
   - Extension functions work correctly in all configurations
   - No regressions in test suite

## Implementation Notes

### crc_fast Library Release Discovery

The crc_fast library releases can be accessed via:
- GitHub Releases page: `https://github.com/awesomized/crc-fast-rust/releases`
- GitHub API: `https://api.github.com/repos/awesomized/crc-fast-rust/releases/latest`
- Direct download: `https://github.com/awesomized/crc-fast-rust/releases/download/{tag}/{artifact}`

Artifact naming patterns:
- Linux: `libcrc_fast-{version}-linux-{arch}.tar.gz` (to be verified)
- Windows: `crc-fast-{version}-windows-{arch}.zip`
  - x64 architecture: `crc-fast-{version}-windows-x86_64.zip`
  - ARM64 architecture: `crc-fast-{version}-windows-aarch64.zip`

### Windows Build Environment

The php-windows-builder action handles:
- PHP SDK setup
- Visual Studio toolchain configuration
- PHP source download and configuration
- Extension compilation
- Artifact collection

**Windows Runner Version**: The workflow uses `windows-2022` instead of `windows-latest` to ensure:
- Predictable build environment across releases
- Maximum backwards compatibility with older Windows versions
- Protection against breaking changes when GitHub updates "latest"
- Consistent toolchain versions (Visual Studio, Windows SDK)

The extension's `config.w32` file must properly locate the crc_fast library. This may require:
- Environment variables set by the workflow
- Command-line arguments passed to configure
- Hardcoded paths in the workflow (less flexible)

### Draft Release Workflow

Draft releases allow:
- Human review before publication
- Editing release notes
- Adding additional documentation
- Verifying DLL integrity
- Testing downloads before public release

The maintainer workflow:
1. Tag is pushed
2. Workflows run automatically
3. Draft release appears in GitHub Releases
4. Maintainer reviews DLLs and release notes
5. Maintainer edits release description
6. Maintainer publishes release

### Workflow Permissions

Required permissions for release workflow:
```yaml
permissions:
  contents: write  # For creating releases and uploading assets
  actions: read    # For downloading artifacts from build job
```

### Caching Strategy

Consider caching:
- PHP SDK downloads (handled by php-windows-builder)
- crc_fast library downloads (cache by version)
- Composer dependencies (if needed)

Cache keys should include:
- Library version
- Architecture
- OS version

### Monitoring and Notifications

Consider adding:
- Workflow status badges in README
- Slack/email notifications on workflow failures
- Metrics tracking for build times
- Success rate monitoring for matrix builds
