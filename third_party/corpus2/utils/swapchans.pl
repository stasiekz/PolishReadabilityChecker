#!/usr/bin/perl

# Copyright (C) 2012 Paweł Orłowicz.
# This program is free software; you can redistribute and/or modify it
# under the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.
#
# See the LICENCE and COPYING files for more details

#Script takes two arguments: corpus directory (DIR) and relation name (REL) that is going to be swapped.  
#In all files within DIR swap direction of all relations with name REL by swapping chan attributes of nodes: <from> and <to>  and it's text nodes
#e.g. 
#<rel name="cop">
#	<from chan="NP">3</from>
#	<to chan="VP">1</to>
#</rel>
# swap: NP with VP and 3 with 1:
#<rel name="cop">
#	<from chan="VP">1</from>
#	<to chan="NP">3</to>
#</rel>

use File::Find;

if ( @ARGV == 2 ){

$dir = $ARGV[0];
$relname = $ARGV[1];


find(\&wanted, $dir);

sub wanted{
	my $F;
	$filename = $_;
	my @file;
	if (/.*xml/){
		sysopen(F, $filename, "r");
		$isRel = 0;
		my $prevLine;
		my $nextLine;
		while(<F>){
			push(@file, $_);
			if( /name="$relname"/ ){
				$isRel = 1;
			}elsif( $isRel and /<\/rel>/){
				$isRel = $isRel + 1;
			}

			if( $isRel == 2){
				$isRel = 0;
				$last = $#file;
				$prevLine = $file[$last-2];
				$nextLine = $file[$last-1];
	
				$prevLine =~ /(chan=".*">[0-9]+<)/;
				$prevChan = $1;
				$nextLine =~ /(chan=".*">[0-9]+<)/;
				$nextChan = $1;

				$prevLine =~ s/chan=".*">[0-9]+</$nextChan/;
				$nextLine =~ s/chan=".*">[0-9]+</$prevChan/;

				$file[$last-2] = $prevLine;
				$file[$last-1] = $nextLine;
			}
		}
		close(F);
		open F, ">$filename";
		print F "@file";

	}

}

}else{
	print "In given directory (DIR) change direction of all relations with name REL by swapping attribute chan between <from> and <to> nodes";
	print "\n\nUSAGE: swapchans.pl DIR REL\n";
}
