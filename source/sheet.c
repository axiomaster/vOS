/* ƒ}ƒEƒX‚âƒEƒBƒ“ƒhƒE‚Ìd‚Ë‡‚í‚¹ˆ— */

#include "bootpack.h"

#define SHEET_USE		1

struct SHTCTL *shtctl_init(struct MEMMAN *memman, unsigned char *vram, int xsize, int ysize)
{
	struct SHTCTL *ctl;
	int i;
	ctl = (struct SHTCTL *) memman_alloc_4k(memman, sizeof (struct SHTCTL));
	if (ctl == 0) {
		goto err;
	}
	ctl->map = (unsigned char *) memman_alloc_4k(memman, xsize * ysize);
	if (ctl->map == 0) {
		memman_free_4k(memman, (int) ctl, sizeof (struct SHTCTL));
		goto err;
	}
	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1; /* ƒV[ƒg‚Íˆê–‡‚à‚È‚¢ */
	for (i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = 0; /* –¢g—pƒ}[ƒN */
		ctl->sheets0[i].ctl = ctl; /* Š‘®‚ğ‹L˜^ */
	}
err:
	return ctl;
}

struct SHEET *sheet_alloc(struct SHTCTL *ctl)
{
	struct SHEET *sht;
	int i;
	for (i = 0; i < MAX_SHEETS; i++) {
		if (ctl->sheets0[i].flags == 0) {
			sht = &ctl->sheets0[i];
			sht->flags = SHEET_USE; /* g—p’†ƒ}[ƒN */
			sht->height = -1; /* ”ñ•\¦’† */
			sht->task = 0;	/* ©“®‚Å•Â‚¶‚é‹@”\‚ğg‚í‚È‚¢ */
			return sht;
		}
	}
	return 0;	/* ‘S‚Ä‚ÌƒV[ƒg‚ªg—p’†‚¾‚Á‚½ */
}

void sheet_setbuf(struct SHEET *sht, unsigned char *buf, int xsize, int ysize, int col_inv)
{
	sht->buf = buf;
	sht->bxsize = xsize;
	sht->bysize = ysize;
	sht->col_inv = col_inv;
	return;
}

void sheet_refreshmap(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1, sid4, *p;
	unsigned char *buf, sid, *map = ctl->map;
	struct SHEET *sht;
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= ctl->top; h++) {
		sht = ctl->sheets[h];
		sid = sht - ctl->sheets0; /* ”Ô’n‚ğˆø‚«Z‚µ‚Ä‚»‚ê‚ğ‰º‚¶‚«”Ô†‚Æ‚µ‚Ä—˜—p */
		buf = sht->buf;
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		if (sht->col_inv == -1) {
			if ((sht->vx0 & 3) == 0 && (bx0 & 3) == 0 && (bx1 & 3) == 0) {
				bx1 = (bx1 - bx0) / 4; /* MOV»ØÊı */
				sid4 = sid | sid << 8 | sid << 16 | sid << 24;
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					vx = sht->vx0 + bx0;
					p = (int *)&map[vy * ctl->xsize + vx];
					for (bx = 0; bx < bx1; bx++) {
						p[bx] = sid4;
					}
				}
			}
			else {
				for (by = by0; by < by1; by++) {
					vy = sht->vy0 + by;
					for (bx = bx0; bx < bx1; bx++) {
						vx = sht->vx0 + bx;
						map[vy*ctl->xsize + vx] = sid;
					}
				}
			}
		}
		else {
			for (by = by0; by < by1; by++) {
				vy = sht->vy0 + by;
				for (bx = bx0; bx < bx1; bx++) {
					vx = sht->vx0 + bx;
					if (buf[by * sht->bxsize + bx] != sht->col_inv) {
						map[vy * ctl->xsize + vx] = sid;
					}
				}
			}
		}
	}
	return;
}

void sheet_refreshsub(struct SHTCTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1)
{
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	unsigned char *buf, *vram = ctl->vram, *map = ctl->map, sid;
	struct SHEET *sht;
	/* refresh”ÍˆÍ‚ª‰æ–ÊŠO‚É‚Í‚İo‚µ‚Ä‚¢‚½‚ç•â³ */
	if (vx0 < 0) { vx0 = 0; }
	if (vy0 < 0) { vy0 = 0; }
	if (vx1 > ctl->xsize) { vx1 = ctl->xsize; }
	if (vy1 > ctl->ysize) { vy1 = ctl->ysize; }
	for (h = h0; h <= h1; h++) {
		sht = ctl->sheets[h];
		buf = sht->buf;
		sid = sht - ctl->sheets0;
		/* vx0`vy1‚ğg‚Á‚ÄAbx0`by1‚ğ‹tZ‚·‚é */
		bx0 = vx0 - sht->vx0;
		by0 = vy0 - sht->vy0;
		bx1 = vx1 - sht->vx0;
		by1 = vy1 - sht->vy0;
		if (bx0 < 0) { bx0 = 0; }
		if (by0 < 0) { by0 = 0; }
		if (bx1 > sht->bxsize) { bx1 = sht->bxsize; }
		if (by1 > sht->bysize) { by1 = sht->bysize; }
		for (by = by0; by < by1; by++) {
			vy = sht->vy0 + by;
			for (bx = bx0; bx < bx1; bx++) {
				vx = sht->vx0 + bx;
				if (map[vy * ctl->xsize + vx] == sid) {
					vram[vy * ctl->xsize + vx] = buf[by * sht->bxsize + bx];
				}
			}
		}
	}
	return;
}

