/*
 * Copyright (c) 2003, 2004, 2006 Tama Communications Corporation
 *
 * This file is part of GNU GLOBAL.
 *
 * GNU GLOBAL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU GLOBAL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.
 */
#ifndef _INCOP_H
#define _INCOP_H

#include "queue.h"
#include "strbuf.h"

struct data {
#if defined(_WIN32) || defined(__DJGPP__)
	char name[MAXPATHLEN];
#else
	char *name;
#endif
        int id;
        int count;
        int ref_count;
        STRBUF *contents;
        STRBUF *ref_contents;
};

void init_inc(void);
void put_inc(const char *, const char *, int);
struct data *get_inc(const char *);
struct data *first_inc(void);
struct data *next_inc(void);
void put_included(struct data *, const char *);
struct data *get_included(const char *);

#endif /* ! _INCOP_H */
