# Essential Commands for CRC Fast PHP Extension

## Critical Commands - NEVER FORGET THESE

### Testing
- **ALWAYS** run tests with: `NO_INTERACTION=1 make test`
- **NEVER** use `php run-tests.php` directly
- **NEVER** use `make test` without `NO_INTERACTION=1`

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