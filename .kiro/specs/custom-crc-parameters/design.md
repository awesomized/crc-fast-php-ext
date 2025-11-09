# Design Document

## Overview

This design extends the existing PHP CRC extension to support custom CRC parameters while maintaining full backward compatibility. The underlying crc_fast library has been updated with custom parameter support through the `CrcFastParams` struct and related functions. We will expose this functionality through a new `CrcFast\Params` class and extend existing functions to accept either algorithm constants or custom parameter objects.

## Architecture

### Core Components

1. **CrcFast\Params Class**: A new PHP class that wraps the C `CrcFastParams` struct
2. **Extended Function Signatures**: Modify existing functions to accept both `int` (algorithm constants) and `CrcFast\Params` objects
3. **Helper Functions**: Add utility functions for creating custom parameters from Rocksoft model parameters
4. **Backward Compatibility Layer**: Ensure all existing code continues to work without modification

### Design Principles

- **Backward Compatibility**: All existing APIs must continue to work exactly as before
- **Type Safety**: Use PHP's type system to validate parameters at runtime
- **Performance**: Custom parameters should not impact performance of predefined algorithms
- **Consistency**: Custom parameters should work seamlessly with all existing functions

## Components and Interfaces

### CrcFast\Params Class

```php
namespace CrcFast {
    class Params {
        public function __construct(
            int $width,
            int $poly,
            int $init,
            bool $refin,
            bool $refout,
            int $xorout,
            int $check,
            ?array $keys = null  // Optional array of 23 pre-computed keys for performance
        );
        
        public function getWidth(): int;
        public function getPoly(): int;
        public function getInit(): int;
        public function getRefin(): bool;
        public function getRefout(): bool;
        public function getXorout(): int;
        public function getCheck(): int;
        public function getKeys(): array;  // Returns the keys (generated if not provided in constructor)
    }
}
```

### Direct Parameter Construction

Users can directly create custom CRC parameters using the `CrcFast\Params` constructor. The constructor accepts the standard Rocksoft model parameters and automatically handles the conversion internally, including generating the required keys.

### Extended Function Signatures

All existing functions will be modified to accept `int|CrcFast\Params` for the algorithm parameter:

```php
namespace CrcFast {
    function hash(int|Params $algorithm, string $data, bool $binary = false): string;
    function hash_file(int|Params $algorithm, string $filename, bool $binary = false): string;
    function combine(int|Params $algorithm, string $checksum1, string $checksum2, int $length2, bool $binary = false): string;
    
    class Digest {
        public function __construct(int|Params $algorithm);
        // ... other methods remain unchanged
    }
}
```

## Data Models

### PHP CrcFast\Params Object Structure

```c
typedef struct _php_crc_fast_params_obj {
    CrcFastParams params;  // C struct from libcrc_fast.h
    zend_object std;
} php_crc_fast_params_obj;
```

### Parameter Validation Rules

- **Width**: Must be 32 or 64 bits (the only widths supported by the crc_fast library)
- **Polynomial**: Must fit within the specified width
- **Init/Xorout**: Must fit within the specified width
- **Check**: Used for validation - computed checksum of "123456789" must match this value
- **Keys**: If provided, must be an array of exactly 23 integer values. If null, keys will be computed on-demand by the C library

## Error Handling

### Parameter Validation Errors

- **InvalidArgumentException**: For invalid parameter values (width, polynomial out of range)
- **ValueError**: For parameters that don't produce the expected check value
- **TypeError**: For incorrect parameter types in function calls

### Runtime Errors

- **RuntimeException**: For memory allocation failures or C library errors
- **Exception**: For general digest operation failures

### Error Messages

All error messages will be descriptive and indicate:
- Which parameter is invalid
- What the valid range/values are
- How to correct the issue

## Testing Strategy

### Unit Tests

1. **Parameter Validation Tests**
   - Valid parameter combinations
   - Invalid parameter combinations
   - Edge cases (min/max values)

2. **Functionality Tests**
   - Custom parameters with hash() function
   - Custom parameters with hash_file() function
   - Custom parameters with combine() function
   - Custom parameters with Digest class

3. **Backward Compatibility Tests**
   - All existing tests must continue to pass
   - Existing code patterns must work unchanged

4. **Integration Tests**
   - Custom parameters producing known checksums
   - Rocksoft model parameter conversion
   - Cross-validation with reference implementations

### Test Data

- Use well-known CRC algorithms as test cases (e.g., CRC-32/ISCSI, CRC-32/ISO-HDLC, CRC-64/NVME)
- Include edge cases for both width=32 and width=64
- Test both reflected and non-reflected algorithms
- Validate custom parameters by recreating existing predefined algorithms and comparing results

## Implementation Details

### C Extension Changes

1. **New Object Type**: Add `php_crc_fast_params_obj` structure and handlers
2. **Parameter Detection**: Add helper functions to detect if parameter is int or Params object
3. **C Struct Conversion**: Convert PHP Params object to C `CrcFastParams` struct
4. **Function Overloading**: Modify existing functions to handle both parameter types

### Memory Management

- **Params Objects**: Standard PHP object lifecycle management
- **C Structs**: Stack-allocated `CrcFastParams` structs for function calls
- **Digest Objects**: Extended to store either algorithm constant or custom parameters

### Performance Considerations

- **Fast Path**: Predefined algorithms use existing fast path with `crc_fast_digest_new()`
- **Custom Path**: Custom parameters use `crc_fast_digest_new_with_params()` and related functions
- **Validation**: Validate parameters once during PHP object creation
- **Key Pre-computation**: When keys are provided in constructor, they are used directly for optimal performance. When keys are null, they are computed once during object creation and stored in the C struct
- **Key Reuse**: The C library handles key management internally - once computed, keys are reused for all operations on that digest

## Backward Compatibility Guarantees

### API Compatibility

- All existing function signatures remain valid
- All existing constants remain unchanged
- All existing behavior is preserved exactly

### Binary Compatibility

- Extension version will be incremented appropriately
- No breaking changes to existing compiled code
- Graceful handling of version mismatches

### Migration Path

- Existing code requires no changes
- New functionality is opt-in only
- Clear documentation for adopting custom parameters

## Security Considerations

### Input Validation

- All custom parameters are validated before use
- Polynomial values are checked for validity
- Width constraints are enforced strictly

### Memory Safety

- All C struct operations are bounds-checked
- PHP object lifecycle prevents use-after-free
- Error handling prevents undefined behavior

### Resource Limits

- Custom parameter objects have reasonable memory footprint
- No unbounded resource allocation
- Proper cleanup on errors