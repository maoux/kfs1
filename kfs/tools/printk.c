#include <kernel.h>
#include <stdint.h>
#include <string.h>
#include <ftlib.h>
#include <vga.h>
#include <stdlib.h>

struct options {
	uint8_t		b_signed;
	uint8_t		b_zpadded;
	uint8_t		b_formatted;
	uint8_t		b_rightpadded;
	size_t		precision;
	size_t		width;
};

//globals

int ret_value;

//statics

static void		init_options(struct options *opts);
static size_t	printk_int(int arg, struct options opts);
static size_t	printk_oct(int arg, struct options opts);
static size_t	printk_hex(int arg, struct options opts, uint8_t uppercase);
static size_t	printk_bin(int arg, struct options opts);
static size_t	printk_char(char c);
static size_t	printk_string(char *str);
static void		pad(size_t len, char c);

static void		init_options(struct options *opts)
{
	opts->b_formatted = 0;
	opts->b_rightpadded = 0;
	opts->b_signed = 0;
	opts->b_zpadded = 0;
	opts->precision = 0;
	opts->width = 0;
}

static void		pad(size_t len, char c)
{
	for (;len > 0;len--) {
		putchar(c);
	}
}

static size_t	printk_int(int arg, struct options opts)
{
	size_t		len = nbrlen(arg);

	if (opts.width && !opts.b_rightpadded) {
		if (opts.precision && opts.width > opts.precision && opts.precision > len) {
			opts.b_zpadded ? pad(opts.width - opts.precision, '0') :  pad(opts.width - opts.precision, ' ');
		}
		else {
			opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
		}
	}
	if (opts.b_signed) {
		arg > 0 ? putchar('+') : putchar('-');
	}
	putnbr(arg);
	if (opts.width && opts.b_rightpadded) {	
		opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
	}
	return (len > opts.width ? len : opts.width);
}

static size_t	printk_oct(int arg, struct options opts)
{
	size_t len = nbrlen_base(arg, 8);

	if (opts.b_formatted) {
		putchar('0');
	}
	if (opts.width && !opts.b_rightpadded) {
		if (opts.precision && opts.width > opts.precision && opts.precision > len) {
			opts.b_zpadded ? pad(opts.width - opts.precision, '0') :  pad(opts.width - opts.precision, ' ');
		}
		else {
			opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
		}
	}
	putnbr_base(arg, 8, 0);
	if (opts.width && opts.b_rightpadded) {	
		opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
	}
	return (len > opts.width ? len : opts.width);
	
	return (0);
}

static size_t	printk_hex(int arg, struct options opts, uint8_t uppercase)
{
	size_t		len = nbrlen_base(arg, 16);

	if (opts.b_formatted) {
		uppercase ? putstring("0X") : putstring("0x");
		len += 2;
	}
	if (opts.width && !opts.b_rightpadded) {
		if (opts.precision && opts.width > opts.precision && opts.precision > len) {
			opts.b_zpadded ? pad(opts.width - opts.precision, '0') :  pad(opts.width - opts.precision, ' ');
		}
		else {
			opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
		}
	}
	putnbr_base(arg, 16, uppercase);
	if (opts.width && opts.b_rightpadded) {
		opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
	}
	return (len > opts.width ? len : opts.width);
}

static size_t	printk_bin(int arg, struct options opts)
{
	size_t		len = nbrlen_base(arg, 2);

	if (opts.b_formatted) {
		putstring("0b");
		len += 2;
	}
	if (opts.width && !opts.b_rightpadded) {
		if (opts.precision && opts.width > opts.precision && opts.precision > len) {
			opts.b_zpadded ? pad(opts.width - opts.precision, '0') :  pad(opts.width - opts.precision, ' ');
		}
		else {
			opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
		}
	}
	putnbr_base(arg, 2, 0);
	if (opts.width && opts.b_rightpadded) {
		opts.b_zpadded ? pad(opts.width - len, '0') :  pad(opts.width - len, ' ');
	}
	return (len > opts.width ? len : opts.width);
}

static size_t	printk_char(char c)
{
	putchar(c);
	return (0);
}

static size_t	printk_string(char *str)
{
	putstring((const char *)str);
	return (0);
}

extern int	printk(const char *fmt, ...)
{
	char	**args = (char **) &fmt;
	struct options opts = {0, 0, 0, 0, 0, 0};

	ret_value = 0;
	args++;
	while (*fmt != '\0') {
		if (*fmt == '%') {
			fmt++;
			do {
				if (*fmt == '0') {
					opts.b_zpadded = 1;
					fmt++;
				}
				if (*fmt == '#') {
					opts.b_formatted = 1;
					fmt++;
				}
				if (*fmt== '-') {
					opts.b_rightpadded = 1;
					fmt++;
					while (*fmt >= '0' && *fmt <= '9') {
						opts.precision = opts.precision * 10 + chtoi(*fmt);
						fmt++;
					}
				}
				if (*fmt > '0' && *fmt < '9') {
					while (*fmt >= '0' && *fmt <= '9') {
						opts.width = opts.width * 10 + chtoi(*fmt);
						fmt++;
					}
				}
				if (*fmt == '+') {
					opts.b_signed = 1;
					fmt++;
				}
				if (*fmt == 'd' || *fmt == 'i') {
					ret_value += printk_int(*((int *) args++), opts);
				}
				else if (*fmt == 'u') {
					ret_value += printk_int(*((int *) args++), opts);
				}
				else if (*fmt == '%') {
					ret_value++;
					putchar('%');
				}
				else if (*fmt == 'o') {
					ret_value += printk_oct(*((int *) args++), opts);
				}
				else if (*fmt == 'x' || *fmt == 'X') {
					ret_value += printk_hex(*((int *) args++), opts, (*fmt == 'x') ? 0 : 1);
				}
				else if (*fmt == 'b') {
					ret_value += printk_bin(*((int *) args++), opts);
				}
				else if (*fmt == 's') {
					if (*args == NULL) {
						ret_value += printk_string("(null)");
					}
					else {
						ret_value += printk_string(*args++);
					}
				}
				else if (*fmt == 'c') {
					ret_value += printk_char((char)(*((int *) args++)));
				}
			} while (strchr("diuoxXbcs%", *fmt) == NULL);
			init_options(&opts);
		}
		else {
			ret_value++;
			putchar(*fmt);
		}
		fmt++;
	}
	return (ret_value);
}