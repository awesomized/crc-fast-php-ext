# Design Document: Windows Build Support for crc_fast Extension

## Overview

This design implements Windows build support for the crc_fast PHP extension by creating a `config.w32` file that mirrors the functionality of the existing Unix `config.m4` file. The Windows build system uses JScript-based configuration files that integrate with the PHP SDK build tools and Visual Studio toolchain.

The design follows PHP's standard Windows extension build patterns, using the ARG_WITH macro for configuration options, CHECK_HEADER_ADD_INCLUDE for header detection, CHECK_LIB for library verification, and ADD_FLAG for compiler/linker options.

## Architecture

### Build Configuration Flow

```
User runs configure
    ↓
config.w32 executes
    ↓
Parse --with-crc-fast argument
    ↓
Determine search paths (custom or standard)
    ↓
Search for libcrc_fast.h header
    ↓
Search for crc_fast.lib static library
    ↓
Verify library symbols
    ↓
Configure compiler flags
    ↓
Configure linker flags
    ↓
Register extension for build
```

### Directory Structure

The crc_fast Rust library distribution for Windows provides a static library:

```
<install_prefix>/
├── include/
│   └── libcrc_fast.h
└── lib/
    └── crc_fast.lib (static library - ~15 MB)
```

This extension uses **static linking** with `crc_fast.lib` to embed all code directly into the extension DLL, eliminating runtime dependencies.

Standard search locations on Windows:
- `C:\Program Files\crc_fast`
- `C:\Program Files (x86)\crc_fast`
- `C:\crc_fast`
- `C:\vcpkg\installed\x64-windows` (vcpkg default)
- `C:\tools\crc_fast` (Chocolatey default)
- `%USERPROFILE%\scoop\apps\crc_fast\current` (Scoop default)
- User-specified path via `--with-crc-fast=<path>`

## Components and Interfaces

### config.w32 File Structure

The config.w32 file will be organized into the following sections:

1. **Argument Parsing**: Define and parse the `--with-crc-fast` configuration option
2. **Path Resolution**: Determine library search paths based on user input or defaults
3. **Header Detection**: Locate and verify the libcrc_fast.h header file
4. **Library Detection**: Locate and verify the crc_fast.lib static library
5. **Symbol Verification**: Confirm required symbols exist in the library
6. **Compiler Configuration**: Set C++ standard, defines, and include paths
7. **Linker Configuration**: Set library paths and link the static library
8. **Extension Registration**: Register the extension with the build system

### Key Windows Build System APIs

The design uses these PHP Windows build system functions:

- `ARG_WITH(name, description, default)`: Define a configuration argument
- `CHECK_HEADER_ADD_INCLUDE(header, flag, path, ...)`: Search for header and add include path
- `CHECK_LIB(libname, ext, path)`: Verify library exists at specified path
- `ADD_FLAG(flag, value)`: Add compiler or linker flags
- `EXTENSION(name, sources, shared, cflags)`: Register extension for compilation
- `ADD_SOURCES(path, files, ext)`: Add source files to extension
- `WARNING(message)`: Display warning message
- `ERROR(message)`: Display error and halt configuration

### Path Search Strategy

1. If `--with-crc-fast=<path>` is provided:
   - Use the specified path exclusively
   - Verify `<path>\include\libcrc_fast.h` exists
   - Verify `<path>\lib\crc_fast.lib` exists
   - Error if either file is missing

2. If `--with-crc-fast` is provided without a path:
   - Search standard Windows locations in order:
     - `C:\Program Files\crc_fast`
     - `C:\Program Files (x86)\crc_fast`
     - `C:\crc_fast`
     - `C:\vcpkg\installed\x64-windows` (vcpkg)
     - `C:\vcpkg\installed\x86-windows` (vcpkg 32-bit)
     - `C:\tools\crc_fast` (Chocolatey)
     - `%USERPROFILE%\scoop\apps\crc_fast\current` (Scoop)
   - Use first location where both header and library are found
   - Error if not found in any standard location

3. If `--with-crc-fast=no` or not specified:
   - Skip extension build

## Data Models

### Configuration State

The config.w32 script maintains these configuration variables:

```javascript
PHP_CRC_FAST          // User's --with-crc-fast value ("no", "yes", or path)
CRC_FAST_DIR          // Resolved installation directory
CRC_FAST_INCLUDE_DIR  // Full path to include directory
CRC_FAST_LIB_DIR      // Full path to lib directory
```

### Build Flags

Compiler flags to be set:
- `/std:c++17` - Enable C++17 standard
- `/DZEND_ENABLE_STATIC_TSRMLS_CACHE=1` - Enable thread-safe resource manager cache
- `/DCRC_FAST_EXCEPTIONS=0` - Disable C++ exceptions
- `/DCRC_FAST_DEVELOPMENT_CHECKS=0` - Disable development assertions

Linker flags:
- `/LIBPATH:<CRC_FAST_LIB_DIR>` - Add library search path
- `crc_fast.lib` - Link static library

