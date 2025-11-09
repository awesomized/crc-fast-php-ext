# Build Environment

IMPORTANT: Building PHP extensions happens inside of a custom `php-sdk-binary-tools` environment which is invoked by executing `phpsdk-vs17-x64.bat` in a Windows CMD prompt from the extension's directory.

Example:
```
C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\pecl\crc-fast-php-ext>..\..\..\..\..\phpsdk-vs17-x64.bat
[vcvarsall.bat] Environment initialized for: 'x64'

PHP SDK 2.4.0-dev

OS architecture:    x64
Build architecture: x64
Visual C++:         14.44.35215.0
PHP-SDK path:       C:\Users\onethumb\git\php-sdk-binary-tools

C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\pecl\crc-fast-php-ext
$
```

Build commands *CANNOT* work unless they're within this environment. If you find yourself outside of the environment for some reason, or the remaining build commands don't seem to be working properly, double-check that you're in the `php-sdk-binar-tools` environment, and if not, start it again.

# Build Commands

## Changing to the PHP source directory

First, you must change to the PHP source directory while in the `php-sdk-binary-tools` build environment:

```
C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\pecl\crc-fast-php-ext
$ cd ../../php-8.5-src

C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$
```

## Buildconf

Next, you must run `buildconf` in the PHP source directory:
```
C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$ buildconf
Rebuilding configure.js
Now run 'configure --help'

C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$
```

## Configure

Next, you must run `configure` to disable all other extensions, but enable `crc-fast`, as well as the `cli` SAPI:

```
C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$ configure --disable-all --enable-cli --enable-crc-fast
PHP Version: 8.5.0-dev

Saving configure options to config.nice.bat

...
```

## Build

Finally, you must run `nmake` to build PHP with the loaded extension:

```
C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$ nmake

C:\Users\onethumb\git\php-sdk-binary-tools\phpdev\vs17\x64\php-8.5-src
$ nmake

Microsoft (R) Program Maintenance Utility Version 14.44.35215.0
Copyright (C) Microsoft Corporation.  All rights reserved.

Recreating build dirs

...
```