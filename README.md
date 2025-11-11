`awesome/crc_fast`
===========

[![Test status](https://github.com/awesomized/crc-fast-php-ext/workflows/Tests/badge.svg)](https://github.com/awesomized/crc-fast-php-ext/actions?query=workflow%3ATests)
[![Latest Stable Version](https://img.shields.io/packagist/v/awesome/crc-fast)](https://packagist.org/packages/awesome/crc-fast)

Fast, hardware-accelerated CRC calculation in `PHP` for [all known CRC-32 and CRC-64 variants](https://reveng.sourceforge.io/crc-catalogue/all.htm) using `SIMD` 
intrinsics, which can exceed _100GiB/s_ for `CRC-32`, and _50GiB/s_ for `CRC-64`, on modern systems.

It is much, much faster ([up to >200X](#performance)) than the native [crc32](https://www.php.net/manual/en/function.crc32.php), `crc32b`, and `crc32c` 
[implementations](https://www.php.net/manual/en/function.hash-algos.php) in `PHP`, plus adds many more variants 
(particularly `CRC-64/NVME`). 

The performance gains are especially pronounced on `aarch64` (Arm) systems, since PHP doesn't
currently use hardware acceleration there.

## CRC-64/NVME

[CRC-64/NVME](https://reveng.sourceforge.io/crc-catalogue/all.htm#crc.cat.crc-64-nvme) is in use in a variety of
large-scale and mission-critical systems, software, and hardware, such as:
- The `AWS S3` [recommended checksum](https://docs.aws.amazon.com/AmazonS3/latest/userguide/checking-object-integrity.html)
- The [Linux kernel](https://github.com/torvalds/linux/blob/786c8248dbd33a5a7a07f7c6e55a7bfc68d2ca48/lib/crc64.c#L66-L73)
- The [NVMe specification](https://nvmexpress.org/wp-content/uploads/NVM-Express-NVM-Command-Set-Specification-1.0d-2023.12.28-Ratified.pdf)

## CRC-32/PHP, one of PHP's special flowers 🌼

`hash('crc32')` in `PHP` is not the same as `crc32()` in `PHP`, and doesn't match the `crc32` definition or output in 
many other programming languages and implementations (which is typically 
[CRC-32/ISO-HDLC](https://reveng.sourceforge.io/crc-catalogue/all.htm#crc.cat.crc-32-iso-hdlc))

Instead, it's actually [CRC-32/BZIP2](https://reveng.sourceforge.io/crc-catalogue/all.htm#crc.cat.crc-32-bzip2) which 
is then byte-reversed. This extension provides a `CrcFast\CRC_32_PHP` algorithm `const` that performs the same calculation, 
but accelerated.

```php 
$checksum = CrcFast\checksum(
    algorithm: CrcFast\CRC_32_PHP,
    string: '123456789'
); // 181989fc
```

## Requirements
Uses the [crc_fast](https://github.com/awesomized/crc-fast-rust) `Rust` package and its `C`-compatible shared library
under the hood, so you'll need to have built and installed it. (See [Usage](#Usage), below).

## Changes

See the [change log](CHANGELOG.md).

## Installing

Use [Composer](https://getcomposer.org) to install this library using [PIE](https://github.com/php/pie) (note the 
[Requirements](#Requirements) above):

```bash
composer install awesome/crc-fast
```

If you're using a non-standard installation location for the [crc_fast](https://github.com/awesomized/crc-fast-rust) 
library, you may need to specify where the `crc_fast` header (in `include`) and shared library (in `lib`) can be found:

```bash
composer install awesome/crc-fast --with-crc-fast=/path/to/crc-fast
```

## Building

Like most `PHP` extensions, you can also build yourself:

```bash
phpize
./configure
make
```

or with a custom [crc_fast](https://github.com/awesomized/crc-fast-rust) location where the `crc_fast` header and shared
library can be found:

```bash
phpize
./configure --with-crc-fast=/path/to/crc-fast
make
```

## Usage

Examples are for `CRC-64/NVME`, but you can use any [supported algorithm](#get-a-list-of-supported-algorithm-variants) 
variant.

### Calculate CRC-64/NVME checksums:

```php

// calculate the checksum of a string
$checksum = CrcFast\hash(
    algorithm: CrcFast\CRC_64_NVME,
    string: '123456789'
); // ae8b14860a799888

// calculate the checksum of a file, which will chunk through the file optimally,
// limiting RAM usage and maximizing throughput
$checksum = CrcFast\hash_file(
    algorithm: CrcFast\CRC_64_NVME,
    filename: 'path/to/123456789.txt',
); // ae8b14860a799888
```

### Calculate CRC-64/NVME checksums with a Digest for intermittent / streaming / etc workloads:

```php
$crc64Digest = CrcFast\Digest::new(
    algorithm: CrcFast\CRC_64_NVME,
);

// write some data to the digest
$crc64Digest->write('1234');

// write some more data to the digest
$crc64Digest->write('56789');

// calculate the entire digest
$checksum = $crc64Digest->finalize(); // ae8b14860a799888
```

### Get a list of supported algorithm variants
```php
$algorithms = get_supported_algorithms();

var_dump($algorithms);
```

## Equivalents to PHP functions

### crc32()
Calculates `CRC-32/ISO-HDLC` as an integer.

```php 
$checksum = CrcFast\crc32(
    data: '123456789'
); // 3421780262
```

### hash('crc32') 🌼
Calculates [CRC-32/PHP](#crc-32php-one-of-phps-special-flowers-) as binary or hex.

```php 
$checksum = CrcFast\hash(
    algorithm: CrcFast\CRC_32_PHP,
    string: '123456789'
    binary: false,
); // 181989fc
```

### hash('crc32b') 
Calculates `CRC-32/ISO-HDLC` as binary or hex.

```php 
$checksum = CrcFast\hash(
    algorithm: CrcFast\CRC_32_ISO_HDLC,
    string: '123456789'
    binary: false,
); // cbf43926
```

### hash('crc32c')
Calculates `CRC-32/ISCSI` as binary or hex.

```php 
$checksum = CrcFast\hash(
    algorithm: CrcFast\CRC_32_ISCSI,
    string: '123456789'
    binary: false,
); // b798b438
```

## IDE Stubs

This extension comes with IDE [stubs](crc_fast.stub.php) for use with your favorite development environment.

## Tests
See the [tests](tests) directory for test coverage, which also double as useful examples.

```bash
make test
```

## Platform support

This extension has been extensively tested on `macOS` and `Linux`, on both `aarch64` and `x86_64`. 

At [Awesome](https://awesome.co) we use it in production at very large scale on `Linux` on both 
[Flickr](https://flickr.com) and [SmugMug](https://smugmug.com).

This extension is not currently supported on `Windows`. :(

The underlying [crc_fast](https://github.com/awesomized/crc-fast-rust) library (same authors) builds and works on 
`Windows`, so this is likely just a build issue with creating a working `config.w32` implementation. (I took a quick 
stab, failed, and moved on since we don't use `Windows` in production.)

If you want to help, please open a working PR. I'd love to merge it.

## Performance

`PHP` already uses `SIMD` intrinsics for `CRC-32` calculations on `x86_64` but not on `aarch64`. Even on `x86_64`, this
library provides considerable improvements, in addition to supporting many more variants.

Tested using the maximum settings for [crc_fast](https://github.com/awesomized/crc-fast-rust) for each platform, using
1MiB random payloads.

### CRC-32/ISCSI and CRC-32/ISO-HDLC

| Arch    | Brand | CPU             | System               |         PHP |      crc_fast | Speedup |
|:--------|:------|-----------------|----------------------|------------:|--------------:|--------:|
| x86_64  | Intel | Sapphire Rapids | EC2 c7i.metal-48xl   | ~27.0 GiB/s |  ~108.1 GiB/s |     ~4X |
| x86_64  | AMD   | Genoa           | EC2 c7a.metal-48xl   | ~13.6 GiB/s |   ~53.7 GiB/s |     ~4X |
| aarch64 | AWS   | Graviton4       | EC2 c8g.metal-48xl   |  ~0.4 GiB/s |   ~52.3 GiB/s |   ~141X |
| aarch64 | Apple | M3 Ultra        | Mac Studio (32 core) |  ~0.4 GiB/s |   ~99.6 GiB/s |   ~233X |

### CRC-32/PHP 🌼

| Arch    | Brand | CPU             | System               |          PHP |    crc_fast | Speedup |
|:--------|:------|-----------------|----------------------|-------------:|------------:|--------:|
| x86_64  | Intel | Sapphire Rapids | EC2 c7i.metal-48xl   |  ~26.6 GiB/s | ~27.4 GiB/s |     n/a |
| x86_64  | AMD   | Genoa           | EC2 c7a.metal-48xl   |  ~13.6 GiB/s | ~25.4 GiB/s |     ~2X |
| aarch64 | AWS   | Graviton4       | EC2 c8g.metal-48xl   |   ~0.4 GiB/s | ~31.5 GiB/s |    ~73X |
| aarch64 | Apple | M3 Ultra        | Mac Studio (32 core) |   ~0.4 GiB/s | ~57.8 GiB/s |   ~134X |

### CRC-64/NVME

Note that PHP has no native equivalent.

| Arch    | Brand | CPU             | System               |    crc_fast | 
|:--------|:------|-----------------|----------------------|------------:|
| x86_64  | Intel | Sapphire Rapids | EC2 c7i.metal-48xl   | ~54.6 GiB/s | 
| x86_64  | AMD   | Genoa           | EC2 c7a.metal-48xl   | ~27.0 GiB/s | 
| aarch64 | AWS   | Graviton4       | EC2 c8g.metal-48xl   | ~37.0 GiB/s |
| aarch64 | Apple | M3 Ultra        | Mac Studio (32 core) | ~70.0 GiB/s | 

## License

`cfc-fast` is dual-licensed under

* Apache 2.0 license ([LICENSE-Apache](./LICENSE-Apache) or <http://www.apache.org/licenses/LICENSE-2.0>)
* MIT license ([LICENSE-MIT](./LICENSE-MIT) or <https://opensource.org/licenses/MIT>)

