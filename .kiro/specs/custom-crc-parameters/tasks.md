# Implementation Plan

- [ ] 1. Update stub file with new CrcFast\Params class and function signatures
  - Add CrcFast\Params class definition with constructor and getter methods
  - Update existing function signatures to accept int|CrcFast\Params for algorithm parameter
  - _Requirements: 1.1, 3.1, 6.1, 6.3_

- [ ] 2. Regenerate arginfo file from updated stub
  - Run PHP's gen_stub.php to regenerate crc_fast_arginfo.h from the updated stub file
  - Verify that new function signatures and class definitions are properly generated
  - _Requirements: 6.2, 6.4_

- [ ] 3. Implement CrcFast\Params class in C extension
  - [ ] 3.1 Define php_crc_fast_params_obj structure and object handlers
    - Create structure containing CrcFastParams and zend_object
    - Implement create_object, free_obj, and other standard object handlers
    - _Requirements: 1.1, 5.1_

  - [ ] 3.2 Implement CrcFast\Params constructor
    - Parse constructor parameters (width, poly, init, refin, refout, xorout, check, keys)
    - Validate parameter values according to design constraints
    - Create and populate CrcFastParams struct
    - Handle optional keys parameter (generate if null, validate if provided)
    - _Requirements: 1.1, 1.2, 2.2, 5.2_

  - [ ] 3.3 Implement CrcFast\Params getter methods
    - Implement getWidth(), getPoly(), getInit(), getRefin(), getRefout(), getXorout(), getCheck()
    - Implement getKeys() method that always returns the computed keys array
    - _Requirements: 1.1_



- [ ] 4. Update existing functions to support custom parameters
  - [ ] 4.1 Create parameter detection helper function
    - Add helper to determine if parameter is int (algorithm constant) or CrcFast\Params object
    - Extract CrcFastParams struct from Params object when needed
    - _Requirements: 3.1, 3.2, 3.3_

  - [ ] 4.2 Update CrcFast\hash() function
    - Modify function to accept int|CrcFast\Params for algorithm parameter
    - Use crc_fast_checksum() for predefined algorithms
    - Use crc_fast_checksum_with_params() for custom parameters
    - Maintain existing behavior for all predefined algorithms
    - _Requirements: 3.1, 4.1, 4.2_

  - [ ] 4.3 Update CrcFast\hash_file() function
    - Modify function to accept int|CrcFast\Params for algorithm parameter
    - Use crc_fast_checksum_file() for predefined algorithms
    - Use crc_fast_checksum_file_with_params() for custom parameters
    - Maintain existing behavior for all predefined algorithms
    - _Requirements: 3.2, 4.1, 4.2_

  - [ ] 4.4 Update CrcFast\combine() function
    - Modify function to accept int|CrcFast\Params for algorithm parameter
    - Use crc_fast_checksum_combine() for predefined algorithms
    - Use crc_fast_checksum_combine_with_custom_params() for custom parameters
    - Maintain existing checksum parsing logic for both binary and hex inputs
    - _Requirements: 3.3, 4.1, 4.2_

- [ ] 5. Update CrcFast\Digest class to support custom parameters
  - [ ] 5.1 Modify Digest constructor
    - Update constructor to accept int|CrcFast\Params for algorithm parameter
    - Use crc_fast_digest_new() for predefined algorithms
    - Use crc_fast_digest_new_with_params() for custom parameters
    - Store parameter type information for later use in formatting
    - _Requirements: 3.4, 4.1, 4.2_

  - [ ] 5.2 Update result formatting for custom parameters
    - Modify php_crc_fast_format_result() to handle custom parameters
    - Determine output width (32 or 64 bit) from custom parameters
    - Ensure binary and hex output formats work correctly for custom algorithms
    - _Requirements: 1.3, 4.3_

- [ ] 6. Add comprehensive error handling
  - Add parameter validation with descriptive error messages
  - Handle C library errors gracefully with appropriate PHP exceptions
  - Validate keys array length and values when provided
  - _Requirements: 5.1, 5.2, 5.3, 5.4_

- [ ] 7. Create unit tests for CrcFast\Params class
  - Test constructor with valid parameters
  - Test constructor with invalid parameters (wrong width, out-of-range values)
  - Test getter methods return correct values
  - Test keys parameter handling (both provided and auto-generated)
  - _Requirements: 1.1, 1.2, 5.1, 5.2_



- [ ] 8. Create integration tests for custom parameters with existing functions
  - [ ] 8.1 Test CrcFast\hash() with custom parameters
    - Create custom parameters that match existing predefined algorithms
    - Verify identical results between custom and predefined algorithms
    - Test both 32-bit and 64-bit custom algorithms
    - _Requirements: 3.1, 4.1_

  - [ ] 8.2 Test CrcFast\hash_file() with custom parameters
    - Test file hashing with custom parameters
    - Verify results match hash() function for same data
    - Test both binary and hex output formats
    - _Requirements: 3.2, 4.1_

  - [ ] 8.3 Test CrcFast\combine() with custom parameters
    - Test checksum combination with custom parameters
    - Verify results match manual calculation
    - Test with both binary and hex checksum inputs
    - _Requirements: 3.3, 4.1_

  - [ ] 8.4 Test CrcFast\Digest class with custom parameters
    - Test digest creation, update, and finalization with custom parameters
    - Test digest combination with custom parameters
    - Verify results match direct hash() function calls
    - _Requirements: 3.4, 4.1_

- [ ] 9. Create backward compatibility tests
  - Run all existing tests to ensure no regressions
  - Verify all existing function signatures still work
  - Verify all existing constants and behavior unchanged
  - _Requirements: 4.1, 4.2, 4.3_

- [ ] 10. Build and test extension
  - Compile extension with updated code
  - Run complete test suite to verify all functionality
  - Test extension loading and basic functionality
  - _Requirements: All requirements verification_