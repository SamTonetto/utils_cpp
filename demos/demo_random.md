# Demo - Random

Although the STL has built-in distributions, it does not specify how these distributions are implemented. Therefore, running `std::uniform_int_distribution`, for example, on two different platform can yield two different results, even for the same random seed (I have experienced this first-hand: my local clang 13.1.6 compiler produces different results compared to whatever Github Actions uses).

Just define a mersenne twistter random generator (this is safely portable),

```
long seed = 0;
std::mt19937 gen(seed);

// or:
//
// std::mt19937 gen;
// gen.seed(seed);
```

Then
```
int r1 = random_int(gen);                // generates 0 or 1 with equal probability (equivalent to `random_int(gen, 0, 2)`
int r2 = random_int(gen, min, max)       // generates random int in [min, max).
double r3 = random_real(gen);            // generates random double in [0,1) inclusive.
double r4 = random_double(gen, min, max) // generates random double in [min, max).
```
