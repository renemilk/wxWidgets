#!################################################################################
#! File:    motif.t
#! Purpose: tmake template file from which src/motif/files.lst containing the
#!          list of files for wxMotif library is generated by tmake
#! Author:  Vadim Zeitlin
#! Created: 28.01.00
#! Version: $Id$
#!################################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric, %wxHtml, %wxUNIX, %wxMOTIF, %wxMOTIF and
    #! %wxOS2PM hashes.
    IncludeTemplate("filelist.t");

    #! find all our sources
    $project{"COMMONOBJS"} .= "parser.o ";
    $project{"GUIOBJS"} .= "xmcombo.o ";

    foreach $file (sort keys %wxGeneric) {
        next if $wxGeneric{$file} =~ /\bX\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;

        $project{"MOTIF_SOURCES"} .= "generic/" . $file . " ";
        $project{"GENERICOBJS"} .= $fileobj . " ";
    }

    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} =~ /\bX\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;

        $project{"MOTIF_SOURCES"} .= "common/" . $file . " ";
        $project{"COMMONOBJS"} .= $fileobj . " ";
    }

    foreach $file (sort keys %wxMOTIF) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;

        $project{"MOTIF_SOURCES"} .= "motif/" . $file . " ";
        $project{"GUIOBJS"} .= $fileobj . " ";
    }

    foreach $file (sort keys %wxUNIX) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;

        $project{"MOTIF_SOURCES"} .= "unix/" . $file . " ";
        $project{"UNIXOBJS"} .= $fileobj . " ";
    }

    foreach $file (sort keys %wxHTML) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;

        $project{"MOTIF_SOURCES"} .= "html/" . $file . " ";
        $project{"HTMLOBJS"} .= $fileobj . " ";
    }
    #! find all our headers
    foreach $file (sort keys %wxWXINCLUDE) {
        next if $wxWXINCLUDE{$file} =~ /\bX\b/;

        $project{"MOTIF_HEADERS"} .= $file . " "
    }

    foreach $file (sort keys %wxMOTIFINCLUDE) {
        $project{"MOTIF_HEADERS"} .= "motif/" . $file . " "
    }

    foreach $file (sort keys %wxGENERICINCLUDE) {
        $project{"MOTIF_HEADERS"} .= "generic/" . $file . " "
    }

    foreach $file (sort keys %wxUNIXINCLUDE) {
        $project{"MOTIF_HEADERS"} .= "unix/" . $file . " "
    }

    foreach $file (sort keys %wxHTMLINCLUDE) {
        $project{"MOTIF_HEADERS"} .= "html/" . $file . " "
    }

    foreach $file (sort keys %wxPROTOCOLINCLUDE) {
        $project{"MOTIF_HEADERS"} .= "protocol/" . $file . " "
    }
#$}
# This file was automatically generated by tmake at #$ Now()
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE MOTIF.T!
ALL_SOURCES = \
		#$ ExpandList("MOTIF_SOURCES");

ALL_HEADERS = \
		#$ ExpandList("MOTIF_HEADERS");

COMMONOBJS = \
		#$ ExpandList("COMMONOBJS");

GENERICOBJS = \
		#$ ExpandList("GENERICOBJS");

GUIOBJS = \
		#$ ExpandList("GUIOBJS");

UNIXOBJS = \
		#$ ExpandList("UNIXOBJS");

HTMLOBJS = \
		#$ ExpandList("HTMLOBJS");

