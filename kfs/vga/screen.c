#include <vga.h>
#include <string.h>
#include <io.h>

size_t *textmemptr;
char attr = (char)0x0F;
int	csr_x = 0, csr_y = 0;

static void		scroll(void)
{
	size_t		blank, tmp;

	blank = 0x20 | (attr << 8);
	if (csr_y >= 25) {
		tmp = csr_y - 25 + 1;
		memcpy(textmemptr, textmemptr + tmp * 80, (25 - tmp) * 80 * 2);
		memsetw(textmemptr + (25 - tmp) * 80, blank, 80);
		csr_y = 24;
	}
}

static void		move_cursor(void)
{
	size_t	tmp;

	tmp = csr_y * 80 + csr_x;
	outportb(0x3D4, 14);
	outportb(0x3D5, tmp >> 8);
	outportb(0x3D4, 15);
	outportb(0x3D5, tmp);
}

extern void		clear_screen(void)
{
	unsigned char	blank;

	blank = 0x20 | (attr < 8);
	memsetw(textmemptr, blank, 80 * 25);
	csr_x = 0;
	csr_y = 0;
	move_cursor();
}

extern void		putchar(unsigned char c)
{
	size_t		*cursor;

	if (c == 0x08) {
		if (csr_x > 0) {
			csr_x--;
		}
	} else if (c == '\t') {
		//n & 000
		csr_x = (csr_x + 8) & ~(8 - 1);
	} else if (c == '\r') {
		csr_x = 0;
	} else if (c == '\n') {
		csr_y++;
		csr_x = 0;
	} else if (c >= ' ') {
		cursor = textmemptr + (csr_y * 80 + csr_x);
		*cursor = c | (attr << 8);
		csr_x++;
	}
	scroll();
	move_cursor();
}

extern void		putnbr(int n)
{
	if (n < 0) {
		putchar('-');
		putnbr(-n);
	}
	if (n < 9) {
		putchar((char)n + 48);
		return ;
	}
	putnbr(n / 10);
	putchar((char)(n % 10) + 48);
}

extern void		putnbr_base(int n, int base, uint8_t uppercase)
{
	static char	uset[16] =	{'0', '1', '2', '3', '4', '5',
							 '6', '7', '8', '9', 'A', 'B',
							 'C', 'D', 'E', 'F'};
	static char	lset[16] =	{'0', '1', '2', '3', '4', '5',
							 '6', '7', '8', '9', 'a', 'b',
							 'c', 'd', 'e', 'f'};

	if (n < 0) {
		putchar('-');
		putnbr_base(-n, base, uppercase);
	}
	if (n < base) {
		uppercase ? putchar(uset[n]) : putchar(lset[n]);
		return ;
	}
	putnbr_base(n / base, base, uppercase);
	uppercase ? putchar(uset[n % base]) : putchar(lset[n % base]);
	
}

extern void		putstring(const char *str)
{
	for (size_t i = 0; str[i]; i++) {
		putchar((const unsigned char)str[i]);
	}
}

extern void		set_textcolor(const unsigned char bg, const unsigned char fg)
{
	attr = (bg << 4) | (fg & 0x0F);
}

extern void		init_video(void)
{
	textmemptr = (size_t *)0xB8000;
	clear_screen();
}