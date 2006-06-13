#----------------------------------------------------------------------
# Name:        wx.tools.pywxrc
# Purpose:     XML resource compiler
#
# Author:      Robin Dunn
#              Based on wxrc.cpp by Vaclav Slavik, Eduardo Marques
#              Ported to Python in order to not require yet another
#              binary in wxPython distributions
#
#              Massive rework by Eli Golovinsky
#
# RCS-ID:      $Id$
# Copyright:   (c) 2004 by Total Control Software, 2000 Vaclav Slavik
# Licence:     wxWindows license
#----------------------------------------------------------------------

"""
pywxrc -- Python XML resource compiler
          (see http://wiki.wxpython.org/index.cgi/pywxrc for more info)

Usage: python pywxrc.py -h
       python pywxrc.py [-p] [-g] [-e] [-o filename] xrc input files... 
       
  -h, --help     show help message
  -p, --python   generate python module
  -g, --gettext  output list of translatable strings (may be combined with -p)
  -e, --embed    embed XRC resources in the output file
  -o, --output   output filename, or - for stdout
"""

import sys, os, getopt, glob, re
import xml.dom.minidom as minidom
import wx
import wx.xrc

#----------------------------------------------------------------------

class PythonTemplates:
    FILE_HEADER = """\
# This file was automatically generated by pywxrc, do not edit by hand.
# -*- coding: UTF-8 -*-

import wx
import wx.xrc as xrc

__res = None

def get_resources():
    \"\"\" This function provides access to the XML resources in this module.\"\"\"
    global __res
    if __res == None:
        __init_resources()
    return __res

"""

    CLASS_HEADER = """\
class xrc%(windowName)s(wx.%(windowClass)s):
    def PreCreate(self):
        \"\"\" This function is called during the class's initialization.
        
        Override it for custom setup before the window is created usually to
        set additional window styles using SetWindowStyle() and SetExtraStyle().\"\"\"
        pass

    def __init__(self, parent):
        # Two stage creation (see http://wiki.wxpython.org/index.cgi/TwoStageCreation)
        pre = wx.Pre%(windowClass)s()
        get_resources().LoadOn%(windowClass)s(pre, parent, "%(windowName)s")
        self.PreCreate()
        self.PostCreate(pre)

        # Define variables for the controls
"""

    CREATE_WIDGET_VAR = """\
        self.%(widgetName)s = xrc.XRCCTRL(self, \"%(widgetName)s\")
"""

    MENU_CLASS_HEADER = """\
class xrc%(windowName)s(wx.%(windowClass)s):
    def __init__(self):
        pre = get_resources().LoadMenu("%(windowName)s")

        # This is a copy of Robin's PostCreate voodoo magic in wx.Window that
        # relinks the self object with the menu object.
        self.this = pre.this
        self.thisown = pre.thisown
        pre.thisown = 0
        if hasattr(self, '_setOORInfo'):
            self._setOORInfo(self)
        if hasattr(self, '_setCallbackInfo'):
            self._setCallbackInfo(self, self.__class__)

        # Define variables for the menu items
"""

    CREATE_MENUITEM_VAR = """\
        self.%(widgetName)s = self.FindItemById(xrc.XRCID(\"%(widgetName)s\"))
"""

    INIT_RESOURE_HEADER = """\
# ------------------------ Resource data ----------------------

def __init_resources():
    global __res
    __res = xrc.EmptyXmlResource()
"""

    LOAD_RES_FILE = """\
    __res.Load('%(resourceFilename)s')"""

    FILE_AS_STRING = """\
    %(filename)s = '''\\
%(fileData)s'''

"""

    PREPARE_MEMFS = """\
    wx.FileSystem.AddHandler(wx.MemoryFSHandler())
"""

    ADD_FILE_TO_MEMFS = """\
    wx.MemoryFSHandler.AddFile('XRC/%(memoryPath)s/%(filename)s', %(filename)s)
"""

    LOAD_RES_MEMFS = """\
    __res.Load('memory:XRC/%(memoryPath)s/%(resourceFilename)s')
"""

    GETTEXT_DUMMY_FUNC = """
# ----------------------- Gettext strings ---------------------

def __gettext_strings():
    # This is a dummy function that lists all the strings that are used in
    # the XRC file in the _("a string") format to be recognized by GNU
    # gettext utilities (specificaly the xgettext utility) and the
    # mki18n.py script.  For more information see:
    # http://wiki.wxpython.org/index.cgi/Internationalization 
    
    def _(str): pass
    
%s
"""

