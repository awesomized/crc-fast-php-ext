# Requirements Document

## Introduction

This feature enhances the GitHub Actions Tests workflow to use a matrix strategy that ensures comprehensive test coverage across multiple operating systems, PHP versions, CPU architectures, and thread safety configurations. The current workflow only tests on a single configuration (ubuntu-latest with default PHP), which leaves gaps in compatibility verification. This enhancement will provide confidence that the crc-fast PHP extension works correctly across all supported environments.

## Glossary

- **Test_Workflow**: The GitHub Actions workflow defined in `.github/workflows/tests.yaml` that builds and tests the crc-fast PHP extension
- **Matrix_Strategy**: A GitHub Actions feature that allows running a job multiple times with different variable combinations
- **TS**: Thread Safe - a PHP build configuration that includes thread safety mechanisms
- **NTS**: Non-Thread Safe - a PHP build configuration without thread safety mechanisms, typically used in single-threaded environments
- **Architecture**: The CPU instruction set architecture (x86_64 or aarch64)
- **PHP_Version**: The major.minor version of PHP (8.1, 8.2, 8.3, or 8.4)
- **Operating_System**: The target operating system platform (Linux, macOS, or Windows)
- **CRC_Fast_Library**: The underlying Rust library (libcrc_fast) that the PHP extension depends on
- **Platform_Identifier**: The string used to identify the operating system in artifact names (linux, macos, or windows)

## Requirements

### Requirement 1

**User Story:** As a maintainer, I want the test workflow to validate all supported PHP versions, so that I can ensure compatibility across the PHP ecosystem

#### Acceptance Criteria

1. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test against PHP version 8.1
2. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test against PHP version 8.2
3. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test against PHP version 8.3
4. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test against PHP version 8.4
5. WHEN any PHP_Version test fails, THE Test_Workflow SHALL report the failure with the specific PHP_Version identifier

### Requirement 2

**User Story:** As a maintainer, I want the test workflow to validate all supported operating systems, so that I can ensure the extension works across different platforms

#### Acceptance Criteria

1. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test on Linux Operating_System
2. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test on macOS Operating_System
3. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test on Windows Operating_System
4. WHEN downloading the CRC_Fast_Library, THE Test_Workflow SHALL use the appropriate Platform_Identifier for the current Operating_System
5. WHEN any Operating_System test fails, THE Test_Workflow SHALL report the failure with the specific Operating_System identifier

### Requirement 3

**User Story:** As a maintainer, I want the test workflow to validate supported CPU architectures for each platform, so that I can ensure the extension works on different hardware platforms

#### Acceptance Criteria

1. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test on x86_64 Architecture for all Operating_System platforms
2. WHEN the Test_Workflow executes on Linux Operating_System, THE Test_Workflow SHALL test on aarch64 Architecture
3. WHEN the Test_Workflow executes on macOS Operating_System, THE Test_Workflow SHALL test on aarch64 Architecture
4. WHEN the Test_Workflow executes on Windows Operating_System, THE Test_Workflow SHALL NOT test on aarch64 Architecture
5. WHEN downloading the CRC_Fast_Library, THE Test_Workflow SHALL select the artifact matching the current Architecture
6. WHEN any Architecture test fails, THE Test_Workflow SHALL report the failure with the specific Architecture identifier

### Requirement 4

**User Story:** As a maintainer, I want the test workflow to validate both thread safety configurations, so that I can ensure the extension works correctly in different PHP deployment scenarios

#### Acceptance Criteria

1. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test with TS PHP builds
2. WHEN the Test_Workflow executes, THE Test_Workflow SHALL test with NTS PHP builds
3. WHEN configuring the PHP extension build, THE Test_Workflow SHALL apply the appropriate thread safety configuration
4. WHEN any thread safety configuration test fails, THE Test_Workflow SHALL report the failure with the specific configuration identifier

### Requirement 5

**User Story:** As a maintainer, I want the test workflow to use a matrix strategy, so that all combinations of operating systems, PHP versions, architectures, and thread safety configurations are tested efficiently

#### Acceptance Criteria

1. THE Test_Workflow SHALL define a Matrix_Strategy with dimensions for Operating_System, PHP_Version, Architecture, and thread safety configuration
2. WHEN the Test_Workflow executes, THE Test_Workflow SHALL create a separate job for each matrix combination
3. THE Test_Workflow SHALL execute all matrix jobs in parallel where resources permit
4. WHEN viewing workflow results, THE Test_Workflow SHALL display each matrix combination as a distinct job with identifiable parameters
5. THE Test_Workflow SHALL complete successfully only when all matrix combinations pass

### Requirement 6

**User Story:** As a maintainer, I want the workflow to download the correct library artifacts for each test configuration, so that tests run with the appropriate dependencies

#### Acceptance Criteria

1. WHEN downloading the CRC_Fast_Library, THE Test_Workflow SHALL construct the artifact name using the current Operating_System and Architecture from the matrix
2. WHEN the CRC_Fast_Library artifact for a specific Operating_System and Architecture combination is unavailable, THE Test_Workflow SHALL fail with a descriptive error message
3. WHEN extracting the CRC_Fast_Library, THE Test_Workflow SHALL verify the presence of required header and library files before proceeding
4. WHEN running on Linux Operating_System, THE Test_Workflow SHALL use Platform_Identifier "linux" in artifact names
5. WHEN running on macOS Operating_System, THE Test_Workflow SHALL use Platform_Identifier "macos" in artifact names
6. WHEN running on Windows Operating_System, THE Test_Workflow SHALL use Platform_Identifier "windows" in artifact names

### Requirement 7

**User Story:** As a maintainer, I want the workflow to use platform-appropriate build processes, so that the extension builds correctly on each operating system

#### Acceptance Criteria

1. WHEN building on Linux Operating_System, THE Test_Workflow SHALL use phpize, configure, and make commands
2. WHEN building on macOS Operating_System, THE Test_Workflow SHALL use phpize, configure, and make commands
3. WHEN building on Windows Operating_System, THE Test_Workflow SHALL use the php-windows-builder action
4. WHEN extracting library files on Linux or macOS, THE Test_Workflow SHALL handle tar.gz archive format
5. WHEN extracting library files on Windows, THE Test_Workflow SHALL handle zip archive format

### Requirement 8

**User Story:** As a maintainer, I want clear visibility into which test configurations pass or fail, so that I can quickly identify compatibility issues

#### Acceptance Criteria

1. WHEN a matrix job fails, THE Test_Workflow SHALL include the Operating_System, PHP_Version, Architecture, and thread safety configuration in the failure report
2. WHEN viewing the workflow run summary, THE Test_Workflow SHALL display the status of each matrix combination separately
3. THE Test_Workflow SHALL preserve test output and logs for failed matrix jobs
4. WHEN all matrix jobs complete, THE Test_Workflow SHALL provide an overall pass/fail status
