#include "apilib.h"

void HariMain(void)
{
	int fh;
	char c, cmdline[30], *p;

	api_cmdline(cmdline, 30);
	for (p = cmdline; *p > ' '; p++) {}	/* ���ک`��������ޤ��i���w�Ф� */
	for (; *p == ' '; p++) {}	/* ���ک`�����i���w�Ф� */
	fh = api_fopen(p);
	if (fh != 0) {
		for (;;) {
			if (api_fread(&c, 1, fh) == 0) {
				break;
			}
			api_putchar(c);
		}
	}
	else {
		api_putstr0("File not found.\n");
	}
	api_end();
}
