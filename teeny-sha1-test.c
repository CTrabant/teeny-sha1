/******************************************************************************
 * A test program for the SHA-1 function in teeny-sha1.c
 *
 * The tests are conducted by calculating the SHA-1 values using
 * sha1digest() and comparing the result to the known digest value.
 *
 * Included are some data values and known hashes from
 * http://www.di-mgt.com.au/sha_testvectors.html, a few examples from
 * https://en.wikipedia.org/wiki/SHA-1 and the ability to use test
 * vectors from NIST's National Software Reference Library (NSRL).
 *
 * Command line options:
 *   -l : Perform a large (1GB) test
 *   -nsrl <directory>
 *        Perform tests with vectors provided in:
 *        http://www.nsrl.nist.gov/testdata/NSRLvectors.zip
 *        The test vectors must be downloaded and unzipped manually.
 *
 * Build test program:
 *   cc -g -O2 -Wall teeny-sha1.c teeny-sha1-test.c -o teeny-sha1-test
 ******************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

/* Declare sha1digest and use directly from teeny-sha1.c */
extern int sha1digest(uint8_t *digest, char *hexdigest, const uint8_t *data, size_t databytes);

/* Generate hexadecimal version of a 20-byte digest value */
void
generatehex (char *hexdigest, uint8_t *digest)
{
  int idx;

  for (idx = 0; idx < 20; idx++)
  {
    sprintf (hexdigest + (idx * 2), "%02x", digest[idx]);
  }
  hexdigest[40] = '\0';
}

/* Run hash function and print comparison results.
 * Return positive mismatch count with known digest, 0 when all match. */
int
testhash (char *data, size_t databytes, char *knowndigest)
{
  uint8_t digest[20];
  char hexdigest[41];
  char binhexdigest[41];
  char *hexstatus;
  char *binstatus;
  int mismatch = 0;

  /* Generate digest, both binary and hex versions */
  if (sha1digest (digest, hexdigest, (uint8_t *)data, databytes))
  {
    fprintf (stderr, "Error with sha1digest()\n");
    return 1;
  }

  /* Generate hex version from binary digest */
  generatehex (binhexdigest, digest);

  /* Compare hex versions to know digest in hex */
  if (strcasecmp (hexdigest, knowndigest))
  {
    hexstatus = "does NOT match";
    mismatch++;
  }
  else
  {
    hexstatus = "matches";
  }
  if (strcasecmp (binhexdigest, knowndigest))
  {
    binstatus = "does NOT match";
    mismatch++;
  }
  else
  {
    binstatus = "matches";
  }

  printf ("Known digest:  '%s'  data length: %zu\n", knowndigest, databytes);
  printf ("  Hex digest:  '%s'  %s\n", hexdigest, hexstatus);
  printf ("  Bin digest:  '%s'  %s\n", binhexdigest, binstatus);
  printf ("\n");

  return mismatch;
}

