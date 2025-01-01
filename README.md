# usg
Tiny program for generating Ulam Spiral and writing it as simple image file. \
Currently supporting uncompressed .bmp and .ppm files. \
This project has no external dependencies (besides libC itself) \
and can be built with any C99-compliant compiler.

![spiral](https://github.com/user-attachments/assets/5c51f550-e331-4d55-a561-6ba73e688214)


# BUILDING
```sh
$ git clone https://github.com/sleeptightAnsiC/usg.git
$ cd usg
$ cc src/*.c -o usg
$ ./usg --help
```

# HELP
```txt
Usage:
  usg [option1] [option2] [optionN]
Options:
  --width NUM     Width of the image in pixels (default: 201)
  --height NUM    Height of the image in pixels (default: 201)
  -o --out FILE   Name of output image file (default: 'a.bmp')
  --type TYPE     Type of output image file (extension)
                  Accepts either: 'ppm' or 'bmp'
                  If unset, determined automatically based on file name
  --fg COLOR      Foreground color of the image (default: '000000ff')
  --bg COLOR      Background color of the image (default: 'ffffffff')
                  COLOR must be in HEX format represented by exactly eight
                  hexidecimal symbols without any prefix (regex: [0-9a-fA-F])
                  Alpha channel is discarded for image types not supporting it
  --start-x NUM   X coordinate where the spiral starts (default: width/2)
  --start-y NUM   Y coordinate where the spiral starts (default: height/2)
                  NUM must be an OpenGL-style "screen coordinate",
                  meaning that 0:0 is in the top left corner of the image
  --start-val NUM  Value that spiral uses at its starting point (default: 1)
  --no-path       Do NOT print the image path after creation
  --stdout        Write image to standard output instead of file
  --faded         Fade between background and forground colors
                  Fade is calculated based on the value on spiral and
                  the max possible value that can occur on spiral (val/max)
                  Use in order to see how values are being distributed
  -h --help       Prints this help message and exits
Example:
  usg --out spiral.bmp --width 1024 --height 1024 --fg 00ff00ff --bg 000000ff
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
Code under this repository is licensed under permissive MIT license
