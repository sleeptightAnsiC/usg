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
	puts("Usage:");
	puts("  usg [option1] [option2] [optionN]");
	puts("Options:");
	puts("  --width <NUM>   Width of the image in pixels (default: 128)");
	puts("  --height <NUM>  Height of the image in pixels (default: 128)");
	puts("  --type <TYPE>   Type of output image file. Accepts either 'ppm' or 'bmp'");
	puts("                  If unset, determined automatically based on file name");
	puts("  --out <FILE>    Name of output image file (default: 'a.bmp')");
	puts("  --fg <COLOR>    Foreground color of the image (default: '000000ff')");
	puts("  --bg <COLOR>    Background color of the image (default: 'ffffffff')");
	puts("                  COLOR must be in HEX format represented by exactly eight");
	puts("                  hexidecimal symbols without any prefix (regex: [0-9a-fA-F])");
	puts("                  Alpha channel is discarded for image types not supporting it");
	puts("  --cx <NUM>      Center coordinate X of the spiral (default: width/2)");
	puts("  --cy <NUM>      Center coordinate Y of the spiral (default: height/2)");
	puts("                  NUM must be a \"screen coordinate\",");
	puts("                  meaning that 0:0 is in the top left corner of the image");
	puts("  -h --help       Prints this help message and exits");
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
			u8 vals[8];
			for (int j = 0; j < 8; j += 1) {
				DBG_STATIC_ASSERT('0' < 'A');
				DBG_STATIC_ASSERT('A' < 'a');
				if (arg[j] == '\0') goto invalid_color_format;
				if (arg[j] < '0') goto invalid_color_format;
				if (arg[j] > 'f') goto invalid_color_format;
				if (arg[j] >= 'a')
					vals[j] = (u8)(arg[j] - 'a' + 10);
				else if (arg[j] >= 'A')
					vals[j] = (u8)(arg[j] - 'A' + 10);
				else if (arg[j] >= '0')
					vals[j] = (u8)(arg[j] - '0');
				else
					dbg_unreachable();
			}
			if (arg[8] != '\0') goto invalid_color_format;
			color->r = (u8)(vals[0] * 16 + vals[1]);
			color->g = (u8)(vals[2] * 16 + vals[3]);
			color->b = (u8)(vals[4] * 16 + vals[5]);
			color->a = (u8)(vals[6] * 16 + vals[7]);
			++i;
			continue;
		invalid_color_format:
			fprintf(stderr, "Invalid color format: %s\n", arg);
			exit(EXIT_FAILURE);
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
			/* FALLTHROUGH */
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
	for (u32 x = 0; x < size; ++x) {
		for (u32 y = 0; y < size; ++y) {
			const u64 val = img_val_from_coords(&image, x,y);
			const b8 prime = soe_is_prime(cache, val);
			const struct img_pixel color = prime ? fg : bg;
			img_write(&image, color);
		}
	}
	img_deinit(&image);
	soe_deinit(cache);

	return EXIT_SUCCESS;
}

