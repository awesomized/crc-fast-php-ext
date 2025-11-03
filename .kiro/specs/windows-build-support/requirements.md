# Requirements Document

## Introduction

This feature enables building the crc_fast PHP extension on Windows systems by implementing a `config.w32` build configuration file. The extension depends on the crc_fast Rust library's C-compatible DLL, which must be located and linked during the build process. The build system should support both standard Windows library locations and custom paths specified by the user.

## Glossary

- **Build System**: The PHP Windows build infrastructure using `php-sdk-binary-tools` and Visual Studio toolchain
- **config.w32**: The Windows-specific build configuration file for PHP extensions, analogous to config.m4 on Unix systems
- **crc_fast Static Library**: The compiled Windows static library (crc_fast.lib) from the crc_fast Rust project
- **Extension**: The crc_fast PHP extension being built for Windows
- **Standard Library Locations**: Common Windows paths where libraries are typically installed (e.g., C:\Program Files, system directories)
- **Custom Library Path**: A user-specified directory containing the crc_fast library files (include and lib subdirectories)

## Requirements

### Requirement 1

**User Story:** As a Windows PHP developer, I want to build the crc_fast extension using the standard PHP build process, so that I can use hardware-accelerated CRC calculations in my Windows PHP applications

#### Acceptance Criteria

1. THE Build System SHALL create a config.w32 file that integrates with the PHP Windows build infrastructure
2. WHEN the configure script executes, THE Build System SHALL check for the presence of the crc_fast library header file (libcrc_fast.h)
3. WHEN the configure script executes, THE Build System SHALL check for the presence of the crc_fast static library file (crc_fast.lib)
4. THE Build System SHALL verify the PHP version is 8.1.0 or higher before proceeding with the build
5. WHEN all prerequisites are met, THE Build System SHALL enable the crc_fast extension for compilation

### Requirement 2

**User Story:** As a Windows PHP developer, I want the build system to automatically find the crc_fast library in standard Windows locations, so that I don't need to manually specify paths when the library is installed in typical locations

#### Acceptance Criteria

1. THE Build System SHALL search for the crc_fast library in standard Windows installation directories
2. THE Build System SHALL check the C:\Program Files directory tree for the crc_fast library
3. THE Build System SHALL check the C:\Program Files (x86) directory tree for the crc_fast library
4. WHEN the crc_fast library is found in a standard location, THE Build System SHALL automatically configure the include and library paths
5. WHEN the crc_fast library is not found in standard locations, THE Build System SHALL provide a clear error message indicating the library was not found

### Requirement 3

**User Story:** As a Windows PHP developer, I want to specify a custom path to the crc_fast library, so that I can build the extension when the library is installed in a non-standard location

#### Acceptance Criteria

1. THE Build System SHALL accept a --with-crc-fast configuration option with a directory path value
2. WHEN a custom path is provided, THE Build System SHALL verify the path contains the required include subdirectory with libcrc_fast.h
3. WHEN a custom path is provided, THE Build System SHALL verify the path contains the required lib subdirectory with the crc_fast static library
4. WHEN a custom path is provided and valid, THE Build System SHALL use the custom path instead of searching standard locations
5. WHEN a custom path is provided but invalid, THE Build System SHALL display an error message indicating which required files are missing

### Requirement 4

**User Story:** As a Windows PHP developer, I want the build system to properly link the crc_fast static library, so that the compiled extension can successfully load and execute CRC operations

#### Acceptance Criteria

1. THE Build System SHALL add the crc_fast include directory to the compiler include path
2. THE Build System SHALL add the crc_fast library directory to the linker library path
3. THE Build System SHALL statically link the crc_fast.lib library to the extension
4. THE Build System SHALL verify the crc_fast_digest_new symbol is available in the library
5. WHEN linking fails, THE Build System SHALL display an error message indicating the library or symbol could not be found

### Requirement 5

**User Story:** As a Windows PHP developer, I want the build configuration to match the Unix build behavior, so that the extension behaves consistently across platforms

#### Acceptance Criteria

1. THE Build System SHALL compile the extension with C++17 standard support
2. THE Build System SHALL define ZEND_ENABLE_STATIC_TSRMLS_CACHE=1 during compilation
3. THE Build System SHALL define CRC_FAST_EXCEPTIONS=0 to disable C++ exceptions
4. THE Build System SHALL define CRC_FAST_DEVELOPMENT_CHECKS=0 to disable development assertions
5. THE Build System SHALL compile php_crc_fast.cpp as the main extension source file
