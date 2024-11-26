# usg
Tiny program for generating Ulam Spiral and writing it as simple image file. \
Currently supporting uncompressed .bmp and .ppm files. \
This project has no external dependency (besides libC itself).

![spiral](https://github.com/user-attachments/assets/5c51f550-e331-4d55-a561-6ba73e688214)


# BUILDING
EITHER, with GNU/make on POSIX (Linux, MacOSX, MinGW, etc):
```sh
$ git clone https://github.com/sleeptightAnsiC/usg.git
$ cd usg
$ make
```
OR, with any C99-capable compiler:
```sh
$ cc src/*.c
```

# REFERENCES
- https://en.wikipedia.org/wiki/Prime_number
- https://en.wikipedia.org/wiki/List_of_prime_numbers
- https://en.wikipedia.org/wiki/Generation_of_primes
- https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
- https://en.wikipedia.org/wiki/BMP_file_format
- https://en.wikipedia.org/wiki/Netpbm

# LICENSE
Copyright holder: https://github.com/sleeptightAnsiC \
Code under this repository is licensed under permissive MIT/X license
