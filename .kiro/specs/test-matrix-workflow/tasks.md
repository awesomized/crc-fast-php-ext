# Implementation Plan

- [x] 1. Update workflow matrix configuration
  - Replace single-configuration test job with matrix strategy
  - Define matrix dimensions: os (ubuntu-22.04, ubuntu-22.04-arm, macos-14, windows-2022), php-version (8.1-8.4), ts (ts, nts)
  - Set fail-fast to false to ensure all combinations are tested
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 2.1, 2.2, 2.3, 3.1, 3.2, 3.3, 4.1, 4.2, 5.1, 5.2_

- [x] 2. Add platform and architecture identification step
  - Create step that sets PLATFORM and ARCH environment variables based on matrix.os
  - Map ubuntu-22.04 to linux/x86_64
  - Map ubuntu-22.04-arm to linux/aarch64
  - Map macos-14 to macos/aarch64
  - Map windows-2022 to windows/x86_64
  - Use shell-appropriate syntax (bash for Unix, PowerShell for Windows)
  - _Requirements: 2.4, 3.5, 6.1_

- [x] 3. Update library download step for Unix platforms
  - Add conditional to run only on Linux and macOS (using if: runner.os != 'Windows')
  - Update curl command to use PLATFORM and ARCH environment variables in artifact name
  - Add GitHub token authentication header to avoid rate limiting: -H "Authorization: Bearer ${{ secrets.GITHUB_TOKEN }}"
  - Update artifact name pattern to use environment variables: crc-fast-${LATEST_RELEASE}-${PLATFORM}-${ARCH}.tar.gz
  - _Requirements: 5.3, 6.1, 6.2, 6.3_

- [x] 4. Update library download step for Windows platform
  - Add conditional to run only on Windows (using if: runner.os == 'Windows')
  - Update PowerShell script to use PLATFORM and ARCH environment variables
  - Add GitHub token authentication to API calls: -Headers @{ "Authorization" = "Bearer ${{ secrets.GITHUB_TOKEN }}" }
  - Update artifact name pattern to use environment variables: crc-fast-$version-$env:PLATFORM-$env:ARCH.zip
  - _Requirements: 2.6, 5.3, 6.1, 6.2, 6.6_

- [x] 5. Update library extraction step for Unix platforms
  - Add conditional to run only on Linux and macOS
  - Ensure extraction uses ARCH environment variable for directory name verification
  - Keep existing verification of header and library files
  - Handle both .so (Linux) and .dylib (macOS) library extensions
  - _Requirements: 6.3, 7.4_

- [x] 6. Update library extraction step for Windows platform
  - Add conditional to run only on Windows
  - Keep existing PowerShell extraction logic
  - Ensure verification of required files (include\libcrc_fast.h and lib\crc_fast.lib)
  - _Requirements: 6.3, 7.5_

- [x] 7. Add PHP setup step for Unix platforms
  - Add step using shivammathur/setup-php action
  - Configure with matrix.php-version
  - Add conditional to run only on Linux and macOS
  - Configure thread safety based on matrix.ts value
  - Install required tools: phpize, php-config
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 4.1, 4.2, 4.3_

- [x] 8. Update Unix build steps to use conditional execution
  - Add if: runner.os != 'Windows' to phpize step
  - Add if: runner.os != 'Windows' to configure step
  - Add if: runner.os != 'Windows' to make step
  - Ensure configure uses correct library path from extraction
  - _Requirements: 7.1, 7.2_

- [x] 9. Update Unix test step
  - Add if: runner.os != 'Windows' conditional
  - Keep existing NO_INTERACTION=1 make test command
  - _Requirements: 1.5, 8.3_

- [x] 10. Update Windows build step
  - Add if: runner.os == 'Windows' conditional
  - Update php-windows-builder/extension action to use matrix.php-version
  - Set arch to 'x64' (Windows only supports x86_64)
  - Set ts to matrix.ts value
  - Keep args: --with-crc-fast=C:\crc_fast
  - _Requirements: 1.1, 1.2, 1.3, 1.4, 4.1, 4.2, 4.3, 7.3_

- [x] 11. Update workflow job naming for clarity
  - Configure job name to include matrix parameters for easy identification
  - Use format: "Test (OS, PHP version, TS/NTS)"
  - Example: "Test (ubuntu-22.04, 8.1, ts)"
  - _Requirements: 5.4, 8.1, 8.2_

- [x] 12. Verify workflow triggers remain unchanged
  - Ensure workflow still triggers on push, pull_request, workflow_dispatch, and workflow_call
  - Confirm no changes needed to trigger configuration
  - _Requirements: 5.5_

- [x] 13. Fix macOS dynamic library loading for tests
  - Add step before Unix test step to set DYLD_LIBRARY_PATH on macOS
  - Set DYLD_LIBRARY_PATH to point to the lib directory containing libcrc_fast.dylib
  - Use conditional to run only on macOS (if: runner.os == 'macOS')
  - Export environment variable for subsequent test step
  - _Requirements: 1.5, 8.3_
