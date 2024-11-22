#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "./soe.h"
#include "./spr.h"
#include "./img.h"
#include "./typ.h"
#include "./dbg.h"

static void
main_help(void)
{
	//   "-------------------------------------------------------------------------------"
	puts("Ulam Spiral Generator <https://github.com/sleeptightAnsiC/usg>");
	puts("Synopsis:");
	puts("  usg [--size <NUM>] [--type <ppm|bmp>] [--out <FILE>] [--fg <RRGGBBAA>]");
	puts("      [--bg <RRGGBBAA>] [-h] [--help]");
	puts("Options:");
	puts("  --size <NUM>   Size of the created image in pixels (default: 128)");
	puts("                 For example, when set to 10, the resolution will be 10x10");
	puts("                 and program will count prime numbers up to value 100");
	puts("  --type <TYPE>  Type of output image file. Accepts either 'ppm' or 'bmp'");
	puts("                 If unset, determined automatically based on file name");
	puts("  --out <FILE>   Name of output image file (default: 'a.bmp')");
	puts("  --fg <COLOR>   Foreground color of the image (default: '000000ff')");
	puts("  --bg <COLOR>   Background color of the image (default: 'ffffffff')");
	puts("                 COLOR must be in HEX format represented by exactly eight");
	puts("                 hexidecimal symbols without any prefix (regex: [0-9a-fA-F])");
	puts("                 Alpha channel is discarded for image types not supporting it");
	puts("  -h --help      Prints this help message and exits");
	puts("Example:");
	puts("  usg --out spiral.bmp --size 1024 --fg ff0000ff");
	//   "-------------------------------------------------------------------------------"
}

int
main(int argc, const char *argv[])
{
	u32 size = 255;
	enum img_type type = IMG_TYPE_INVALID;
	const char *out = NULL;
	struct img_pixel fg = {.r=0, .g=0, .b=0, .a=255};
	struct img_pixel bg = {.r=255, .g=255, .b=255, .a=255};

	// WARN: argument parsing is done by hand which is pretty fragile
	// the alternative would be getopt(_long) but it's not portable outside of POSIX
	// and I don't wanna use external dependency like kimgr/getopt_port, nor implement this myself.
	dbg_assert(argc >= 1);
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			main_help();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(argv[i], "--fg") || !strcmp(argv[i], "--bg")) {
			if (i + 1 == argc) goto missing_additional_argument;
			const char *arg = argv[i + 1];
			struct img_pixel *color = !strcmp(argv[i], "--fg") ? &fg : &bg;
			const b8 success = img_pixel_from_arg(color, arg);
			if (!success) {
				fprintf(stderr, "Invalid color format: %s\n", arg);
				exit(EXIT_FAILURE);
			}
			++i;
		} else if (!strcmp(argv[i], "--out")) {
			if (i + 1 == argc) goto missing_additional_argument;
			out = argv[i + 1];
			++i;
		} else if (!strcmp(argv[i], "--type")) {
			if (i + 1 == argc) goto missing_additional_argument;
			const char *arg = argv[i + 1];
			if (!strcmp(arg, "bmp")) {
				type = IMG_TYPE_BMP;
			} else if (!strcmp(arg, "ppm")) {
				type = IMG_TYPE_PPM;
			} else {
				fprintf(stderr, "Invalid image type: %s\n", arg);
				exit(EXIT_FAILURE);
			}
			++i;
		} else if (!strcmp(argv[i], "--size")) {
			if (i + 1 == argc) goto missing_additional_argument;
			size = 0;
			for (const char *arg = argv[i + 1]; *arg != '\0'; ++arg) {
				if (*arg > '9' || *arg < '0') goto invalid_size_argument;
				if (size == 0 && *arg == '0') continue;
				const u32 val = (u32)(*arg - '0');
				if (size > UINT32_MAX / 10) goto invalid_size_argument;
				size *= 10;
				if (UINT32_MAX - size < val) goto invalid_size_argument;
				size += val;
			}
			if (size == 0)  goto invalid_size_argument;
			++i;
			continue;
		invalid_size_argument:
			fprintf(stderr, "Size is either invalid or out of scope: %s\n", argv[i + 1]);
			exit(EXIT_FAILURE);
		} else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			exit(EXIT_FAILURE);
		}
		continue;
	missing_additional_argument:
		fprintf(stderr, "%s requires additional argument\n", argv[i]);
		exit(EXIT_FAILURE);
	}

	if (out == NULL) {
		switch (type) {
		case IMG_TYPE_INVALID:
			type = IMG_TYPE_BMP;
			out = "a.bmp";
			break;
		case IMG_TYPE_BMP:
			out = "a.bmp";
			break;
		case IMG_TYPE_PPM:
			out = "a.ppm";
			break;
		default:
			dbg_unreachable();
		}
	}

	if (type == IMG_TYPE_INVALID) {
		const size_t len = strlen(out);
		dbg_assert(strlen(".ppm") == 4);
		dbg_assert(strlen(".bmp") == 4);
		if (len > 4 && !strcmp(out + len - 4, ".bmp")) {
			type = IMG_TYPE_BMP;
		} else if (len > 4 && !strcmp(out + len - 4, ".ppm")) {
			type = IMG_TYPE_PPM;
		} else {
			fprintf(stderr, "Unable to determine file type based on its name: %s\n", out);
			fprintf(stderr, "Try enforcing file type with: --type\n");
			exit(EXIT_FAILURE);
		}
	}

	// FIXME: generation of spiral was programmed in the way
	// that the starting point is always in the very middle
	// which means that width and height cannot be even!
	if (size % 2 == 0) {
		dbg_error("(FIXME) Size cannot be even! Decrementing size by 1...");
		--size;
	}

	const u64 max = size * size;
	const struct soe_cache cache = soe_init(max);
	struct img_context image = img_init(out, size, size, type);
	for (u64 i = 0; i < size; ++i) {
		for (u64 j = 0; j < size; ++j) {
			const i64 x = spr_screen_to_coord_x(j, size);
			const i64 y = spr_screen_to_coord_y(i, size);
			const u64 val = spr_coords_to_val(x,y);
			const b8 prime = soe_is_prime(cache, val);
			const struct img_pixel color = prime ? fg : bg;
			img_write(&image, color);
		}
	}
	img_deinit(&image);
	soe_deinit(cache);

	return EXIT_SUCCESS;
}

