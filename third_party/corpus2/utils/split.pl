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

#Process all xml files in SOURCE_DIR assuming that it contains files which consist of ccl+rel part.
#Separate each file into ccl-file and rel-file and place them into TARGET_DIR 
#(don't empty the TARGET_DIR, just add new files; it's up to the user to empty the TARGET_DIR)

use File::Find;
use Cwd 'abs_path';

if( -d $ARGV[0] and -d $ARGV[1]){
$srcdir = abs_path($ARGV[0]);
$targetdir = abs_path($ARGV[1]);

find(\&wanted, $srcdir);

sub wanted{

	if(/.*xml/){
		$filename = $_;
		$cclFilename = "$targetdir/ccl-".$filename;
		$relFilename = "$targetdir/rel-".$filename;
		$F = "filehandle";
		@cclPart = ();
		@relPart = ();
		$isCclPart = 1;
		sysopen(F, $filename, "r");
		
		while(<F>){
			if($isCclPart){
				if( $_ =~ /.*<relations>.*/){
					push(@relPart, $_);
					$isCclPart = 0;
				}else{
					push(@cclPart, $_);
				}
			}else{
				if($_ =~ /.*<\/chunkList>.*/){
					push(@cclPart, $_);
				}else{
					push(@relPart, $_);
				}
			}
		}

		close(F);
	
		$CCL_FILE = "cclHandle";
		$REL_FILE = "relHandle";	
		open CCL_FILE, ">$cclFilename" or die $!."dir:".$cclFilename;
		open REL_FILE, ">$relFilename" or die $!."dir:".$relFilename;
		print CCL_FILE "@cclPart";
		print REL_FILE "@relPart";
		close(CCL_FILE);
		close(REL_FILE);

	}
	

}
}else{
print "Usage: ./split.pl SOURCE_DIR TARGET_DIR\n";
}
