/*
 * Copyright (c) 2004, 2005 Tama Communications Corporation
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <stdio.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>
#include <sys/types.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#include <sys/file.h>
#endif

#include "global.h"
#include "common.h"
#include "path2url.h"
#include "htags.h"

/*----------------------------------------------------------------------*/
/* Tag definitions							*/
/*----------------------------------------------------------------------*/
char *html_begin;
char *html_end;
char *body_begin;
char *body_end;
char *head_begin;
char *head_end;
char *title_begin;
char *title_end;
char *header_begin;
char *header_end;
char *cvslink_begin;
char *cvslink_end;
char *caution_begin;
char *caution_end;
char *list_begin;
char *item_begin;
char *item_end;
char *list_end;
char *table_begin;
char *table_end;
char *comment_begin;
char *comment_end;
char *sharp_begin;
char *sharp_end;
char *brace_begin;
char *brace_end;
char *verbatim_begin;
char *verbatim_end;
char *reserved_begin;
char *reserved_end;
char *position_begin;
char *position_end;
char *warned_line_begin;
char *warned_line_end;
char *error_begin;
char *error_end;
char *message_begin;
char *message_end;
char *string_begin;
char *string_end;
char *quote_great;
char *quote_little;
char *quote_amp;
char *quote_space;
char *hr;
char *br;
/*
 * Set up HTML tags.
 */
void
setup_html()
{
	html_begin	= "<html>";
	html_end	= "</html>";
	body_begin	= "<body>";
	body_end	= "</body>";
	head_begin	= "<head>";
	head_end	= "</head>";
	title_begin	= "<h1><font color='#cc0000'>";
	title_end	= "</font></h1>";
	header_begin	= "<h2>";
	header_end	= "</h2>";
	cvslink_begin	= "<font size='-1'>";
	cvslink_end	= "</font>";
	caution_begin	= "<center>\n<blockquote>";
	caution_end	= "</blockquote>\n</center>";
	list_begin	= "<ol>";
	item_begin	= "<li>";
	item_end	= "";
	list_end	= "</ol>";
	table_begin	= "<table>";
	table_end	= "</table>";
	comment_begin	= "<i><font color='green'>";
	comment_end	= "</font></i>";
	sharp_begin	= "<font color='darkred'>";
	sharp_end	= "</font>";
	brace_begin	= "<font color='blue'>";
	brace_end	= "</font>";
	verbatim_begin	= "<pre>";
	verbatim_end	= "</pre>";
	reserved_begin	= "<b>";
	reserved_end	= "</b>";
	position_begin	= "<font color='gray'>";
	position_end	= "</font>";
	warned_line_begin = "<span style='background-color:yellow'>";
	warned_line_end   = "</span>";
	error_begin     = "<h1><font color='#cc0000'>";
	error_end       = "</font></h1>";
	message_begin   = "<h3>";
	message_end     = "</h3>";
	string_begin	= "<u>";
	string_end	= "</u>";
	quote_great	= "&gt;";
	quote_little	= "&lt;";
	quote_amp	= "&amp;";
	quote_space	= "&nbsp;";
	hr              = "<hr>";
	br              = "<br>";
}
/*
 * Set up XHTML tags.
 * (under construction)
 */
void
setup_xhtml()
{
	setup_html();
}
/*
 * Set up TEX tags.
 * (under construction)
 */
void
setup_tex()
{
	setup_html();
}
/*
 * Generate meta record.
 */
char *
meta_record()
{
	static char buf[512];
	char *s1 = "meta name='robots' content='noindex,nofollow'";
	char *s2 = "meta name='generator'";

	snprintf(buf, sizeof(buf), "<%s>\n<%s content='GLOBAL-%s'>\n", s1, s2, get_version());
	return buf;
}
/*
 * Generate upper directory.
 */
char *
upperdir(dir)
	const char *dir;
{
	static char path[MAXPATHLEN];
	snprintf(path, sizeof(path), "../%s", dir);
	return path;
}
/*
 * Generate header tag.
 */
