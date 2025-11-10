# Requirements Document

## Introduction

This feature enables automated building and releasing of Windows DLL artifacts for the crc_fast PHP extension through GitHub Actions. The workflow will trigger on version tags, build the extension for multiple PHP versions and architectures, create a draft GitHub release, and attach the compiled DLL artifacts. The system must use immutable releases of the crc_fast Rust library dependency rather than building from the main branch.

## Glossary

- **Release Workflow**: The GitHub Actions workflow that builds Windows DLLs and creates draft releases
- **Test Workflow**: The existing GitHub Actions workflow that runs tests on the extension
- **crc_fast Library**: The Rust library dependency (libcrc_fast) that provides CRC calculation functionality
- **DLL Artifact**: The compiled Windows dynamic-link library file for the PHP extension
- **Draft Release**: A GitHub release that is created but not published, allowing human review before making it public
- **Version Tag**: A Git tag matching the pattern N.N.N (e.g., 1.0.0, 2.1.3) that triggers the release workflow
- **Immutable Release**: A specific tagged version of a dependency that does not change over time
- **Extension Matrix**: A set of PHP versions, architectures, and thread safety options to build against

## Requirements

### Requirement 1

**User Story:** As a maintainer, I want the release workflow to trigger only on version tags after tests pass, so that only validated code is built and released

#### Acceptance Criteria

1. WHEN a Git tag matching the pattern N.N.N is pushed, THE Release Workflow SHALL trigger after the Test Workflow completes successfully
2. THE Release Workflow SHALL NOT trigger on branch pushes
3. THE Release Workflow SHALL NOT trigger on pull requests
4. WHEN the Test Workflow fails, THE Release Workflow SHALL NOT execute
5. THE Release Workflow SHALL have write permissions for repository contents to create releases

### Requirement 2

**User Story:** As a maintainer, I want the Test Workflow to use immutable releases of the crc_fast library, so that test results are reproducible and not affected by changes in the main branch

#### Acceptance Criteria

1. THE Test Workflow SHALL download a specific tagged release of the crc_fast library instead of checking out the main branch
2. THE Test Workflow SHALL use version 1.7.0 or later of the crc_fast library
3. THE Test Workflow SHALL download pre-built library artifacts from the crc_fast GitHub releases
4. WHEN the library download fails, THE Test Workflow SHALL fail with a clear error message
5. THE Test Workflow SHALL extract the downloaded library files to the expected directory structure

### Requirement 3

**User Story:** As a maintainer, I want the release workflow to download and use pre-built crc_fast library releases, so that build times are faster and more reliable than building from source

#### Acceptance Criteria

1. THE Release Workflow SHALL download the latest tagged release of the crc_fast library from GitHub
2. THE Release Workflow SHALL download Windows-specific pre-built library artifacts
3. THE Release Workflow SHALL extract the library files to the correct directory structure for the PHP build system
4. WHEN the library download fails, THE Release Workflow SHALL fail with a clear error message
5. THE Release Workflow SHALL verify the required library files (header and lib files) are present after extraction

### Requirement 4

**User Story:** As a maintainer, I want the release workflow to build DLLs for multiple PHP versions and architectures, so that users on different PHP configurations can use the extension

#### Acceptance Criteria

1. THE Release Workflow SHALL generate a build matrix based on the extension's PHP version constraints
2. THE Release Workflow SHALL build the extension for each combination of PHP version, architecture, and thread safety option in the matrix
3. THE Release Workflow SHALL use the php-windows-builder GitHub Action for building
4. WHEN a build fails for any matrix combination, THE Release Workflow SHALL report the failure but continue building other combinations
5. THE Release Workflow SHALL produce DLL artifacts for each successful build

### Requirement 5

**User Story:** As a maintainer, I want the release workflow to create a draft GitHub release with DLL artifacts attached, so that I can review and edit the release before publishing it to users

#### Acceptance Criteria

1. WHEN all builds complete, THE Release Workflow SHALL create a draft GitHub release
2. THE Release Workflow SHALL name the release using the version tag that triggered the workflow
3. THE Release Workflow SHALL upload all built DLL artifacts to the draft release
4. THE Release Workflow SHALL NOT automatically publish the release
5. THE Release Workflow SHALL allow the maintainer to manually review, edit, and publish the release through the GitHub interface

### Requirement 6

**User Story:** As a maintainer, I want the release workflow to follow the same build configuration as local Windows builds, so that CI-built DLLs behave identically to locally-built ones

#### Acceptance Criteria

1. THE Release Workflow SHALL use the config.w32 build configuration file
2. THE Release Workflow SHALL compile with C++17 standard support
3. THE Release Workflow SHALL define ZEND_ENABLE_STATIC_TSRMLS_CACHE=1 during compilation
4. THE Release Workflow SHALL define CRC_FAST_EXCEPTIONS=0 during compilation
5. THE Release Workflow SHALL define CRC_FAST_DEVELOPMENT_CHECKS=0 during compilation
