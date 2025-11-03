# Implementation Plan

- [x] 1. Create config.w32 file with basic structure and argument parsing
  - Create config.w32 file in the extension root directory
  - Implement ARG_WITH for --with-crc-fast configuration option
  - Add PHP version check (require PHP 8.1.0 or higher)
  - Set up basic conditional logic for when extension is enabled
  - _Requirements: 1.1, 1.4, 1.5_

- [x] 2. Implement library path search and detection logic
  - [x] 2.1 Implement custom path handling
    - Parse user-provided path from --with-crc-fast=<path>
    - Validate custom path contains include/libcrc_fast.h
    - Validate custom path contains lib/crc_fast.lib
    - Set CRC_FAST_DIR variable when custom path is valid
    - _Requirements: 3.1, 3.2, 3.3, 3.4_
  
  - [x] 2.2 Implement standard location search
    - Define array of standard search paths (Program Files, vcpkg, Chocolatey, Scoop)
    - Implement loop to check each standard location
    - Use FileSystemObject to verify header and library files exist
    - Set CRC_FAST_DIR to first valid location found
    - _Requirements: 2.1, 2.2, 2.3, 2.4_
  
  - [x] 2.3 Implement error handling for missing library
    - Display clear error when library not found in standard locations
    - Display clear error when custom path is invalid
    - Display specific error for missing header file
    - Display specific error for missing library file
    - _Requirements: 2.5, 3.5_

- [x] 3. Configure compiler and linker settings
  - [x] 3.1 Add include path configuration
    - Use CHECK_HEADER_ADD_INCLUDE to add crc_fast include directory
    - Verify libcrc_fast.h is accessible
    - _Requirements: 4.1_
  
  - [x] 3.2 Add library path and linking configuration
    - Add library directory to linker search path
    - Use CHECK_LIB to verify crc_fast.lib exists and contains required symbols
    - Verify crc_fast_digest_new symbol is present
    - Link crc_fast.lib statically to the extension
    - _Requirements: 4.2, 4.3, 4.4_
  
  - [x] 3.3 Set compiler flags for C++17 and defines
    - Add /std:c++17 compiler flag
    - Add /DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 define
    - Add /DCRC_FAST_EXCEPTIONS=0 define
    - Add /DCRC_FAST_DEVELOPMENT_CHECKS=0 define
    - _Requirements: 5.1, 5.2, 5.3, 5.4_

- [x] 4. Register extension with build system
  - Use EXTENSION macro to register crc_fast extension
  - Specify php_crc_fast.cpp as the main source file
  - Configure extension to build as shared library
  - Pass all compiler flags to EXTENSION macro
  - _Requirements: 1.1, 5.5_

- [ ] 5. Test config.w32 with various scenarios
  - Test with library in standard Program Files location
  - Test with library in vcpkg location
  - Test with custom path specified
  - Test error handling when library is missing
  - Test error handling with invalid custom path
  - Verify extension builds successfully with nmake
  - Verify extension loads in PHP
  - Run existing .phpt tests to verify functionality
  - _Requirements: All_
