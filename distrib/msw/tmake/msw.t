#!################################################################################
#! File:    msw.t
#! Purpose: tmake template file from which src/msw/files.lst containing the
#!          list of files for wxMSW library is generated by tmake
#! Author:  Vadim Zeitlin
#! Created: 28.01.00
#! Version: $Id$
#!################################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric, %wxHtml, %wxUNIX, %wxMSW, %wxMOTIF and
    #! %wxOS2PM hashes.
    IncludeTemplate("filelist.t");

    #! find all our sources
    $project{"COMMONOBJS"} .= "parser.o ";
    $project{"COMMONDEPS"} .= "parser.d ";

    foreach $file (sort keys %wxGeneric) {
        next if $wxGeneric{$file} =~ /\b(PS|G|U|16)\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"MSW_SOURCES"} .= "generic/" . $file . " ";
        $project{"GENERICOBJS"} .= $fileobj . " ";
        $project{"GENERICDEPS"} .= $filedep . " "
    }

    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} =~ /\bR\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"MSW_SOURCES"} .= "common/" . $file . " ";
        $project{"COMMONOBJS"} .= $fileobj . " ";
        $project{"COMMONDEPS"} .= $filedep . " "
    }

    foreach $file (sort keys %wxMSW) {
        next if $wxMSW{$file} =~ /\b16\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        if( $wxMSW{$file} =~ /\bO\b/ ) {
            $project{"MSW_SOURCES"} .= "msw/ole/" . $file . " ";
            $project{"OLEOBJS"} .= $fileobj . " ";
            $project{"OLEDEPS"} .= $filedep . " ";
        } else {
            $project{"MSW_SOURCES"} .= "msw/" . $file . " ";
            $project{"GUIOBJS"} .= $fileobj . " ";
            $project{"GUIDEPS"} .= $filedep . " ";
        }
    }

    foreach $file (sort keys %wxHTML) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"MSW_SOURCES"} .= "html/" . $file . " ";
        $project{"HTMLOBJS"} .= $fileobj . " ";
        $project{"HTMLDEPS"} .= $filedep . " "
    }
    #! find all our headers
    foreach $file (sort keys %wxWXINCLUDE) {
        $project{"MSW_HEADERS"} .= $file . " "
    }

    foreach $file (sort keys %wxMSWINCLUDE) {
        $project{"MSW_HEADERS"} .= "msw/" . $file . " "
    }

    foreach $file (sort keys %wxGENERICINCLUDE) {
        $project{"MSW_HEADERS"} .= "generic/" . $file . " "
    }

    foreach $file (sort keys %wxUNIXINCLUDE) {
        $project{"MSW_HEADERS"} .= "unix/" . $file . " "
    }

    foreach $file (sort keys %wxHTMLINCLUDE) {
        $project{"MSW_HEADERS"} .= "html/" . $file . " "
    }

    foreach $file (sort keys %wxPROTOCOLINCLUDE) {
        $project{"MSW_HEADERS"} .= "protocol/" . $file . " "
    }
#$}
# This file was automatically generated by tmake at #$ Now()
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE MSW.T!
ALL_SOURCES = \
		#$ ExpandList("MSW_SOURCES");

ALL_HEADERS = \
		#$ ExpandList("MSW_HEADERS");

COMMONOBJS = \
		#$ ExpandList("COMMONOBJS");

COMMONDEPS = \
		#$ ExpandList("COMMONDEPS");

GENERICOBJS = \
		#$ ExpandList("GENERICOBJS");

GENERICDEPS = \
		#$ ExpandList("GENERICDEPS");

GUIOBJS = \
		#$ ExpandList("GUIOBJS");

GUIDEPS = \
		#$ ExpandList("GUIDEPS");

UNIXOBJS = \
		#$ ExpandList("UNIXOBJS");

UNIXDEPS = \
		#$ ExpandList("UNIXDEPS");

HTMLOBJS = \
		#$ ExpandList("HTMLOBJS");

HTMLDEPS = \
		#$ ExpandList("HTMLDEPS");

OLEOBJS = \
		#$ ExpandList("OLEOBJS");

OLEDEPS = \
		#$ ExpandList("OLEDEPS");

IODBCOBJS = \
		#$ ExpandList("IODBCOBJS");

IODBCDEPS = \
		#$ ExpandList("IODBCDEPS");
