#!################################################################################
#! File:    gtk.t
#! Purpose: tmake template file from which src/gtk/files.lst containing the
#!          list of files for wxGTK library is generated by tmake
#! Author:  Vadim Zeitlin
#! Created: 28.01.00
#! Version: $Id$
#!################################################################################
#${
    #! include the code which parses filelist.txt file and initializes
    #! %wxCommon, %wxGeneric, %wxHtml, %wxUNIX, %wxGTK, %wxMOTIF and
    #! %wxOS2PM hashes.
    IncludeTemplate("filelist.t");

    #! find all our sources
    $project{"COMMONOBJS"} .= "parser.o ";
    $project{"COMMONDEPS"} .= "parser.d ";

    foreach $file (sort keys %wxGeneric) {
        next if $wxGeneric{$file} =~ /\bR\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"GTK_SOURCES"} .= "generic/" . $file . " ";
        $project{"GENERICOBJS"} .= $fileobj . " ";
        $project{"GENERICDEPS"} .= $filedep . " "
    }

    foreach $file (sort keys %wxCommon) {
        next if $wxCommon{$file} =~ /\bR\b/;

        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"GTK_SOURCES"} .= "common/" . $file . " ";
        $project{"COMMONOBJS"} .= $fileobj . " ";
        $project{"COMMONDEPS"} .= $filedep . " "
    }

    foreach $file (sort keys %wxGTK) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"GTK_SOURCES"} .= "gtk/" . $file . " ";
        $project{"GUIOBJS"} .= $fileobj . " ";
        $project{"GUIDEPS"} .= $filedep . " ";

        if ( $wxGTK{$file} =~ /\bL\b/ ) {
            $project{"GUI_LOWLEVEL_OBJS"} .= $fileobj . " ";
            $project{"GUI_LOWLEVEL_DEPS"} .= $filedep . " ";
        }
    }

    foreach $file (sort keys %wxUNIX) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"GTK_SOURCES"} .= "unix/" . $file . " ";
        $project{"UNIXOBJS"} .= $fileobj . " ";
        $project{"UNIXDEPS"} .= $filedep . " "
    }

    foreach $file (sort keys %wxHTML) {
        ($fileobj = $file) =~ s/cp?p?$/\o/;
        ($filedep = $file) =~ s/cp?p?$/\d/;

        $project{"GTK_SOURCES"} .= "html/" . $file . " ";
        $project{"HTMLOBJS"} .= $fileobj . " ";
        $project{"HTMLDEPS"} .= $filedep . " "
    }

    #! find all our headers
    foreach $file (sort keys %wxWXINCLUDE) {
        $project{"GTK_HEADERS"} .= $file . " "
    }

    foreach $file (sort keys %wxGTKINCLUDE) {
        $project{"GTK_HEADERS"} .= "gtk/" . $file . " "
    }

    foreach $file (sort keys %wxGENERICINCLUDE) {
        $project{"GTK_HEADERS"} .= "generic/" . $file . " "
    }

    foreach $file (sort keys %wxUNIXINCLUDE) {
        $project{"GTK_HEADERS"} .= "unix/" . $file . " "
    }

    foreach $file (sort keys %wxHTMLINCLUDE) {
        $project{"GTK_HEADERS"} .= "html/" . $file . " "
    }

    foreach $file (sort keys %wxPROTOCOLINCLUDE) {
        $project{"GTK_HEADERS"} .= "protocol/" . $file . " "
    }
#$}
# This file was automatically generated by tmake at #$ Now()
# DO NOT CHANGE THIS FILE, YOUR CHANGES WILL BE LOST! CHANGE GTK.T!
ALL_SOURCES = \
		#$ ExpandList("GTK_SOURCES");

ALL_HEADERS = \
		#$ ExpandList("GTK_HEADERS");

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

GUI_LOWLEVEL_OBJS = \
		#$ ExpandList("GUI_LOWLEVEL_OBJS");

GUI_LOWLEVEL_DEPS = \
		#$ ExpandList("GUI_LOWLEVEL_DEPS");

UNIXOBJS = \
		#$ ExpandList("UNIXOBJS");

UNIXDEPS = \
		#$ ExpandList("UNIXDEPS");

HTMLOBJS = \
		#$ ExpandList("HTMLOBJS");

HTMLDEPS = \
		#$ ExpandList("HTMLDEPS");