int
main (int argc, char **argv)
{
  char *data;
  char *knowndigest;
  char *nsrldir = NULL;
  int failurecount = 0;
  int largetest = 0;
  int idx;

  /* Process command line options */
  for (idx = 0; idx < argc; idx++)
  {
    /* A "-l" option turns on the large test */
    if (!strcmp (argv[idx], "-l"))
      largetest = 1;

    /* A "-nsrl <directory>" option does testing with the NSRL Sample Vectors subset */
    if (!strcmp (argv[idx], "-nsrl"))
    {
      if ((idx + 1) == argc)
      {
        printf ("-nsrl option requires a directory argument\n");
        return 1;
      }
      else
        nsrldir = argv[idx + 1];
    }
  }

  /* Test vectors from http://www.di-mgt.com.au/sha_testvectors.html */
  data = "abc";
  knowndigest = "a9993e364706816aba3e25717850c26c9cd0d89d";
  failurecount += testhash (data, strlen(data), knowndigest);

  data = "";
  knowndigest = "da39a3ee5e6b4b0d3255bfef95601890afd80709";
  failurecount += testhash (data, strlen(data), knowndigest);

  data = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
  knowndigest = "84983e441c3bd26ebaae4aa1f95129e5e54670f1";
  failurecount += testhash (data, strlen(data), knowndigest);

  data = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
  knowndigest = "a49b2446a02c645bf419f995b67091253a04a259";
  failurecount += testhash (data, strlen(data), knowndigest);

  /* One million repetitions of 'a' */
  data = (char *)malloc (1000000);
  memset (data, 'a', 1000000);
  knowndigest = "34aa973cd4c4daa4f61eeb2bdbad27316534016f";
  failurecount += testhash (data, 1000000, knowndigest);
  free (data);

  /* Large test (~1GB), 16,777,216 repetitions of base */
  if (largetest)
  {
    char *base = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmno";
    size_t baselen = strlen(base);
    data = (char *) malloc (16777216 * baselen);
    for (idx = 0; idx < 16777216; idx++)
    {
      memcpy (data + (idx * baselen), base, baselen);
    }

    knowndigest = "7789f0c9ef7bfc40d93311143dfbe69e2017f592";
    failurecount += testhash (data, baselen * 16777216, knowndigest);
    free(data);
  }

  /* Example hashes from Wikipedia */
  data = "The quick brown fox jumps over the lazy dog";
  knowndigest = "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12";
  failurecount += testhash (data, strlen(data), knowndigest);

  data = "The quick brown fox jumps over the lazy cog";
  knowndigest = "de9f2c7fd25e1b3afad3e85a0bd17d9b100db4b3";
  failurecount += testhash (data, strlen(data), knowndigest);

  /* Read test vectors from the NIST NSRL Sample Vectors subset:
     http://www.nsrl.nist.gov/testdata/
     http://www.nsrl.nist.gov/testdata/NSRLvectors.zip */
  if (nsrldir)
  {
    FILE *infile;
    struct stat sb;
    char *nsrlhashes[200] = {0};
    char path[100];
    char line[100];
    int hashcount = 0;
    int idx;

    /* Open byte-hashes.sha1 and store known hash values (in hex) */
    snprintf (path, sizeof(path), "%s/byte-hashes.sha1", nsrldir);
    if (!(infile = fopen(path, "r")))
    {
      printf ("Error opening %s: %s\n", path, strerror(errno));
      return 1;
    }
    while (fgets(line, sizeof(line), infile))
    {
      /* Store known hash values, e.g. "DA39A3EE5E6B4B0D3255BFEF95601890AFD80709 ^" */
      if (strlen(line) >= 42 && line[40] == ' ' && line[41] == '^')
      {
        line[40] = '\0';
        nsrlhashes[hashcount] = strdup(line);
        hashcount++;
      }
    }
    fclose (infile);

    fprintf (stderr, "hash count: %d\n", hashcount);

    /* Open byte####.dat file that match the order of the hashes */
    for (idx = 0; idx < hashcount; idx++)
    {
      snprintf (path, sizeof(path), "%s/byte%04d.dat", nsrldir, idx);
      if (!(infile = fopen(path, "r")))
      {
        printf ("Error opening %s: %s\n", path, strerror(errno));
        return 1;
      }

      /* Read file contents into buffer */
      fstat (fileno(infile), &sb);
      data = (char *) malloc ((size_t)sb.st_size);
      if (sb.st_size > 0 && fread (data, (size_t)sb.st_size, 1, infile) != 1)
      {
        if (ferror(infile))
          printf ("Error reading %s: %s\n", path, strerror(errno));
        else
          printf ("Short read of %s, expected %zu bytes\n", path, (size_t)sb.st_size);

        return 1;
      }
      fclose (infile);

      printf ("File: %s\n", path);
      failurecount += testhash (data, (size_t)sb.st_size, nsrlhashes[idx]);

      free (data);
    }
  } /* Done with NSRL test vectors */


  printf ("Failures: %d\n", failurecount);

  return failurecount;
}
