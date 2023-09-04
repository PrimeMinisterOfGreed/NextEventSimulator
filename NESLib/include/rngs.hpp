/* -----------------------------------------------------------------------
 * Name            : rngs.h  (header file for the library file rngs.c)
 * Author          : Steve Park & Dave Geyer
 * Language        : ANSI C
 * Latest Revision : 09-22-98
 * -----------------------------------------------------------------------
 */
#pragma once

#define MODULUS 2147483647 /* DON'T CHANGE THIS VALUE                  */
#define MULTIPLIER 48271   /* DON'T CHANGE THIS VALUE                  */
#define CHECK 399268537    /* DON'T CHANGE THIS VALUE                  */
#define STREAMS 256        /* # of streams, DON'T CHANGE THIS VALUE    */
#define A256 22925         /* jump multiplier, DON'T CHANGE THIS VALUE */
#define DEFAULT 123456789  /* initial seed, use 0 < DEFAULT < MODULUS  */

class RandomStream
{
    long seed[STREAMS] = {DEFAULT}; /* current state of each stream   */
    int stream = 0;                 /* stream index, 0 is the default */
    int initialized = 0;            /* test for stream initialization */

public:
    RandomStream();
    double Random(void);
    void PlantSeeds(long x);
    void GetSeed(long *x);
    void PutSeed(long x);
    void SelectStream(int index);
    void TestRandom(void);
    static RandomStream& Global();
};


