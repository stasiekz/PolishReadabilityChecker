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

#Assume that SOURCE_DIR contains pairs of files ccl-NAME.xml.* + rel-NAME.xml
#According to the measure: option -n NUMBER_OF_DOCUMENTS_TO_HOLD_OUT or RATIO percentage of all documents to be hold out
#copy appropriate number of documents (1 document == 1 pair (ccl-NAME.xml.* + rel-NAME.xml) into HELD_OUT_PART_DIR
#and all the rest into REST_PART_DIR

use Getopt::Std;
use File::Find;
use File::Basename;
use File::Copy;
use Cwd 'abs_path';

getopt('n');

$source_dir = $ARGV[0];
$target_dir = $ARGV[1];
$rest_dir = $ARGV[2];
$ratio = $ARGV[3];

if( $ratio > 1 or $ratio < 0 and $opt_n eq ""){
print "USAGE: Ratio denotes fraction of documents for held_out.It is the value between 0 and 1.\nYou can also use option -n to determine the exact number of documents to hold out.";
}else{

	if( -d $source_dir and -d $target_dir and -d $rest_dir){
		opendir(DIR, $source_dir) or die $!;
		%cclfiles;
		%relfiles;
		while($filename = readdir(DIR)){
			$filename = abs_path("$source_dir/$filename");
			#common part of filename as key
			#full filename (with absolute path) as value
			if($filename =~ /(.*\/ccl-(.+)\.xml.+)/){
				$cclfiles{$2} = $1;
			}elsif($filename =~ /(.*\/rel-(.+)\.xml)/){
				$relfiles{$2} = $1;
			}
		}
		close(DIR);
		delete($cclfiles{""});
		delete($relfiles{""});
		%picker;
		$i = 0;
		#numerate hashes' keys for random selection by indices
		while( ($k, $v) = each %relfiles){
			$picker{$i} = $k;
			$i++;	
		}
		$size_of_files = scalar(keys %cclfiles);
		if($opt_n ne ""){
			if($opt_n > $size_of_files){
				die "ERROR: You want to hold out more files than are available.\n";
			}
			$ratio = $opt_n;
		}else{
			$ratio = $size_of_files * $ratio;
		}
		
		for($n = 0; $n < $ratio; ){
			$index = int( rand($size_of_files));
			if($picker{$index} eq ""){
				next;	
			}
			#CCL part
			$from = "$cclfiles{$picker{$index}}";
			$base = basename($from);
			$to = "$target_dir/$base";
			$mv1 = copy($from, $to);

			#REL part
			$from = "$relfiles{$picker{$index}}";
			$base = basename($from);
			$to = "$target_dir/$base";
			$mv2 = copy($from, $to);
			
			if ($mv1 and $mv2){
				$picker{$index} = "";
				$n++;
			}elsif($mv1 or $mv2){
				die "Error! Only one file of ccl+rel pair file was moved!\n";
			}
		}	
		for($n = 0; $n < $size_of_files; $n++){
			if($picker{$n} eq ""){
				next;	
			}
			#CCL part
			$from = "$cclfiles{$picker{$n}}";
			$base = basename($cclfiles{$picker{$n}});
			$to = "$rest_dir/$base";
			$mv1 = copy($from, $to);

			#REL part
			$from = "$relfiles{$picker{$n}}";
			$base = basename($relfiles{$picker{$n}});
			$to = "$rest_dir/$base";
			$mv2 = copy($from, $to);
		}	
	}else{
		print "USAGE: ./get_held_out [-n NUMBER_OF_DOCUMENTS] SOURCE_DIR HELD_OUT_PART_DIR REST_PART_DIR [RATIO]\n";
	}
}