#----------------------------------------------------------------------

class XmlResourceCompiler:
    
    templates = PythonTemplates()

    """This class generates Python code from XML resource files (XRC)."""

    def MakePythonModule(self, inputFiles, outputFilename,
                         embedResources=False, generateGetText=False):

        outputFile = self._OpenOutputFile(outputFilename)

        classes = []
        resources = []
        gettextStrings = []

        # process all the inputFiles, collecting the output data
        for inFile in inputFiles:
            resourceDocument = minidom.parse(inFile)
            classes.append(self.GenerateClasses(resourceDocument))

            if embedResources:
                res = self.GenerateInitResourcesEmbedded(inFile, resourceDocument)
            else:
                res = self.GenerateInitResourcesFile(inFile, resourceDocument)
            resources.append(res)

            if generateGetText:
                gettextStrings += self.FindStringsInNode(resourceDocument.firstChild)
                
        # now write it all out
        print >>outputFile, self.templates.FILE_HEADER

        # Note: Technically it is not legal to have anything other
        # than ascii for class and variable names, but since the user
        # can create the XML with non-ascii names we'll go ahead and
        # allow for it here, and then let Python complain about it
        # later when they try to run the program.
        classes = u"\n".join(classes)
        print >>outputFile, classes.encode("UTF-8")

        print >>outputFile, self.templates.INIT_RESOURE_HEADER
        if embedResources:
            print >>outputFile, self.templates.PREPARE_MEMFS
        resources = u"\n".join(resources)
        print >>outputFile, resources.encode("UTF-8")

        if generateGetText:
            # These have already been converted to utf-8...
            gettextStrings = ['    _("%s")' % s for s in gettextStrings]
            gettextStrings = "\n".join(gettextStrings)
            print >>outputFile, self.templates.GETTEXT_DUMMY_FUNC % gettextStrings

    #-------------------------------------------------------------------

    def MakeGetTextOutput(self, inputFiles, outputFilename):
        """
        Just output the gettext strings by themselves, with no other
        code generation.
        """
        outputFile = self._OpenOutputFile(outputFilename)
        for inFile in inputFiles:
            resourceDocument = minidom.parse(inFile)
            resource = resourceDocument.firstChild
            strings = self.FindStringsInNode(resource)
            strings = ['_("%s");' % s for s in strings]
            print >>outputFile, "\n".join(strings)

    #-------------------------------------------------------------------

    def GenerateClasses(self, resourceDocument):
        outputList = []
        
        resource = resourceDocument.firstChild
        topWindows = [e for e in resource.childNodes
                      if e.nodeType == e.ELEMENT_NODE and e.tagName == "object"]
        
        # Generate a class for each top-window object (Frame, Panel, Dialog, etc.)
        for topWindow in topWindows:
            windowClass = topWindow.getAttribute("class")
            windowClass = re.sub("^wx", "", windowClass)
            windowName = topWindow.getAttribute("name")
            
            if windowClass == "Menu":
                outputList.append(self.templates.MENU_CLASS_HEADER % locals())
            else:
                outputList.append(self.templates.CLASS_HEADER % locals())
            
            # Generate a variable for each control, and standard event handlers
            # for standard controls.
            for widget in topWindow.getElementsByTagName("object"):
                widgetClass = widget.getAttribute("class")
                widgetClass = re.sub("^wx", "", widgetClass)
                widgetName = widget.getAttribute("name")
                if (widgetName != "" and widgetClass != ""):
                    if widgetClass == "MenuItem":
                        outputList.append(self.templates.CREATE_MENUITEM_VAR % locals())
                    elif widgetClass not in \
                        ['tool', 'unknown', 'notebookpage', 'separator', 'sizeritem']:
                        outputList.append(self.templates.CREATE_WIDGET_VAR % locals())
            outputList.append('\n\n')
                    
        return "".join(outputList)

    #-------------------------------------------------------------------

    def GenerateInitResourcesEmbedded(self, resourceFilename, resourceDocument):
        outputList = []
        files = []

        resourcePath = os.path.split(resourceFilename)[0]
        memoryPath = self.GetMemoryFilename(os.path.splitext(os.path.split(resourceFilename)[1])[0])
        resourceFilename = self.GetMemoryFilename(os.path.split(resourceFilename)[1])
        
        self.ReplaceFilenamesInXRC(resourceDocument.firstChild, files, resourcePath)
        
        filename = resourceFilename
        fileData = resourceDocument.toxml() # what about this? encoding=resourceDocument.encoding)
        outputList.append(self.templates.FILE_AS_STRING % locals())

        for f in files:
            filename = self.GetMemoryFilename(f)
            fileData = self.FileToString(os.path.join(resourcePath, f))
            outputList.append(self.templates.FILE_AS_STRING % locals())

        for f in [resourceFilename] + files:
            filename = self.GetMemoryFilename(f)
            outputList.append(self.templates.ADD_FILE_TO_MEMFS % locals())
   
        outputList.append(self.templates.LOAD_RES_MEMFS % locals())
        
        return "".join(outputList)
        
    #-------------------------------------------------------------------

    def GenerateInitResourcesFile(self, resourceFilename, resourceDocument):
        # take only the filename portion out of resourceFilename
        resourceFilename = os.path.split(resourceFilename)[1]
        outputList = []
        outputList.append(self.templates.LOAD_RES_FILE % locals())
        return "".join(outputList)

    #-------------------------------------------------------------------

    def GetMemoryFilename(self, filename):
        # Remove special chars from the filename
        return re.sub(r"[^A-Za-z0-9_]", "_", filename)

    #-------------------------------------------------------------------

    def FileToString(self, filename):
        outputList = []
        
        buffer = open(filename, "rb").read()
        fileLen = len(buffer)

        linelng = 0
        for i in xrange(fileLen):
            s = buffer[i]
            c = ord(s)
            if s == '\n':
                tmp = s
                linelng = 0
            elif c < 32 or c > 127 or s == "'":
                tmp = "\\x%02x" % c
            elif s == "\\":
                tmp = "\\\\"            
            else:
                tmp = s

            if linelng > 70:
                linelng = 0
                outputList.append("\\\n")
            
            outputList.append(tmp)
            linelng += len(tmp)
            
        return "".join(outputList)
            
    #-------------------------------------------------------------------

    def NodeContainsFilename(self, node):
        """ Does 'node' contain filename information at all? """

        # Any bitmaps:
        if node.nodeName == "bitmap":
            return True

        if node.nodeName == "icon":
            return True

        # URLs in wxHtmlWindow:
        if node.nodeName == "url":
            return True

        # wxBitmapButton:
        parent = node.parentNode
        if parent.__class__ != minidom.Document and \
           parent.getAttribute("class") == "wxBitmapButton" and \
           (node.nodeName == "focus" or node.nodeName == "disabled" or
            node.nodeName == "selected"):
            return True

        # wxBitmap or wxIcon toplevel resources:
        if node.nodeName == "object":
            klass = node.getAttribute("class")
            if klass == "wxBitmap" or klass == "wxIcon":
                return True

        return False

    #-------------------------------------------------------------------

    def ReplaceFilenamesInXRC(self, node, files, resourcePath):
        """ Finds all files mentioned in resource file, e.g. <bitmap>filename</bitmap> 
        and replaces them with the memory filenames.
        
        Fills a list of the filenames found."""
        
        # Is 'node' XML node element?
        if node is None: return
        if node.nodeType != minidom.Document.ELEMENT_NODE: return

        containsFilename = self.NodeContainsFilename(node);

        for n in node.childNodes:

            if (containsFilename and
                (n.nodeType == minidom.Document.TEXT_NODE or
                 n.nodeType == minidom.Document.CDATA_SECTION_NODE)):
                
                filename = n.nodeValue
                memoryFilename = self.GetMemoryFilename(filename)
                n.nodeValue = memoryFilename

                if filename not in files:
                    files.append(filename)

            # Recurse into children
            if n.nodeType == minidom.Document.ELEMENT_NODE:
                self.ReplaceFilenamesInXRC(n, files, resourcePath);

    #-------------------------------------------------------------------

    def FindStringsInNode(self, parent):
        def is_number(st):
            try:
                i = int(st)
                return True
            except ValueError:
                return False
            
        strings = []
        if parent is None:
            return strings;

        for child in parent.childNodes:
            if ((parent.nodeType == parent.ELEMENT_NODE) and
                # parent is an element, i.e. has subnodes...
                (child.nodeType == child.TEXT_NODE or
                child.nodeType == child.CDATA_SECTION_NODE) and
                # ...it is textnode...
                (
                    parent.tagName == "label" or
                    (parent.tagName == "value" and
                                   not is_number(child.nodeValue)) or
                    parent.tagName == "help" or
                    parent.tagName == "longhelp" or
                    parent.tagName == "tooltip" or
                    parent.tagName == "htmlcode" or
                    parent.tagName == "title" or
                    parent.tagName == "item"
                )):
                # ...and known to contain translatable string
                if (parent.getAttribute("translate") != "0"):
                    strings.append(self.ConvertText(child.nodeValue))

            # subnodes:
            if child.nodeType == child.ELEMENT_NODE:
                strings += self.FindStringsInNode(child)

        return strings

    #-------------------------------------------------------------------

    def ConvertText(self, st):
        st2 = ""
        dt = list(st)

        skipNext = False
        for i in range(len(dt)):
            if skipNext:
                skipNext = False
                continue
            
            if dt[i] == '_':
                if dt[i+1] == '_':
                    st2 += '_'
                    skipNext = True
                else:
                    st2 += '&'
            elif dt[i] == '\n':
                st2 += '\\n'
            elif dt[i] == '\t':
                st2 += '\\t'
            elif dt[i] == '\r':
                st2 += '\\r'
            elif dt[i] == '\\':
                if dt[i+1] not in ['n', 't', 'r']:
                    st2 += '\\\\'
                else:
                    st2 += '\\'
            elif dt[i] == '"':
                st2 += '\\"'
            else:            
                st2 += dt[i]

        return st2.encode("UTF-8")                


    #-------------------------------------------------------------------

    def _OpenOutputFile(self, outputFilename):
        if outputFilename == "-":
            outputFile = sys.stdout
        else:
            try:
                outputFile = open(outputFilename, "wt")
            except IOError:
                raise IOError("Can't write output to '%s'" % outputFilename)
        return outputFile

    



