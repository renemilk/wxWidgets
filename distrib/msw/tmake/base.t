#!################################################################################
#! File:    base.t
#! Purpose: tmake template file from which src/common/files.lst containing the
#!          list of files for wxBase library is generated by tmake
#! Author:  Vadim Zeitlin
#! Created: 27.01.00
#! Version: $Id$
#!################################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric, %wxHtml, %wxUnix, %wxGTK, %wxMOTIF and
    #! %wxOS2PM hashes.
    IncludeTemplate("filelist.t");

    #! find all our sources
    foreach $file (sort keys %wxBase) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"BASE_SOURCES"} .= "common/" . $file . " ";
        $project{"BASE_OBJS"} .= $fileobj . " ";
        $project{"BASE_DEPS"} .= $filedep . " ";
    }

    foreach $file (sort keys %wxCommon) {
        next unless $wxCommon{$file} =~ /\bB\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"BASE_SOURCES"} .= "common/" . $file . " ";
        $project{"BASE_OBJS"} .= $fileobj . " ";
        $project{"BASE_DEPS"} .= $filedep . " ";
    }

    foreach $file (sort keys %wxUNIX) {
        next unless $wxUNIX{$file} =~ /\bB\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"BASE_SOURCES"} .= "unix/" . $file . " ";
        $project{"BASE_UNIX_OBJS"} .= $fileobj . " ";
        $project{"BASE_UNIX_DEPS"} .= $filedep . " ";
    }

    foreach $file (sort keys %wxMSW) {
        next unless $wxMSW{$file} =~ /\bB\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"BASE_SOURCES"} .= "msw/" . $file . " ";
        $project{"BASE_MSW_OBJS"} .= $fileobj . " ";
        $project{"BASE_MSW_DEPS"} .= $filedep . " ";
    }

    #! find all our headers
    foreach $file (sort keys %wxWXINCLUDE) {
        next unless $wxWXINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= $file . " "
    }

    foreach $file (sort keys %wxUNIXINCLUDE) {
        next unless $wxUNIXINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "unix/" . $file . " "
    }

    foreach $file (sort keys %wxMSWINCLUDE) {
        next unless $wxMSWINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "msw/" . $file . " "
    }

    foreach $file (sort keys %wxPROTOCOLINCLUDE) {
        next unless $wxPROTOCOLINCLUDE{$file} =~ /\bB\b/;

        $project{"BASE_HEADERS"} .= "protocol/" . $file . " "
    }
#$}
# This file was automatically generated by tmake at #$ Now()
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE BASE.T!
ALL_SOURCES = \
		#$ ExpandList("BASE_SOURCES");

ALL_HEADERS = \
		#$ ExpandList("BASE_HEADERS");

BASE_OBJS = \
		#$ ExpandList("BASE_OBJS");

BASE_DEPS = \
		#$ ExpandList("BASE_DEPS");

BASE_UNIX_OBJS = \
		#$ ExpandList("BASE_UNIX_OBJS");

BASE_UNIX_DEPS = \
		#$ ExpandList("BASE_UNIX_DEPS");

BASE_MSW_OBJS = \
		#$ ExpandList("BASE_MSW_OBJS");

BASE_MSW_DEPS = \
		#$ ExpandList("BASE_MSW_DEPS");

