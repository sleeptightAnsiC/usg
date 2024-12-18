#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "./soe.h"
#include "./img.h"
#include "./typ.h"
#include "./dbg.h"


// TODO: If I ever would like to replace this macro
// it is possible with variadic function and 'vfprintf' and 'stdarg'
// but current solution is better for diagnostic and generally shorter.
#define _main_exit_failure(...) \
	do { \
		fprintf(stderr, "usg: "); \
		fprintf(stderr, __VA_ARGS__); \
		if (errno != 0) { \
			dbg_assert(errno != EXIT_SUCCESS); \
			perror("usg"); \
			exit(errno); \
		} else { \
			exit(EXIT_FAILURE); \
		} \
	} while(0) \


static void _main_help(void);
static b8 _main_arg_to_u32(const char *arg, u32 *out);
static b8 _main_arg_to_color(const char *arg, struct img_color *out);


int
main(int argc, const char *argv[])
{
	u32 width = 255;
	u32 height = 255;
	enum img_type type = IMG_TYPE_INVALID;
	const char *out = NULL;
	struct img_color fg = {.r=0, .g=0, .b=0, .a=255};
	struct img_color bg = {.r=255, .g=255, .b=255, .a=255};
	b8 no_stdout = false;
	u32 start_val = 1;
	u32 start_x;
	u32 start_y;
	b8 start_x_assigned = false;
	b8 start_y_assigned = false;
	b8 faded = false;

	// TODO: would be nice to support this someday:
	// https://en.wikipedia.org/wiki/Prime_k-tuple

	// WARN: argument parsing is done by hand which is pretty fragile
	// the alternative would be getopt(_long) but it's not portable outside of POSIX
	// and I don't wanna use external dependency like kimgr/getopt_port, nor implement this myself.
	dbg_assert(argc >= 1);
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			_main_help();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(argv[i], "--fg")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!_main_arg_to_color(arg, &fg)) goto invalid_color;
			++i;
		} else if (!strcmp(argv[i], "--bg")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!_main_arg_to_color(arg, &bg)) goto invalid_color;
			++i;
		} else if (!strcmp(argv[i], "--out")) {
			if (i + 1 == argc) goto missing_additional;
			out = argv[i + 1];
			++i;
		} else if (!strcmp(argv[i], "--type")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!strcmp(arg, "bmp")) {
				type = IMG_TYPE_BMP;
			} else if (!strcmp(arg, "ppm")) {
				type = IMG_TYPE_PPM;
			} else {
				_main_exit_failure("Invalid image type: %s\n", arg);
			}
			++i;
		} else if (!strcmp(argv[i], "--width")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &width)) goto invalid_num;
			if (width == 0)  goto invalid_num;
			++i;
		} else if (!strcmp(argv[i], "--height")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &height)) goto invalid_num;
			if (height == 0)  goto invalid_num;
			++i;
		} else if (!strcmp(argv[i], "--start-x")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &start_x)) goto invalid_num;
			start_x_assigned = true;
			++i;
		} else if (!strcmp(argv[i], "--start-y")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &start_y)) goto invalid_num;
			start_y_assigned = true;
			++i;
		} else if (!strcmp(argv[i], "--start-val")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &start_val)) goto invalid_num;
			++i;
		} else if (!strcmp(argv[i], "--no-stdout")) {
			no_stdout = true;
		} else if (!strcmp(argv[i], "--faded")) {
			faded = true;
		} else {
			_main_exit_failure("Unknown option: %s\n", argv[i]);
		}
		continue;
	missing_additional:
		_main_exit_failure("%s requires additional argument\n", argv[i]);
	invalid_num:
		_main_exit_failure("Size is either invalid or out of scope: %s\n", argv[i + 1]);
	invalid_color:
		_main_exit_failure("Invalid color format: %s\n", argv[i + 1]);
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
		if (len > 4 && !strcmp(out + len - 4, ".bmp"))
			type = IMG_TYPE_BMP;
		else if (len > 4 && !strcmp(out + len - 4, ".ppm"))
			type = IMG_TYPE_PPM;
		else
			_main_exit_failure("Unable to determine file type based on its name: %s\n", out);
	}

	if (!start_x_assigned)
		start_x = width / 2;

	if (!start_y_assigned)
		start_y = height / 2;

	struct img_context image;
	if (!img_init(&image, out, width, height, start_x, start_y, start_val, type))
		_main_exit_failure("Unable to create image context for %s\n", out);
	const u64 max = img_val_max(&image);
	struct soe_cache cache;
	if (!soe_init(&cache, max))
		_main_exit_failure("Failed to create Prime Numbers cache!\n");
	for (u32 y = 0; y < height; ++y) {
		for (u32 x = 0; x < width; ++x) {
			const u64 val = img_val_from_coords(&image, x, y);
			const b8 prime = soe_is_prime(&cache, val);
			struct img_color color;
			if (faded) {
				const f32 ratio = (f32)val / (f32)max;
				const f32 ratio_fixed = (prime) ? (ratio) : (1 - ratio);
				const struct img_color diff = {
					.r = (fg.r > bg.r) ? (fg.r - bg.r) : (bg.r - fg.r),
					.g = (fg.g > bg.g) ? (fg.g - bg.g) : (bg.g - fg.g),
					.b = (fg.b > bg.b) ? (fg.b - bg.b) : (bg.b - fg.b),
					.a = (fg.a > bg.a) ? (fg.a - bg.a) : (bg.a - fg.a),
				};
				const struct img_color part = {
					.r = (u8)(ratio_fixed * (f32)diff.r),
					.g = (u8)(ratio_fixed * (f32)diff.g),
					.b = (u8)(ratio_fixed * (f32)diff.b),
					.a = (u8)(ratio_fixed * (f32)diff.a),
				};
				color = (struct img_color){
					.r = (fg.r > bg.r) ? (part.r + bg.r) : (fg.r + part.r),
					.g = (fg.g > bg.g) ? (part.g + bg.g) : (fg.g + part.g),
					.b = (fg.b > bg.b) ? (part.b + bg.b) : (fg.b + part.b),
					.a = (fg.a > bg.a) ? (part.a + bg.a) : (fg.a + part.a),
				};
			} else {
				color = prime ? fg : bg;
			}
			img_write(&image, color);
		}
	}
	if (!soe_deinit(&cache))
		_main_exit_failure("Failed to deinitialize Prime Numbers cache!\n");
	if (!img_deinit(&image))
		_main_exit_failure("Unable to deinitialize image context for %s !\n", out);

	if (!no_stdout)
		printf("%s\n", out);

	exit(EXIT_SUCCESS);
}