char *
set_header(title)
	char *title;
{
	static STRBUF *sb = NULL;

	if (sb == NULL)
		sb = strbuf_open(0);
	else
		strbuf_reset(sb);
	strbuf_sprintf(sb, "%s\n", head_begin);
	strbuf_sprintf(sb, "<title>%s</title>\n", title);
	strbuf_puts(sb, meta_record());
	if (style_sheet)
		strbuf_puts(sb, style_sheet);
	strbuf_sprintf(sb, "%s\n", head_end);

	return strbuf_value(sb);
}

/*
 * Generate image tag.
 *
 *	i)	where	Where is the icon directory?
 *			CURRENT: current directory
 *			PARENT: parent directory
 *	i)	file	icon file without suffix.
 *	i)	alt	alt string
 */
char *
gen_image(where, file, alt)
	int where;
	const char *file;
	const char *alt;
{
	static char buf[1024];
	char *dir = (where == PARENT) ? "../icons" : "icons";
		
	snprintf(buf, sizeof(buf), "<img src='%s/%s.%s' alt='[%s]' %s>",
		dir, file, icon_suffix, alt, icon_spec);
	return buf;
}
/*
 * Generate name tag.
 */
char *
gen_name_number(number)
	int number;
{
	static char buf[128];
	snprintf(buf, sizeof(buf), "<a name='%d'>", number);
	return buf;
}
/*
 * Generate name tag.
 */
char *
gen_name_string(name)
	const char *name;
{
	static char buf[128];
	snprintf(buf, sizeof(buf), "<a name='%s'>", name);
	return buf;
}
/*
 * Generate anchor begin tag.
 * (complete format)
 *
 *	i)	dir	directory
 *	i)	file	file
 *	i)	suffix	suffix
 *	i)	key	key
 *	i)	title	title='xxx'
 *	i)	target	target='xxx'
 *	r)		generated anchor tag
 */
char *
gen_href_begin_with_title_target(dir, file, suffix, key, title, target)
	const char *dir;
	const char *file;
	const char *suffix;
	const char *key;
	const char *title;
	const char *target;
{
	static STRBUF *sb = NULL;

	if (sb == NULL)
		sb = strbuf_open(0);
	else
		strbuf_reset(sb);
	/*
	 * Construct URL.
	 * href='dir/file.suffix#key'
	 */
	strbuf_puts(sb, "<a href='");
	if (file) {
		if (dir) {
			strbuf_puts(sb, dir);
			strbuf_putc(sb, '/');
		}
		strbuf_puts(sb, file);
		if (suffix) {
			strbuf_putc(sb, '.');
			strbuf_puts(sb, suffix);
		}
	}
	if (key) {
		strbuf_putc(sb, '#');
		strbuf_puts(sb, key);
	}
	strbuf_putc(sb, '\'');
	if (target)
		strbuf_sprintf(sb, " target='%s'", target);
	if (title)
		strbuf_sprintf(sb, " title='%s'", title);
	strbuf_putc(sb, '>');
	return strbuf_value(sb);
}
/*
 * Generate simple anchor begin tag.
 */
char *
gen_href_begin_simple(file)
	const char *file;
{
	return gen_href_begin_with_title_target(NULL, file, NULL, NULL, NULL, NULL);
}
/*
 * Generate anchor begin tag without title and target.
 */
char *
gen_href_begin(dir, file, suffix, key)
	const char *dir;
	const char *file;
	const char *suffix;
	const char *key;
{
	return gen_href_begin_with_title_target(dir, file, suffix, key, NULL, NULL);
}
/*
 * Generate anchor begin tag without target.
 */
char *
gen_href_begin_with_title(dir, file, suffix, key, title)
	const char *dir;
	const char *file;
	const char *suffix;
	const char *key;
	const char *title;
{
	return gen_href_begin_with_title_target(dir, file, suffix, key, title, NULL);
}
/*
 * Generate anchor end tag.
 */
char *
gen_href_end()
{
	return "</a>";
}
/*
 * Generate list begin tag.
 */
char *
gen_list_begin()
{
	static char buf[1024];

	if (table_list) {
		snprintf(buf, sizeof(buf), "%s\n%s%s%s%s",
			table_begin, 
			"<tr><th nowrap align='left'>tag</th>",
			"<th nowrap align='right'>line</th>",
			"<th nowrap align='center'>file</th>",
			"<th nowrap align='left'>source code</th></tr>");
	} else {
		strlimcpy(buf, verbatim_begin, sizeof(buf));
	}
	return buf;
}
/*
 * Generate list body.
 *
 * s must be choped.
 */