## Error Handling

### Error Scenarios and Messages

1. **Library not found in standard locations**:
   ```
   ERROR: crc_fast library not found in standard locations.
   Please specify the installation path using --with-crc-fast=<path>
   ```

2. **Header file not found at specified path**:
   ```
   ERROR: Cannot find libcrc_fast.h in <path>\include
   Please verify the crc_fast library is properly installed
   ```

3. **Static library not found at specified path**:
   ```
   ERROR: Cannot find crc_fast.lib in <path>\lib
   Please verify the crc_fast library is properly installed
   ```

4. **Required symbol not found**:
   ```
   ERROR: crc_fast library does not contain required symbol: crc_fast_digest_new
   Please verify you have the correct version of the crc_fast library
   ```

5. **PHP version too old**:
   ```
   ERROR: crc_fast extension requires PHP 8.1.0 or higher
   Current PHP version: <version>
   ```

### Validation Strategy

The config.w32 script will validate in this order:
1. PHP version check (fail fast if too old)
2. Argument parsing (determine user intent)
3. Path resolution (find library location)
4. Header existence (verify header file)
5. Library existence (verify static library)
6. Symbol verification (confirm library is correct version)
7. Configuration (only if all validations pass)

## Testing Strategy

### Manual Testing Approach

Since config.w32 is a build configuration script, testing will be manual and scenario-based:

1. **Standard Location Test**:
   - Install crc_fast library to `C:\Program Files\crc_fast`
   - Run: `configure --enable-crc-fast`
   - Verify: Build succeeds and finds library automatically

2. **Custom Path Test**:
   - Install crc_fast library to custom location (e.g., `D:\libs\crc_fast`)
   - Run: `configure --with-crc-fast=D:\libs\crc_fast`
   - Verify: Build succeeds using specified path

3. **Missing Library Test**:
   - Run: `configure --enable-crc-fast` (without library installed)
   - Verify: Clear error message indicating library not found

4. **Invalid Path Test**:
   - Run: `configure --with-crc-fast=C:\invalid\path`
   - Verify: Clear error message indicating files not found at path

5. **Build and Load Test**:
   - Complete build with: `nmake`
   - Load extension in PHP
   - Run: `php -m | findstr crc_fast`
   - Verify: Extension loads successfully

6. **Functionality Test**:
   - Run existing PHP tests: `nmake test`
   - Verify: All tests pass on Windows

### Integration with Existing Tests

The existing `.phpt` test files in the `tests/` directory should work without modification on Windows once the extension builds successfully. The test suite will validate:
- CRC-32 variants (ISO-HDLC, ISCSI, PHP)
- CRC-64 variants (NVME)
- Digest operations (new, write, finalize, reset)
- File hashing operations
- Algorithm enumeration

## Implementation Notes

### JScript Syntax Considerations

Windows config.w32 files use JScript syntax, which differs from the shell script syntax used in config.m4:
- Use `==` for string comparison (not `=`)
- Use `&&` for logical AND (not `-a`)
- Use `||` for logical OR (not `-o`)
- Use `FSO` (FileSystemObject) for file operations
- String concatenation uses `+` operator
- Paths use backslashes `\` (or forward slashes `/` which Windows accepts)

### Compatibility with config.m4

The design maintains feature parity with the Unix config.m4:
- Same configuration option name (`--with-crc-fast`)
- Same search behavior (custom path or standard locations)
- Same compiler flags and defines
- Same source files compiled
- Same validation checks

### Static Linking Approach

This implementation uses **static linking** with `crc_fast.lib` for the following reasons:
- Eliminates runtime DLL dependencies
- Simplifies deployment (single extension DLL)
- Standard practice for PHP extensions on Windows
- Avoids PATH configuration issues
- No version conflicts with other software using crc_fast

The config.w32 will look for and link against `crc_fast.lib`, embedding all CRC functionality directly in the extension DLL. The static library is approximately 15 MB, which is acceptable given the significant performance benefits the extension provides.

## Design Decisions and Rationales

### Decision 1: Use Standard PHP Windows Build Patterns

**Rationale**: Following established PHP extension patterns ensures compatibility with the build system and makes the configuration familiar to PHP extension developers.

### Decision 2: Use Static Linking

**Rationale**: Static linking is the standard practice for Windows PHP extensions because:
- Eliminates runtime DLL dependencies and PATH issues
- Simplifies deployment (single extension DLL file)
- Prevents version conflicts
- The ~15 MB size increase is acceptable for the performance benefits provided

### Decision 3: Mirror config.m4 Functionality

**Rationale**: Maintaining feature parity with the Unix build ensures consistent behavior across platforms and reduces user confusion.

### Decision 4: Search Standard Windows Locations

**Rationale**: Checking common installation directories (Program Files, etc.) provides a better user experience by auto-detecting the library when possible.

### Decision 5: Fail Fast on Missing Dependencies

**Rationale**: Clear, early error messages help users quickly identify and resolve configuration issues rather than encountering cryptic linker errors later.