static void
_main_help(void)
{
	//   "-------------------------------------------------------------------------------"
	puts("Ulam Spiral Generator <https://github.com/sleeptightAnsiC/usg>");
	puts("Usage:");
	puts("  usg [option1] [option2] [optionN]");
	puts("Options:");
	puts("  --width <NUM>   Width of the image in pixels (default: 255)");
	puts("  --height <NUM>  Height of the image in pixels (default: 255)");
	puts("  --type <TYPE>   Type of output image file. Accepts either 'ppm' or 'bmp'");
	puts("                  If unset, determined automatically based on file name");
	puts("  --out <FILE>    Name of output image file (default: 'a.bmp')");
	puts("  --fg <COLOR>    Foreground color of the image (default: '000000ff')");
	puts("  --bg <COLOR>    Background color of the image (default: 'ffffffff')");
	puts("                  COLOR must be in HEX format represented by exactly eight");
	puts("                  hexidecimal symbols without any prefix (regex: [0-9a-fA-F])");
	puts("                  Alpha channel is discarded for image types not supporting it");
	puts("  --start-x <NUM>  X coordinate where the spiral starts (default: width/2)");
	puts("  --start-y <NUM>  Y coordinate where the spiral starts (default: height/2)");
	puts("                  NUM must be an OpenGL-style \"screen coordinate\",");
	puts("                  meaning that 0:0 is in the top left corner of the image");
	puts("  --start-val <NUM> Value that spiral uses at its starting point (default: 1)");
	puts("  --no-stdout     Do NOT print the image path to stdout after creation");
	puts("  --faded         Fade between background and forground colors (default: off)");
	puts("                  Fade is calculated based on the value on spiral and");
	puts("                  the max possible value that can occur on spiral (val/max)");
	puts("                  Use in order to see how values are being distributed");
	puts("  -h --help       Prints this help message and exits");
	puts("Example:");
	puts("  usg --out spiral.bmp --width 1024 --height 1024 --fg 00ff00ff --bg 000000ff");
	//   "-------------------------------------------------------------------------------"
}

static b8
_main_arg_to_u32(const char *arg, u32 *out)
{
	dbg_assert(arg != NULL);
	dbg_assert(out != NULL);
	u32 sum = 0;
	for (; *arg != '\0'; ++arg) {
		if (*arg < '0' || *arg > '9') return false;
		if (sum == 0 && *arg == '0') continue;
		if (sum > UINT32_MAX / 10) return false;
		sum *= 10;
		const u32 val = (u32)(*arg - '0');
		if (UINT32_MAX - sum < val) return false;
		sum += val;
	}
	*out = sum;
	return true;
}

static b8
_main_arg_to_color(const char *arg, struct img_color *out)
{
	dbg_assert(arg != NULL);
	dbg_assert(out != NULL);
	u8 vals[8];
	for (int i = 0; i < 8; i += 1) {
		DBG_STATIC_ASSERT('0' < 'A');
		DBG_STATIC_ASSERT('A' < 'a');
		if (arg[i] == '\0') return false;
		if (arg[i] < '0') return false;
		if (arg[i] > 'f') return false;
		if (arg[i] >= 'a')
			vals[i] = (u8)(arg[i] - 'a' + 10);
		else if (arg[i] >= 'A')
			vals[i] = (u8)(arg[i] - 'A' + 10);
		else if (arg[i] >= '0')
			vals[i] = (u8)(arg[i] - '0');
		else
			dbg_unreachable();
	}
	if (arg[8] != '\0') return false;
	out->r = (u8)(vals[0] * 16 + vals[1]);
	out->g = (u8)(vals[2] * 16 + vals[3]);
	out->b = (u8)(vals[4] * 16 + vals[5]);
	out->a = (u8)(vals[6] * 16 + vals[7]);
	return true;
}