#---------------------------------------------------------------------------

def main(args):
    resourceFilename = ""
    outputFilename = None
    embedResources = False
    generateGetText = False
    generatePython = False

    try:
        opts, args = getopt.gnu_getopt(args,
                                       "hpgeo:",
                                       "help python gettext embed output=".split())
    except getopt.GetoptError, e:
        print "\nError : %s\n" % str(e)
        print __doc__
        sys.exit(1)

    # If there is no input file argument, show help and exit
    if not args:
        print __doc__
        print "No xrc input file was specified."
        sys.exit(1)

    # Parse options and arguments
    for opt, val in opts:
        if opt in ["-h", "--help"]:
            print __doc__
            sys.exit(1)

        if opt in ["-p", "--python"]:
            generatePython = True

        if opt in ["-o", "--output"]:
            outputFilename = val
            
        if opt in ["-e", "--embed"]:
            embedResources = True

        if opt in ["-g", "--gettext"]:
            generateGetText = True


    # check for and expand any wildcards in the list of input files
    inputFiles = []
    for arg in args:
        inputFiles += glob.glob(arg)


    comp = XmlResourceCompiler()
    
    try:
        if generatePython:
            if not outputFilename:
                outputFilename = os.path.splitext(args[0])[0] + "_xrc.py"
            comp.MakePythonModule(inputFiles, outputFilename,
                                  embedResources, generateGetText)

        elif generateGetText:
            if not outputFilename:
                outputFilename = '-'
            comp.MakeGetTextOutput(inputFiles, outputFilename)

        else:
            print __doc__
            print "One or both of -p, -g must be specified."
            sys.exit(1)
            
            
    except IOError, e:
        print >>sys.stderr, "%s." % str(e)
    else:
        if outputFilename != "-":
            print >>sys.stderr, "Resources written to %s." % outputFilename

if __name__ == "__main__":
    main(sys.argv[1:])

