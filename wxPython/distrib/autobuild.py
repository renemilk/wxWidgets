

import sys, os, string, time
from ftplib import FTP

cwd = os.getcwd()

logfile = 'c:\\temp\\autobuild.log'
WXDIR   = os.environ['WXWIN']
dllVer  = '21_15'
wxpVer  = '2.1.15'
dateSt  = time.strftime("%Y%m%d", time.localtime(time.time()))

#----------------------------------------------------------------------

def do(cmd):
    st = "  " + cmd + " >> " + logfile
    print st
    f = open(logfile, "at")
    f.write(st + '\n')
    f.close()
    os.system(cmd + " >>& " + logfile)

#----------------------------------------------------------------------

def logTruncate():
    f = open(logfile, "wt")
    f.close()


def logSeparator(msg=None, f=None, recurse=1):
    if not f:
        f = open(logfile, "at")
    f.write('\n')
    f.write('--' * 35)
    f.write('\n')
    if msg:
        f.write(msg)
        f.write('\n')
        f.write('--' * 35)
        f.write('\n')
        if recurse:
            logSeparator(msg, sys.stdout, 0)

#----------------------------------------------------------------------

def validateFile(file):
    if not os.path.exists(file):
        logSeparator("*****  %s does not exist, exiting! *****" % file)
        raise SystemExit
    else:
        logSeparator("%s found, continuing..." % file, recurse=0)


#----------------------------------------------------------------------

def main():
    logTruncate()

    try:
        logSeparator("Cleanup")
        os.chdir(WXDIR + '/src/msw')
        do('make cleandll FINAL=1')
        do('makeaddons clean FINAL=1')

        logSeparator("Building Documentation...")
        os.chdir(WXDIR + '/src/msw')
        do('make touchmanual htmlhelp')
        validateFile(WXDIR + '/docs/htmlhelp/wx.chm')

        logSeparator("Building wxWindows and other libraries...")
        os.chdir(WXDIR + '/src/msw')
        do('make dll pch FINAL=1')
        validateFile(WXDIR + '/lib/wx'+dllVer+'.dll')
        ## do("upx -9 " + WXDIR + '/lib/wx'+dllVer+'.dll')

        do('makeaddons FINAL=1')
        validateFile(WXDIR + '/contrib/lib/ogl.lib')
        validateFile(WXDIR + '/contrib/lib/stc.lib')

        logSeparator("Cleaning wxPython build directory...")
        os.chdir(WXDIR + '/utils/wxPython')
        do("del /sxy *.*")

        logSeparator("Copying wxPython workspace to build directory...")
        do("copy /s %s %s" % ('c:\\projects\\wxPython\\*.*', WXDIR+'\\utils\\wxPython'))
        os.chdir(WXDIR + '/utils/wxPython')
        f = open("build.local", "w")
        f.write("""
TARGETDIR = 'c:\\projects\\wx\\utils\\wxPython'
WXPSRCDIR = 'c:\\projects\\wx\\utils\\wxPython\\src'
CRTFLAG='/MD'
FINAL=1
""")
        f.close()


        logSeparator("Cleaning wxPython...")
        os.chdir(WXDIR+'\\utils\\wxPython')
        do("buildall -cu")
        do("ll")


        logSeparator("Building core wxPython...")
        os.chdir(WXDIR+'\\utils\\wxPython\\src')
        do("build -bi")
        validateFile(WXDIR+'\\utils\\wxPython\\wxc.pyd')



        logSeparator("Building wxPython addon modules...")
        os.chdir(WXDIR+'\\utils\\wxPython\\modules')
        do("buildall -bi")
        validateFile(WXDIR+'\\utils\\wxPython\\utilsc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\htmlc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\glcanvasc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\oglc.pyd')
        validateFile(WXDIR+'\\utils\\wxPython\\stc_c.pyd')

        ## os.chdir(WXDIR+'\\utils\\wxPython')
        ## do("upx -9 *.pyd")

        logSeparator("Building installer executable...")
        os.chdir(WXDIR+'\\utils\\wxPython\\distrib')
        do("autoit2 wise.aut")
        srcName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-'+wxpVer+'.EXE'
        destName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-'+wxpVer+'-'+dateSt+'.EXE'
        validateFile(srcName)
        try:
            time.sleep(5)
            os.rename(srcName, destName)
            validateFile(destName)
        except:
            logSeparator("****** UNABLE TO RENAME FILE ******")


        logSeparator("Building source and docs zip files...")
        os.chdir(WXDIR+'\\utils')
        do("wxPython\\distrib\\zipit.bat %s" % wxpVer)
        srcZName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-src-'+wxpVer+'.zip'
        destZName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-src-'+wxpVer+'-'+dateSt+'.zip'
        validateFile(srcZName)
        try:
            os.rename(srcZName, destZName)
        except:
            pass

        srcDName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-docs-'+wxpVer+'.zip'
        destDName = WXDIR+'\\utils\\wxPython\\distrib\\wxPython-docs-'+wxpVer+'-'+dateSt+'.zip'
        validateFile(srcDName)
        try:
            os.rename(srcDName, destDName)
        except:
            pass


        logSeparator("Copying built files...")
        do("copy %s %s" % (destName, cwd))
        do("copy %s %s" % (destZName, cwd))
        do("copy %s %s" % (destDName, cwd))



        # #*#*#*#*#*  Comment this out to allow upload...
        return

        logSeparator("Uploading to website...")
        do('python c:\\utils\\sendwxp.py %s' % destName)
        do('python c:\\utils\\sendwxp.py %s' % destZName)
        do('python c:\\utils\\sendwxp.py %s' % destDName)
        os.unlink(destName)
        os.unlink(destZName)


        logSeparator("Finished!!!")

    finally:
        os.system("list " + logfile)
        pass






if __name__ == '__main__':
    main()
