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


int
main(int argc, const char *argv[])
{
	struct img_context image = {
		.width = 201,
		.height = 201,
		.type = IMG_TYPE_INVALID,
		.start_val = 1,
	};
	const char *out = NULL;
	struct img_color fg = {.r=0, .g=0, .b=0, .a=255};
	struct img_color bg = {.r=255, .g=255, .b=255, .a=255};
	b8 no_stdout = false;
	b8 start_x_assigned = false;
	b8 start_y_assigned = false;
	b8 faded = false;

	// TODO: would be nice to support this someday:
	// https://en.wikipedia.org/wiki/Prime_k-tuple

	// NOTE: I cannot use getopt because it a non-standard POSIX function
	dbg_assert(argc >= 1);
	for (int i = 1; i < argc; ++i) {
		if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
			_main_help();
			exit(EXIT_SUCCESS);
		} else if (!strcmp(argv[i], "--fg")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!img_color_from_str(&fg, arg)) goto invalid_color;
			++i;
		} else if (!strcmp(argv[i], "--bg")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!img_color_from_str(&bg, arg)) goto invalid_color;
			++i;
		} else if (!strcmp(argv[i], "--out") || !strcmp(argv[i], "-o")) {
			if (i + 1 == argc) goto missing_additional;
			out = argv[i + 1];
			++i;
		} else if (!strcmp(argv[i], "--type")) {
			if (i + 1 == argc) goto missing_additional;
			const char *arg = argv[i + 1];
			if (!strcmp(arg, "bmp")) {
				image.type = IMG_TYPE_BMP;
			} else if (!strcmp(arg, "ppm")) {
				image.type = IMG_TYPE_PPM;
			} else {
				_main_exit_failure("Invalid image type: %s\n", arg);
			}
			++i;
		} else if (!strcmp(argv[i], "--width")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &(image.width))) goto invalid_num;
			if (image.width == 0)  goto invalid_num;
			++i;
		} else if (!strcmp(argv[i], "--height")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &(image.height))) goto invalid_num;
			if (image.height == 0)  goto invalid_num;
			++i;
		} else if (!strcmp(argv[i], "--start-x")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &(image.start_x))) goto invalid_num;
			start_x_assigned = true;
			++i;
		} else if (!strcmp(argv[i], "--start-y")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &(image.start_y))) goto invalid_num;
			start_y_assigned = true;
			++i;
		} else if (!strcmp(argv[i], "--start-val")) {
			if (i + 1 == argc) goto missing_additional;
			if (!_main_arg_to_u32(argv[i + 1], &(image.start_val))) goto invalid_num;
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
		switch (image.type) {
		case IMG_TYPE_INVALID:
			image.type = IMG_TYPE_BMP;
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

	if (image.type == IMG_TYPE_INVALID) {
		const size_t len = strlen(out);
		if (len > 4 && !strcmp(out + len - 4, ".bmp"))
			image.type = IMG_TYPE_BMP;
		else if (len > 4 && !strcmp(out + len - 4, ".ppm"))
			image.type = IMG_TYPE_PPM;
		else
			_main_exit_failure("Unable to determine file type based on its name: %s\n", out);
	}

	if (!start_x_assigned)
		image.start_x = image.width / 2;

	if (!start_y_assigned)
		image.start_y = image.height / 2;

	dbg_log("Attempting to open file: %s", out);
	image.file = fopen(out , "w");
	if (image.file == NULL)
		_main_exit_failure("Unable to open file: %s\n", out);
	img_write_header(&image);
	struct soe_cache cache;
	const u64 max = img_val_max(&image);
	if (!soe_init(&cache, max))
		_main_exit_failure("Failed to create Prime Numbers cache!\n");
	for (u32 y = 0; y < image.height; ++y) {
		for (u32 x = 0; x < image.width; ++x) {
			const u64 val = img_val_from_coords(&image, x, y);
			const b8 prime = soe_is_prime(&cache, val);
			struct img_color color;
			if (faded) {
				const f32 ratio = (f32)val / (f32)max;
				const f32 ratio_fixed = (prime) ? (ratio) : (1 - ratio);
				color = img_color_faded(fg, bg, ratio_fixed);
			} else {
				color = prime ? fg : bg;
			}
			img_write_pixel(&image, color);
		}
	}
	if (!soe_deinit(&cache))
		_main_exit_failure("Failed to deinitialize Prime Numbers cache!\n");
	if (fclose(image.file))
		_main_exit_failure("Unable to close file: %s !\n", out);

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
	puts("  --width <NUM>   Width of the image in pixels (default: 201)");
	puts("  --height <NUM>  Height of the image in pixels (default: 201)");
	puts("  --type <TYPE>   Type of output image file (extension)");
	puts("                  Accepts either: 'ppm' or 'bmp'");
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