void sheet_updown(struct SHEET *sht, int height)
{
	struct SHTCTL *ctl = sht->ctl;
	int h, old = sht->height; /* İ’è‘O‚Ì‚‚³‚ğ‹L‰¯‚·‚é */

	/* w’è‚ª’á‚·‚¬‚â‚‚·‚¬‚¾‚Á‚½‚çAC³‚·‚é */
	if (height > ctl->top + 1) {
		height = ctl->top + 1;
	}
	if (height < -1) {
		height = -1;
	}
	sht->height = height; /* ‚‚³‚ğİ’è */

	/* ˆÈ‰º‚Íå‚Ésheets[]‚Ì•À‚×‘Ö‚¦ */
	if (old > height) {	/* ˆÈ‘O‚æ‚è‚à’á‚­‚È‚é */
		if (height >= 0) {
			/* ŠÔ‚Ì‚à‚Ì‚ğˆø‚«ã‚°‚é */
			for (h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h - 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height + 1, old);
		} else {	/* ”ñ•\¦‰» */
			if (ctl->top > old) {
				/* ã‚É‚È‚Á‚Ä‚¢‚é‚à‚Ì‚ğ‚¨‚ë‚· */
				for (h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h + 1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--; /* •\¦’†‚Ì‰º‚¶‚«‚ªˆê‚ÂŒ¸‚é‚Ì‚ÅAˆê”Ôã‚Ì‚‚³‚ªŒ¸‚é */
			sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0);
			sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, 0, old - 1);
		}
	} else if (old < height) {	/* ˆÈ‘O‚æ‚è‚à‚‚­‚È‚é */
		if (old >= 0) {
			/* ŠÔ‚Ì‚à‚Ì‚ğ‰Ÿ‚µ‰º‚°‚é */
			for (h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h + 1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sht;
		} else {	/* ”ñ•\¦ó‘Ô‚©‚ç•\¦ó‘Ô‚Ö */
			/* ã‚É‚È‚é‚à‚Ì‚ğ‚¿ã‚°‚é */
			for (h = ctl->top; h >= height; h--) {
				ctl->sheets[h + 1] = ctl->sheets[h];
				ctl->sheets[h + 1]->height = h + 1;
			}
			ctl->sheets[height] = sht;
			ctl->top++; /* •\¦’†‚Ì‰º‚¶‚«‚ªˆê‚Â‘‚¦‚é‚Ì‚ÅAˆê”Ôã‚Ì‚‚³‚ª‘‚¦‚é */
		}
		sheet_refreshmap(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height);
		sheet_refreshsub(ctl, sht->vx0, sht->vy0, sht->vx0 + sht->bxsize, sht->vy0 + sht->bysize, height, height);
	}
	return;
}

void sheet_refresh(struct SHEET *sht, int bx0, int by0, int bx1, int by1)
{
	if (sht->height >= 0) { /* ‚à‚µ‚à•\¦’†‚È‚çAV‚µ‚¢‰º‚¶‚«‚Ìî•ñ‚É‰ˆ‚Á‚Ä‰æ–Ê‚ğ•`‚«’¼‚· */
		sheet_refreshsub(sht->ctl, sht->vx0 + bx0, sht->vy0 + by0, sht->vx0 + bx1, sht->vy0 + by1, sht->height, sht->height);
	}
	return;
}

void sheet_slide(struct SHEET *sht, int vx0, int vy0)
{
	struct SHTCTL *ctl = sht->ctl;
	int old_vx0 = sht->vx0, old_vy0 = sht->vy0;
	sht->vx0 = vx0;
	sht->vy0 = vy0;
	if (sht->height >= 0) { /* ‚à‚µ‚à•\¦’†‚È‚çAV‚µ‚¢‰º‚¶‚«‚Ìî•ñ‚É‰ˆ‚Á‚Ä‰æ–Ê‚ğ•`‚«’¼‚· */
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0);
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height);
		sheet_refreshsub(ctl, old_vx0, old_vy0, old_vx0 + sht->bxsize, old_vy0 + sht->bysize, 0, sht->height - 1);
		sheet_refreshsub(ctl, vx0, vy0, vx0 + sht->bxsize, vy0 + sht->bysize, sht->height, sht->height);
	}
	return;
}

void sheet_free(struct SHEET *sht)
{
	if (sht->height >= 0) {
		sheet_updown(sht, -1); /* •\¦’†‚È‚ç‚Ü‚¸”ñ•\¦‚É‚·‚é */
	}
	sht->flags = 0; /* –¢g—pƒ}[ƒN */
	return;
}
