# =========================================================================
#     This configuration file was generated by
#     Bakefile 0.1.4 (http://bakefile.sourceforge.net)
#     Beware that all changes made to this file will be overwritten next
#     time you run Bakefile!
# =========================================================================


# -------------------------------------------------------------------------
# These are configurable options:
# -------------------------------------------------------------------------

# C compiler 
CC = wcc386

# C++ compiler 
CXX = wpp386

# Standard flags for CC 
CFLAGS = 

# Standard flags for C++ 
CXXFLAGS = 

# Standard preprocessor flags (common for CC and CXX) 
CPPFLAGS = 

# Standard linker flags 
LDFLAGS = 

# What type of library to build? [0,1]
SHARED = 0

# Build wxUniversal instead of native port? [0,1]
WXUNIV = 0

# Compile Unicode build of wxWidgets? [0,1]
UNICODE = 0

# Type of compiled binaries [debug,release]
BUILD = debug

# Should debugging info be included in the executables? The default value
# "default" means that debug info will be included if BUILD=debug
# and not included if BUILD=release. [0,1,default]
DEBUG_INFO = default

# Should __WXDEBUG__ be defined? The default value "default" means that it will
# be defined if BUILD=debug and not defined if BUILD=release. [0,1,default]
DEBUG_FLAG = default

# Multiple libraries or single huge monolithic one? [0,1]
MONOLITHIC = 1

# Build GUI libraries? [0,1]
USE_GUI = 1

# Build wxHTML library (USE_GUI must be 1)? [0,1]
USE_HTML = 1

# Build wxXRC library (USE_GUI must be 1)? [0,1]
USE_XRC = 1

# Build OpenGL canvas library (USE_GUI must be 1)? [0,1]
USE_OPENGL = 0

# Build ODBC database classes (USE_GUI must be 1)? [0,1]
USE_ODBC = 0

# Enable exceptions in compiled code. [0,1]
USE_EXCEPTIONS = 1

# Is this official build by wxWidgets developers? [0,1]
OFFICIAL_BUILD = 0

# Use this to name your customized DLLs differently 
VENDOR = custom

# Name of your custom configuration. This affects directory
# where object files are stored as well as the location of
# compiled .lib files and setup.h under the lib/ toplevel directory. 
CFG = 

# Compiler flags needed to compile test suite in tests directory. If you want
# to run the tests, set it so that the compiler can find CppUnit headers. 
CPPUNIT_CFLAGS = 

# Linker flags needed to link test suite in tests directory. If you want
# to run the tests, include CppUnit library here. 
CPPUNIT_LIBS = 

# Version of C runtime library to use. You can change this to
# static if SHARED=0, but it is highly recommended to not do
# it if SHARED=1 unless you know what you are doing. [dynamic,static]
RUNTIME_LIBS = dynamic

