# Requirements Document

## Introduction

This feature extends the existing PHP CRC extension to support custom CRC parameters, allowing users to define their own CRC algorithms beyond the predefined set. The underlying crc_fast library has been updated with custom parameter support, and we need to expose this functionality through the PHP extension while maintaining full backward compatibility with existing APIs.

## Requirements

### Requirement 1

**User Story:** As a PHP developer, I want to create custom CRC algorithms with specific parameters, so that I can calculate checksums for proprietary or specialized CRC variants not included in the predefined algorithms.

#### Acceptance Criteria

1. WHEN a user provides custom CRC parameters (width, polynomial, init, refin, refout, xorout, check) THEN the system SHALL create a custom CRC algorithm instance
2. WHEN custom parameters are invalid (e.g., unsupported width) THEN the system SHALL throw a descriptive exception
3. WHEN custom parameters are valid THEN the system SHALL allow all existing operations (hash, hash_file, combine, Digest class) to work with the custom algorithm
4. IF custom parameters match a predefined algorithm THEN the system SHALL still function correctly without optimization requirements

### Requirement 2

**User Story:** As a PHP developer, I want to use a helper function to create custom CRC parameters from Rocksoft model parameters, so that I can easily convert standard CRC specifications into the required parameter format.

#### Acceptance Criteria

1. WHEN a user provides Rocksoft model parameters (name, width, poly, init, reflected, xorout, check) THEN the system SHALL return a custom CRC parameter object
2. WHEN Rocksoft parameters are invalid THEN the system SHALL throw a descriptive exception
3. WHEN the helper function is called THEN the system SHALL validate all parameters before creating the custom parameter object

### Requirement 3

**User Story:** As a PHP developer, I want all existing CRC functions to accept custom parameter objects, so that I can use custom algorithms seamlessly with the current API.

#### Acceptance Criteria

1. WHEN CrcFast\hash() receives a custom parameter object instead of an algorithm constant THEN the system SHALL calculate the checksum using the custom algorithm
2. WHEN CrcFast\hash_file() receives a custom parameter object THEN the system SHALL calculate the file checksum using the custom algorithm
3. WHEN CrcFast\combine() receives a custom parameter object THEN the system SHALL combine checksums using the custom algorithm
4. WHEN the Digest class constructor receives a custom parameter object THEN the system SHALL create a digest instance for the custom algorithm

### Requirement 4

**User Story:** As a PHP developer, I want the extension to maintain backward compatibility, so that my existing code continues to work without modifications after the update.

#### Acceptance Criteria

1. WHEN existing code uses predefined algorithm constants THEN the system SHALL continue to work exactly as before
2. WHEN existing function signatures are used THEN the system SHALL maintain the same behavior and return types
3. WHEN existing class methods are called THEN the system SHALL maintain the same behavior and return types
4. IF new functionality is added THEN the system SHALL not break any existing functionality

### Requirement 5

**User Story:** As a PHP developer, I want proper error handling for custom parameters, so that I can debug issues with my custom CRC configurations.

#### Acceptance Criteria

1. WHEN invalid custom parameters are provided THEN the system SHALL throw specific exceptions with clear error messages
2. WHEN custom parameter validation fails THEN the system SHALL indicate which parameter is invalid and why
3. WHEN custom algorithms fail during computation THEN the system SHALL provide meaningful error information
4. WHEN memory allocation fails for custom algorithms THEN the system SHALL handle the error gracefully

### Requirement 6

**User Story:** As a PHP developer, I want the stub file to remain the source of truth for the API, so that the extension maintains consistency with PHP's standard practices.

#### Acceptance Criteria

1. WHEN API changes are made THEN the system SHALL update the stub file first
2. WHEN the arginfo file needs updates THEN the system SHALL regenerate it from the stub file
3. WHEN new functions or classes are added THEN the system SHALL define them in the stub file
4. WHEN documentation is updated THEN the system SHALL update it in the stub file