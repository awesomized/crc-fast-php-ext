# Essential Commands for CRC Fast PHP Extension

## Critical Commands - NEVER FORGET THESE

### Testing
- **ALWAYS** run tests with: `NO_INTERACTION=1 make test`
- **NEVER** use `php run-tests.php` directly
- **NEVER** use `make test` without `NO_INTERACTION=1`
- **ALWAYS** write tests in the `tests/` directory using `.phpt` format
- **ALWAYS** follow the existing test architecture and naming conventions
- **NEVER** create tests outside the `tests/` directory

### Code Generation
- **ALWAYS** regenerate arginfo after modifying stub files: `php build/gen_stub.php crc_fast.stub.php`
- This must be done whenever `crc_fast.stub.php` is modified

### Build Process
1. `make clean` - Clean previous builds
2. `make` - Compile the extension
3. `NO_INTERACTION=1 make test` - Run tests

## Why These Commands Matter
- `NO_INTERACTION=1 make test` prevents interactive prompts and ensures tests run completely
- `php build/gen_stub.php` generates the required arginfo headers from stub files
- Using the wrong test command leads to incomplete or incorrect test results

## Before Any Code Changes
1. Always run `NO_INTERACTION=1 make test` to ensure existing functionality works
2. After making changes, rebuild and test again
3. Never assume tests pass without running them properly

## Test Writing Guidelines
- All tests must be written as `.phpt` files in the `tests/` directory
- Follow the existing test structure: `--TEST--`, `--FILE--`, `--EXPECT--` sections
- Test file names should be descriptive and follow existing patterns
- Always test both success and error cases
- Include tests for parameter validation and error handling
- Run tests after writing them to ensure they work correctly

## Test Debugging
- **NEVER** run PHP directly to test - always use `NO_INTERACTION=1 make test`
- When tests fail, check the generated output files for debugging:
  - `tests/[testname].out` - Contains the actual output from the test
  - `tests/[testname].log` - Contains detailed test execution log
  - `tests/[testname].diff` - Shows the difference between expected and actual output
  - `tests/[testname].exp` - Contains the expected output
- These files are automatically generated when tests run and are essential for debugging test failures
- Always read the `.out` file first to see what the test actually produced