char *
gen_list_body(srcdir, string)
	char *srcdir;
	char *string;
{
	static STRBUF *sb = NULL;
	char *name, *lno, *filename, *line, *fid;
	char *p;
	SPLIT ptable;

	if (sb == NULL)
		sb = strbuf_open(0);
	else
		strbuf_reset(sb);
	if (split(string, 4, &ptable) < 4) {
		recover(&ptable);
		die("too small number of parts in list_body().\n'%s'", string);
	}
	name = ptable.part[0].start;
	lno = ptable.part[1].start;
	filename = ptable.part[2].start;
	line = ptable.part[3].start;
	filename += 2;				/* remove './' */
	fid = path2fid(filename);

	if (table_list) {
		strbuf_puts(sb, "<tr><td nowrap>");
		strbuf_puts(sb, gen_href_begin(srcdir, fid, HTML, lno));
		strbuf_puts(sb, name);
		strbuf_puts(sb, gen_href_end());
		strbuf_sprintf(sb, "</td><td nowrap align='right'>%s</td><td nowrap align='left'>%s</td><td nowrap>",
			lno, filename);

		for (p = line; *p; p++) {
			unsigned char c = *p;

			if (c == '&')
				strbuf_puts(sb, quote_amp);
			else if (c == '<')
				strbuf_puts(sb, quote_little);
			else if (c == '>')
				strbuf_puts(sb, quote_great);
			else if (c == ' ')
				strbuf_puts(sb, quote_space);
			else if (c == '\t') {
				strbuf_puts(sb, quote_space);
				strbuf_puts(sb, quote_space);
			} else
				strbuf_putc(sb, c);
		}
		strbuf_puts(sb, "</td></tr>");
		recover(&ptable);
	} else {
		int done = 0;

		strbuf_puts(sb, gen_href_begin(srcdir, fid, HTML, lno));
		strbuf_puts(sb, name);
		strbuf_puts(sb, gen_href_end());
		p = string + strlen(name);
		recover(&ptable);

		for (; *p; p++) {
			unsigned char c = *p;

			/* ignore "./" in path name */
			if (!done && c == '.' && *(p + 1) == '/') {
				p++;
				done = 1;
			} else if (c == '&')
				strbuf_puts(sb, quote_amp);
			else if (c == '<')
				strbuf_puts(sb, quote_little);
			else if (c == '>')
				strbuf_puts(sb, quote_great);
			else
				strbuf_putc(sb, c);
		}
	}
	p = strbuf_value(sb);
	return p;
}
/*
 * Generate list end tag.
 */
char *
gen_list_end()
{
	return table_list ? table_end : verbatim_end;
}

/*
 * Decide whether or not the path is binary file.
 *
 *	i)	path
 *	r)	0: is not binary, 1: is binary
 */
int
is_binary(path)
	char *path;
{
	int ip;
	char buf[32];
	int i, c, size;

	ip = open(path, 0);
	if (ip < 0)
		die("cannot open file '%s' in read mode.", path);
	size = read(ip, buf, sizeof(buf));
	close(ip);
	if (size < 0)
		return 1;
	if (!strncmp(buf, "!<arch>", 7))
		return 1;
	for (i = 0; i < size; i++) {
		c = (unsigned char)buf[i];
		if (c == 0 || c > 127)
			return 1;
	}
	return 0;
}
/*
 * Encode URL.
 *
 *	i)	url	URL
 *	r)		encoded URL
 */
char *
encode(url)
        char *url;
{
	static STRBUF *sb = NULL;
        char *p;

	if (sb)
		strbuf_reset(sb);
	else
		sb = strbuf_open(0);
        for (p = url; *p; p++) {
		int c = (unsigned char)*p;

                if (isalnum(c))
			strbuf_putc(sb, c);
		else
			strbuf_sprintf(sb, "%%%02x", c);
        }

	return strbuf_value(sb);
}
