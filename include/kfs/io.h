#ifndef __IO_H__
# define __IO_H__

#include <stddef.h>

extern void				outportb(size_t port, unsigned char byte);
extern unsigned char	inportb(size_t port);

#endif