# Implementation Plan

- [x] 1. Update Test Workflow to use immutable crc_fast library releases
  - Modify `.github/workflows/tests.yaml` to download pre-built library artifacts instead of building from source
  - Replace the repository checkout step with a release download step
  - Remove Rust toolchain installation and cargo build steps
  - Update library file copy steps to work with extracted release artifacts
  - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5_

- [x] 2. Create Windows Release Workflow file structure
  - Create `.github/workflows/windows-release.yaml` file
  - Configure workflow to trigger on `workflow_run` completion of Tests workflow
  - Set up workflow permissions for contents write and actions read
  - Add condition to check if triggering event was a tag push matching N.N.N pattern
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5_

- [ ] 3. Implement extension matrix generation job
- [ ] 3.1 Create get-extension-matrix job
  - Configure job to run on ubuntu-latest
  - Add condition to only run on tag pushes matching N.N.N
  - Use `php/php-windows-builder/extension-matrix@v1` action
  - Define matrix output for use by build job
  - _Requirements: 4.1, 4.2_

- [ ] 4. Implement Windows build job with crc_fast library integration
- [ ] 4.1 Set up build job matrix configuration
  - Configure job to depend on get-extension-matrix
  - Set up matrix strategy using output from matrix job
  - Pin runner to windows-2022 for stability
  - _Requirements: 4.1, 4.2, 4.3, 6.1, 6.2, 6.3, 6.4, 6.5_

- [ ] 4.2 Add crc_fast library download and extraction steps
  - Add step to download crc-fast Windows release artifact from GitHub
  - Map PHP architecture to library architecture (x64→x86_64, arm64→aarch64)
  - Extract library files to build directory
  - Verify presence of required header and lib files
  - _Requirements: 3.1, 3.2, 3.3, 3.4, 3.5_

- [ ] 4.3 Configure extension build with php-windows-builder
  - Use `php/php-windows-builder/extension@v1` action
  - Pass matrix parameters (php-version, arch, ts)
  - Configure library path for config.w32 to find crc_fast
  - Upload built DLL artifacts with descriptive names
  - _Requirements: 4.4, 4.5, 6.1, 6.2, 6.3, 6.4, 6.5_

- [ ] 5. Implement draft release creation job
- [ ] 5.1 Create release job with artifact collection
  - Configure job to depend on build job completion
  - Run on ubuntu-latest
  - Download all DLL artifacts from build job
  - _Requirements: 5.1, 5.2, 5.3_

- [ ] 5.2 Create draft release with DLL uploads
  - Use `php/php-windows-builder/release@v1` action with draft: true
  - Set release name to tag name
  - Upload all DLL artifacts to the draft release
  - Ensure release is not automatically published
  - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5_

- [ ]* 6. Test and validate workflows
- [ ]* 6.1 Test updated Test Workflow
  - Create a test branch and verify test workflow runs successfully
  - Verify library download and extraction works correctly
  - Confirm all existing tests pass with pre-built library
  - _Requirements: 2.1, 2.2, 2.3, 2.4, 2.5_

- [ ]* 6.2 Test Release Workflow with test tag
  - Create a test tag (e.g., 0.0.1-test) to trigger workflow
  - Verify workflow only runs after tests pass
  - Verify matrix generation produces expected configurations
  - Verify builds complete for all matrix combinations
  - Verify draft release is created with all DLL artifacts
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 4.1, 4.2, 4.3, 4.4, 4.5, 5.1, 5.2, 5.3, 5.4, 5.5_

- [ ]* 6.3 Validate DLL artifacts
  - Download DLLs from draft release
  - Verify DLL files are valid Windows PE files
  - Test loading DLLs in corresponding PHP versions
  - Verify extension functions work correctly
  - Clean up test release after validation
  - _Requirements: 4.5, 5.3, 6.1, 6.2, 6.3, 6.4, 6.5_
