#!/usr/bin/env python
#---------------------------------------------------------------------------
"""
Usage:  build_renamers.py filename.i

Run SWIG on file.i using the XML language module and then scan the XML
file produced and generate the %rename directives needed to implement
the new wx namespace.  The rename directives are output in a file
named _filename_rename.i in the same dir as filename.i.

Also output a reverse 'renamer' Python module located in
wxPython/filename.py (relative the the current dir) to make a
backwards compatibility interface for the old wxPython packages.
"""

import sys, os, tempfile, pprint
import xml.sax
from distutils.spawn import spawn



#---------------------------------------------------------------------------

DO_UNLINK = True

wxPythonDir = "wxPython"
swig_cmd = "/opt/swig/bin/swig"
if os.name == 'nt':
    swig_cmd = 'e:/projects/SWIG-cvs/swig.exe'

swig_args = ['-c++',
             '-Wall',
             '-nodefault',

             '-xml',
             '-xmllite',

             '-I./src',
             '-c'
             ]


renamerTemplateStart = """\
// A bunch of %%rename directives generated by %s
// in order to remove the wx prefix from all global scope names.

#ifndef SWIGXML

"""

renamerTemplateEnd = """
#endif
"""

wxPythonTemplateStart = """\
## This file reverse renames symbols in the wx package to give
## them their wx prefix again, for backwards compatibility.
##
## Generated by %s

# This silly stuff here is so the wxPython.wx module doesn't conflict
# with the wx package.  We need to import modules from the wx package
# here, then we'll put the entry back in sys.modules.
import sys
_wx = None
if sys.modules.has_key('wxPython.wx'):
    _wx = sys.modules['wxPython.wx']
    del sys.modules['wxPython.wx']

import wx.%s

sys.modules['wxPython.wx']
del sys, _wx


# Now assign all the reverse-renamed names:
"""

wxPythonTemplateEnd = """

"""



#---------------------------------------------------------------------------

def main(args):
    # check args
    if len(args) < 1:
        print __doc__
        sys.exit(1)

    # check location (there should be a wxPython subdir)
    if not os.path.exists(wxPythonDir) or not os.path.isdir(wxPythonDir):
        print __doc__
        print "You should only run this script from the main wxPython source dir.\n"
        sys.exit(1)
        

    source = args[0]
    sourcePath, sourceBase = os.path.split(source)
    sourceBase = os.path.splitext(sourceBase)[0]

    tempfile.tempdir = sourcePath
    xmlDest = tempfile.mktemp('.xml')
    swigDest = os.path.join(sourcePath, "_"+sourceBase+"_rename.i")
    pyDest = os.path.join(wxPythonDir, sourceBase + '.py')

    #print "source:      ", source
    #print "xmlDest:     ", xmlDest
    #print "swigDest:    ", swigDest
    #print "pyDest:      ", pyDest

    cmd = [ swig_cmd ] + swig_args + args[1:] + ['-I'+sourcePath, '-o', xmlDest, source]
    print ' '.join(cmd)
    spawn(cmd)

    swigDestTemp = tempfile.mktemp('.tmp')
    swigFile = open(swigDestTemp, "w")
    swigFile.write(renamerTemplateStart % sys.argv[0])

    pyFile = open(pyDest, "w")
    pyFile.write(wxPythonTemplateStart % (sys.argv[0], sourceBase))
    
    print "Parsing and building renamers", 
    try:
##         try:
##             import libxml2
##             print "using libxml2..."
##             ctxt = libxml2.createPushParser(ContentHandler(source, sourceBase, swigFile, pyFile),
##                                             '', 0, xmlDest)
##             for line in file(xmlDest):
##                 if not line:
##                     ctxt.parseChunck('', 0, 1)
##                     break
##                 ctxt.parseChunk(line, len(line), 0)
                
