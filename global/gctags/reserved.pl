#!/usr/bin/perl
#
# Copyright (c) 2003 Tama Communications Corporation
#
# This file is part of GNU GLOBAL.
#
# GNU GLOBAL is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# GNU GLOBAL is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#
# This command does one of the followings:
#
# [1] Generate option string for gperf(1).
# [2] Generate regular expression which means reserved words for htags(1).
# [3] Generate gperf(1) source from keyword file.
#
# Both output is stdout.
#
$com = $0;
$com =~ s/.*\///;

#
# Allocation of ID
#
# 1001 - 2000: Reserved variable (PHP)
# 2001 -; Reserved word (wide sense)
#	2001 - 3000: reserved word
#	3001 - 4000: # macro
#	4001 - 5000: yacc word
#
$START_VARIABLE = 1001;
$START_WORD = 2001;
$START_SHARP = 3001;
$START_YACC = 4001;

sub usage {
	print STDERR "usage: $com --prefix=prefix --option\n";
	print STDERR "       $com --prefix=prefix --perl keyword_file\n";
	print STDERR "       $com --prefix=prefix keyword_file\n";
	exit(1);
}

$slot_name = 'name';
$option = 0;
$perl = 0;
$prefix = '';
$keyword_file = '';
while ($ARGV[0] =~ /^-/) {
	$opt = shift;
	if ($opt =~ /^--prefix=(.*)$/) {
		$prefix = $1;
	} elsif ($opt =~ /^--option$/) {
		$option = 1;
	} elsif ($opt =~ /^--perl$/) {
		$perl = 1;
	} else {
		usage();
	}
}
$keyword_file = $ARGV[0];
if (!$prefix) {
	usage();
}
#
# [1] Generate option string for gperf(1).
#
if ($option) {
	#
	# This value should be maintained according to the value of
	# the reserved words.
	#
	if ($prefix eq 'php') {
		print "--key-positions=1-2,4-6,9,\$\n";
	} else {
		print "--key-positions=1-3,6,\$\n";
	}
	print "--language=C\n";
	print "--struct-type\n";
	print "--slot-name=${slot_name}\n";
	print "--hash-fn-name=${prefix}_hash\n";
	print "--lookup-fn-name=${prefix}_lookup\n";
	exit(0);
}
if (!$keyword_file) {
	usage();
}
#
# [2] Generate regular expression which means reserved words for htags(1).
#
if ($perl) {
	open(IP, $keyword_file) || die("$com: cannot open file '$keyword_file'.\n");
	print "# This part is generated automatically by $com from '$keyword_file'.\n";
	print "\$'${prefix}_reserved_words = \"(";
	$sep = '';
	while(<IP>) {
		chop;
		next if (/^$/ || /^;/);
		($id, $type) = split;
		@id = split(/,/, $id);
		if ($type eq 'word') {
			for ($i = 0; $i < @id; $i++) {
				print $sep, $id[$i];
				$sep = '|';
			}
		}
	}
	print ")\";\n";
	print "# end of generated part.\n";
	close(IP);
	exit(0);
}
#
# [3] Generate gperf(1) source from keyword file.
#
$PRE = $pre = $prefix;
$PRE =~ tr/a-z/A-Z/;
$pre =~ tr/A-Z/a-z/;
local(%yacctab) = ('%%', 'SEP', '%{', 'BEGIN', '%}', 'END');

#
# Macro definitions.
#
$n_variable = $START_VARIABLE;
$n_word = $START_WORD;
$n_sharp = $START_SHARP;
$n_yacc = $START_YACC;

open(IP, $keyword_file) || die("$com: cannot open file '$keyword_file'.\n");
print "%{\n";
print "#define START_VARIABLE\t$n_variable\n";
print "#define START_WORD\t$n_word\n";
print "#define START_SHARP\t$n_sharp\n";
print "#define START_YACC\t$n_yacc\n";
print "#define IS_RESERVED(a)	((a) >= START_WORD)\n";
print "\n";
while(<IP>) {
	chop;
	next if (/^$/ || /^;/);
	($id, $type) = split;
	@id = split(/,/, $id);
	$upper = $id[0];
	$upper =~ tr/a-z/A-Z/;
	if ($type eq 'word') {
		print "#define ${PRE}_${upper}\t${n_word}\n";
		$n_word++;
	} elsif ($type eq 'variable') {
		print "#define ${PRE}_${upper}\t${n_variable}\n";
		$n_variable++;
	} elsif ($type eq 'sharp') {
		$upper =~ s/##/SHARP/;
		$upper =~ s/#//g;
		print "#define SHARP_${upper}\t${n_sharp}\n";
		$n_sharp++;
	} elsif ($type eq 'yacc') {
		$upper =~ s/(%%|%{|%})/$yacctab{$1}/ge;
		$upper =~ s/%//g;
		print "#define YACC_${upper}\t${n_yacc}\n";
		$n_yacc++;
	}
}
close(IP);
print "%}\n";
#
# Structure definition.
#
print "struct keyword { char *${slot_name}; int token; }\n";
print "%%\n";
#
# Keyword definitions.
#
open(IP, $keyword_file) || die("$com: cannot open file '$keyword_file'.\n");
while(<IP>) {
	chop;
	next if (/^$/ || /^;/);
	($id, $type) = split;
	@id = split(/,/, $id);
	$upper = $id[0];
	$upper =~ tr/a-z/A-Z/;
	for ($i = 0; $i < @id; $i++) {
		$name = $id[$i];
		if ($type eq 'word') {
			print "$name, ${PRE}_${upper}\n";
		} elsif ($type eq 'variable') {
			print "\"${name}\", ${PRE}_${upper}\n";
		} elsif ($type eq 'sharp') {
			$upper =~ s/##/SHARP/;
			$upper =~ s/#//g;
			print "\"${name}\", SHARP_${upper}\n";
		} elsif ($type eq 'yacc') {
			$upper =~ s/(%%|%{|%})/$yacctab{$1}/ge;
			$upper =~ s/%//g;
			print "\"${name}\", YACC_${upper}\n";
		}
	}
}
close(IP);
print "%%\n";
print "static int reserved_word(const char *, int);\n";
print "static int\n";
print "reserved_word(str, len)\n";
print "const char *str;\n";
print "int len;\n";
print "{\n";
print "\tstruct keyword *keyword = ${pre}_lookup(str, len);\n";
print "\tint n = keyword ? keyword->token : 0;\n";
print "\treturn IS_RESERVED(n) ? n : 0;\n";
print "}\n";
if ($n_variable > $START_VARIABLE) {
	print "static int reserved_variable(const char *, int);\n";
	print "static int\n";
	print "reserved_variable(str, len)\n";
	print "const char *str;\n";
	print "int len;\n";
	print "{\n";
	print "\tstruct keyword *keyword = ${pre}_lookup(str, len);\n";
	print "\tint n = keyword ? keyword->token : 0;\n";
	print "\treturn (n >= START_VARIABLE && n < START_WORD) ? n : 0;\n";
	print "}\n";
}
exit 0;
