# Teeny SHA-1

A small, single function implementation of the
[SHA-1](https://en.wikipedia.org/wiki/SHA-1) in straightforward C
(C99) that generates a digest in binary and/or hexadecimal.  This
code is based on the SHA-1 implementation in
[EncryptionLibrary](https://github.com/jinqiangshou/EncryptionLibrary)
by [@jinqiangshou](https://github.com/jinqiangshou).

If you need a SHA-1 calculation with no external dependencies to embed
in your project this may be for you.

This implementation does not provide an incremental update interface,
the entire buffer to be hashed must be in memory in a single buffer.
This is also approximately an order of magnitude slower than solutions
like [OpenSSL](https://www.openssl.org/).  If you are doing many, many
hash calculations on very large buffers and/or need to do chunked
calculations you may wish to look elsewhere.

## Usage pattern

The expectation is that you'll copy the function into an existing
project source code file or add the teeny-sha1.c file to your project
and then add a declaration for the function wherever it's needed.

#### The sha1digest() documentation
```
/*******************************************************************************
 * sha1digest: https://github.com/CTrabant/teeny-sha1
 *
 * Calculate the SHA-1 value for supplied data buffer and generate a
 * text representation in hexadecimal.
 *
 * Based on https://github.com/jinqiangshou/EncryptionLibrary, credit
 * goes to @jinqiangshou, all new bugs are mine.
 *
 * @input:
 *    data      -- data to be hashed
 *    databytes -- bytes in data buffer to be hashed
 *
 * @output:
 *    digest    -- the result, MUST be at least 20 bytes
 *    hexdigest -- the result in hex, MUST be at least 41 bytes
 *
 * At least one of the output buffers must be supplied.  The other, if not 
 * desired, may be set to NULL.
 *
 * @return: 0 on success and non-zero on error.
 ******************************************************************************/
int
sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes)
```

## MIT License

See the included LICENSE details for more details.  The original was
MIT licensed and so is this in kind.

## Teeny?

I was looking for a small implementation of SHA-1 while trying to
avoid dependencies like OpenSSL.  In my case I was writing a WebSocket
implementation in a project not already dependent on OpenSSL or other
libraries providing SHA-1 and needed to hash 30-50 bytes of data.  There
are many "tiny" implementations around, but I found them not-so-tiny
and nearly always requiring multiple functions.  This function
provides a SHA-1 in ~150 lines of commented C99 in a single function,
no fuss.  Not exactly teeny but compact, encapsulated and easy to use.