##         except ImportError:
        print "using xml.sax..."
        xml.sax.parse(xmlDest, ContentHandler(source, sourceBase, swigFile, pyFile))

    finally:
        checkOtherNames(pyFile, sourceBase,
                        os.path.join(sourcePath, '_'+sourceBase+'_reverse.txt'))
        pyFile.write(wxPythonTemplateEnd)        
        pyFile.close()

        swigFile.write(renamerTemplateEnd)
        swigFile.close()

        # Compare the file just created with the existing one and
        # blow away the old one if they are different.
        if open(swigDest).read() != open(swigDestTemp).read():
            os.unlink(swigDest)
            os.rename(swigDestTemp, swigDest)
        else:
            print swigDest + " not changed."
            os.unlink(swigDestTemp)

        if DO_UNLINK:
            os.unlink(xmlDest)


#---------------------------------------------------------------------------

def checkOtherNames(pyFile, moduleName, filename):
    if os.path.exists(filename):
        prefixes = []
        for line in file(filename):
            if line.endswith('\n'):
                line = line[:-1]
            if line and not line.startswith('#'):
                if line.endswith('*'):
                    prefixes.append(line[:-1])
                else:
                    wxname = 'wx' + line
                    if line.startswith('wx') or line.startswith('WX') or line.startswith('EVT'):
                        wxname = line
                    pyFile.write("%s = wx.%s.%s\n" % (wxname, moduleName, line))

        if prefixes:
            pyFile.write(
                "\n\nd = globals()\nfor k, v in wx.%s.__dict__.iteritems():"
                % moduleName)
            first = True
            for p in prefixes:
                if first:
                    pyFile.write("\n    if ")
                    first = False
                else:
                    pyFile.write("\n    elif ")
                pyFile.write("k.startswith('%s'):\n        d[k] = v" % p)
            pyFile.write("\ndel d, k, v\n\n")

                        
#---------------------------------------------------------------------------

interestingTypes = [ 'class', 'cdecl', 'enumitem', 'constructor', 'constant' ]
interestingAttrs = [ 'name', 'sym_name', 'decl', 'feature_immutable', 'module',
                     'storage', 'type' ]


class Element:
    def __init__(self, tagtype):
        self.tagtype   = tagtype
        self.level     = -1
        self.name      = None
        self.sym_name  = None
        self.decl      = None
        self.immutable = None
        self.klass     = None
        self.module    = None
        self.storage   = None
        self.type      = None
        self.startLine = -1


    def write(self, moduleName, swigFile, pyFile):
        doRename = False
        addWX = False
        revOnly = False

        #if self.name.find('DefaultPosition') != -1:
        #    pprint.pprint(self.__dict__)
       
        if self.tagtype in  ['cdecl', 'constant']:
            if self.storage == 'typedef':
                pass
            
            # top level functions
            elif self.level == 0 and self.decl != "":
                doRename = True

            # top level global vars
            elif self.level == 0 and self.immutable == '1':
                doRename = True

            # static methods
            elif self.storage == 'static':
                if not self.klass:
                    pprint.pprint(self.__dict__)
                else:
                    self.name     = self.klass + '_' + self.name
                    self.sym_name = self.sym_klass + '_' + self.sym_name
                    # only output the reverse renamer in this case
                    doRename = revOnly = True

                            

            if doRename and self.name != self.sym_name:
                #print "%-25s %-25s" % (self.name, self.sym_name)
                self.name = self.sym_name
                addWX = True
                

        elif self.tagtype == 'class' and self.module == moduleName:
            doRename = True
            if self.sym_name != self.klass:
                #print self.sym_name
                self.name = self.sym_name
                addWX = True

        elif self.tagtype == 'constructor':
            #print "%-25s  %-25s" % (self.name, self.sym_name)
            if self.sym_name != self.klass:
                #print self.sym_name
                self.name = self.sym_name
                addWX = True
                doRename = True

        elif self.tagtype == 'enumitem' and self.level == 0:
            doRename = True


        if doRename:
            #print "%-25s %-25s" % (self.name, self.sym_name)
            old = new = self.name
            if old.startswith('wx') and not old.startswith('wxEVT_'):
                # remove all wx prefixes except wxEVT_ and write a %rename directive for it
                new = old[2:]
                if not revOnly:
                    swigFile.write("%%rename(%s)  %35s;\n" % (new, old))

            # Write assignments to import into the old wxPython namespace
            if addWX and not old.startswith('wx'):
                old = 'wx'+old
            pyFile.write("%s = wx.%s.%s\n" % (old, moduleName, new))
            
            
        #else:
        #    text = "%07d  %d  %10s  %-35s %s\n" % (
        #        self.startLine, self.level, self.tagtype, self.name, self.decl)
        #    #rejects.write(text)
        #    print text,


#---------------------------------------------------------------------------

class ContentHandler(xml.sax.ContentHandler):
    def __init__(self, source, sourceBase, swigFile, pyFile):
        xml.sax.ContentHandler.__init__(self)
        self.source = source
        self.sourceBase = sourceBase
        self.swigFile = swigFile
        self.pyFile   = pyFile
        self.elements = []
        self.imports  = 0
        self.klass = None
        self.sym_klass = None


    def setDocumentLocator(self, locator):
        self.locator = locator



    def startElement(self, name, attrs):
        if name in interestingTypes:
            # start of a new element that we are interested in
            ce = Element(name)
            ce.startLine = self.locator.getLineNumber()
            ce.level = len(self.elements)
            if name == 'constructor':
                ce.klass = self.elements[0].name
            else:
                ce.klass = self.klass
                ce.sym_klass = self.sym_klass
            self.elements.insert(0, ce)


        elif len(self.elements) and name == 'attribute' and attrs['name'] in interestingAttrs:
            attrName = attrs['name']
            attrVal  = attrs['value']
            if attrName.startswith('feature_'):
                attrName = attrName.replace('feature_', '')
            ce = self.elements[0]
            if getattr(ce, attrName) is None:
                setattr(ce, attrName, attrVal)
            if ce.tagtype == 'class' and attrName == 'name' and self.klass is None:
                self.klass = attrVal
            if ce.tagtype == 'class' and attrName == 'sym_name' and self.sym_klass is None:
                self.sym_klass = attrVal
            

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'name':
##             # save the elements name
##             ce = self.elements[0]
##             if ce.name is None:
##                 ce.name = attrs['value']
##                 ce.nameLine = self.locator.getLineNumber()

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'sym_name':
##             # save the elements name
##             ce = self.elements[0]
##             if ce.sym_name is None:
##                 ce.sym_name = attrs['value']

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'decl':
##             # save the elements decl
##             ce = self.elements[0]
##             ce.decl = attrs['value']

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'feature_immutable':
##             # save the elements decl
##             ce = self.elements[0]
##             ce.immutable = int(attrs['value'])

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'module':
##             # save the elements decl
##             ce = self.elements[0]
##             ce.module = attrs['value']

        elif name == 'import':
            self.imports += 1

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'storage':
##             # save the elements decl
##             ce = self.elements[0]
##             ce.storage = attrs['value']

##         elif len(self.elements) and name == 'attribute' and attrs['name'] == 'type':
##             # save the elements decl
##             ce = self.elements[0]
##             ce.type = attrs['value']


    def endElement(self, name):
        if name in interestingTypes:
            # end of an element that we are interested in
            ce = self.elements.pop(0)
            
            if self.imports == 0:
                # only write for items that are in this file, not imported
                ce.write(self.sourceBase, self.swigFile, self.pyFile)
            
        if name == 'import':
            self.imports -= 1

        if name == 'class':
            self.klass = None
            self.sym_klass = None
            

#---------------------------------------------------------------------------

if __name__ == "__main__":
    main(sys.argv[1:])

