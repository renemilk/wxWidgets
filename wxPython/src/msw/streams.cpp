/*
 * FILE : src/msw/streams.cpp
 * 
 * This file was automatically generated by :
 * Simplified Wrapper and Interface Generator (SWIG)
 * Version 1.1 (Build 883)
 * 
 * Portions Copyright (c) 1995-1998
 * The University of Utah and The Regents of the University of California.
 * Permission is granted to distribute this file in any manner provided
 * this notice remains intact.
 * 
 * Do not make changes to this file--changes will be lost!
 *
 */


#define SWIGCODE
/* Implementation : PYTHON */

#define SWIGPYTHON
#include <string.h>
#include <stdlib.h>
/* Definitions for Windows/Unix exporting */
#if defined(__WIN32__)
#   if defined(_MSC_VER)
#	define SWIGEXPORT(a) __declspec(dllexport) a
#   else
#	if defined(__BORLANDC__)
#	    define SWIGEXPORT(a) a _export
#	else
#	    define SWIGEXPORT(a) a
#	endif
#   endif
#else
#   define SWIGEXPORT(a) a
#endif

#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

extern void SWIG_MakePtr(char *, void *, char *);
extern void SWIG_RegisterMapping(char *, char *, void *(*)(void *));
extern char *SWIG_GetPtr(char *, void **, char *);
extern char *SWIG_GetPtrObj(PyObject *, void **, char *);
extern void SWIG_addvarlink(PyObject *, char *, PyObject *(*)(void), int (*)(PyObject *));
extern PyObject *SWIG_newvarlink(void);
#ifdef __cplusplus
}
#endif
#define SWIG_init    initstreamsc

#define SWIG_name    "streamsc"

#include "helpers.h"
#include "pyistream.h"
#include <wx/stream.h>
#include <wx/list.h>


static PyObject* t_output_helper(PyObject* target, PyObject* o) {
    PyObject*   o2;
    PyObject*   o3;

    if (!target) {
        target = o;
    } else if (target == Py_None) {
        Py_DECREF(Py_None);
        target = o;
    } else {
        if (!PyTuple_Check(target)) {
            o2 = target;
            target = PyTuple_New(1);
            PyTuple_SetItem(target, 0, o2);
        }
        o3 = PyTuple_New(1);
        PyTuple_SetItem(o3, 0, o);

        o2 = target;
        target = PySequence_Concat(o2, o3);
        Py_DECREF(o2);
        Py_DECREF(o3);
    }
    return target;
}
#ifdef __cplusplus
extern "C" {
#endif
static wxPyInputStream *new_wxPyInputStream(PyObject *p) {
            wxInputStream* wxis = wxPyCBInputStream::create(p);
            if (wxis)
                return new wxPyInputStream(wxis);
            else
                return NULL;
        }

static PyObject *_wrap_new_wxInputStream(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxPyInputStream * _result;
    PyObject * _arg0;
    PyObject * _obj0 = 0;
    char *_kwnames[] = { "p", NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:new_wxInputStream",_kwnames,&_obj0)) 
        return NULL;
{
  _arg0 = _obj0;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxPyInputStream *)new_wxPyInputStream(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxPyInputStream_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

#define wxInputStream_close(_swigobj)  (_swigobj->close())
static PyObject *_wrap_wxInputStream_close(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_close",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_close. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxInputStream_close(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxInputStream_flush(_swigobj)  (_swigobj->flush())
static PyObject *_wrap_wxInputStream_flush(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_flush",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_flush. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxInputStream_flush(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxInputStream_eof(_swigobj)  (_swigobj->eof())
static PyObject *_wrap_wxInputStream_eof(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_eof",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_eof. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxInputStream_eof(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_read(_swigobj,_swigarg0)  (_swigobj->read(_swigarg0))
static PyObject *_wrap_wxInputStream_read(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    PyObject * _result;
    wxPyInputStream * _arg0;
    int  _arg1 = (int ) -1;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","size", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O|i:wxInputStream_read",_kwnames,&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_read. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (PyObject *)wxInputStream_read(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{
  _resultobj = _result;
}
    return _resultobj;
}

#define wxInputStream_readline(_swigobj,_swigarg0)  (_swigobj->readline(_swigarg0))
static PyObject *_wrap_wxInputStream_readline(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    PyObject * _result;
    wxPyInputStream * _arg0;
    int  _arg1 = (int ) -1;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","size", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O|i:wxInputStream_readline",_kwnames,&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_readline. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (PyObject *)wxInputStream_readline(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{
  _resultobj = _result;
}
    return _resultobj;
}

#define wxInputStream_readlines(_swigobj,_swigarg0)  (_swigobj->readlines(_swigarg0))
static PyObject *_wrap_wxInputStream_readlines(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    PyObject * _result;
    wxPyInputStream * _arg0;
    int  _arg1 = (int ) -1;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","sizehint", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O|i:wxInputStream_readlines",_kwnames,&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_readlines. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (PyObject *)wxInputStream_readlines(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{
  _resultobj = _result;
}
    return _resultobj;
}

#define wxInputStream_seek(_swigobj,_swigarg0,_swigarg1)  (_swigobj->seek(_swigarg0,_swigarg1))
static PyObject *_wrap_wxInputStream_seek(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxPyInputStream * _arg0;
    int  _arg1;
    int  _arg2 = (int ) 0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","offset","whence", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"Oi|i:wxInputStream_seek",_kwnames,&_argo0,&_arg1,&_arg2)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_seek. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxInputStream_seek(_arg0,_arg1,_arg2);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxInputStream_tell(_swigobj)  (_swigobj->tell())
static PyObject *_wrap_wxInputStream_tell(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    int  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_tell",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_tell. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (int )wxInputStream_tell(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_Peek(_swigobj)  (_swigobj->Peek())
static PyObject *_wrap_wxInputStream_Peek(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    char  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_Peek",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_Peek. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (char )wxInputStream_Peek(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

#define wxInputStream_GetC(_swigobj)  (_swigobj->GetC())
static PyObject *_wrap_wxInputStream_GetC(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    char  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_GetC",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_GetC. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (char )wxInputStream_GetC(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("c",_result);
    return _resultobj;
}

#define wxInputStream_LastRead(_swigobj)  (_swigobj->LastRead())
static PyObject *_wrap_wxInputStream_LastRead(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    size_t  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_LastRead",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_LastRead. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (size_t )wxInputStream_LastRead(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_CanRead(_swigobj)  (_swigobj->CanRead())
static PyObject *_wrap_wxInputStream_CanRead(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_CanRead",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_CanRead. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxInputStream_CanRead(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_Eof(_swigobj)  (_swigobj->Eof())
static PyObject *_wrap_wxInputStream_Eof(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_Eof",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_Eof. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxInputStream_Eof(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_Ungetch(_swigobj,_swigarg0)  (_swigobj->Ungetch(_swigarg0))
static PyObject *_wrap_wxInputStream_Ungetch(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxPyInputStream * _arg0;
    char  _arg1;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","c", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"Oc:wxInputStream_Ungetch",_kwnames,&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_Ungetch. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxInputStream_Ungetch(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

#define wxInputStream_SeekI(_swigobj,_swigarg0,_swigarg1)  (_swigobj->SeekI(_swigarg0,_swigarg1))
static PyObject *_wrap_wxInputStream_SeekI(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    long  _result;
    wxPyInputStream * _arg0;
    long  _arg1;
    wxSeekMode  _arg2 = (wxSeekMode ) (wxFromStart);
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self","pos","mode", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"Ol|i:wxInputStream_SeekI",_kwnames,&_argo0,&_arg1,&_arg2)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_SeekI. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (long )wxInputStream_SeekI(_arg0,_arg1,_arg2);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("l",_result);
    return _resultobj;
}

#define wxInputStream_TellI(_swigobj)  (_swigobj->TellI())
static PyObject *_wrap_wxInputStream_TellI(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    long  _result;
    wxPyInputStream * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxInputStream_TellI",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxPyInputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxInputStream_TellI. Expected _wxPyInputStream_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (long )wxInputStream_TellI(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("l",_result);
    return _resultobj;
}

static void  wxOutputStream_write(wxOutputStream *self,PyObject * obj) {
            // We use only strings for the streams, not unicode
            PyObject* str = PyObject_Str(obj);
            if (! str) {
                PyErr_SetString(PyExc_TypeError, "Unable to convert to string");
                return;
            }
            self->Write(PyString_AS_STRING(str),
                        PyString_GET_SIZE(str));
            Py_DECREF(str);
        }
static PyObject *_wrap_wxOutputStream_write(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxOutputStream * _arg0;
    PyObject * _arg1;
    PyObject * _argo0 = 0;
    PyObject * _obj1 = 0;
    char *_kwnames[] = { "self","obj", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OO:wxOutputStream_write",_kwnames,&_argo0,&_obj1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxOutputStream_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxOutputStream_write. Expected _wxOutputStream_p.");
        return NULL;
        }
    }
{
  _arg1 = _obj1;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxOutputStream_write(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyMethodDef streamscMethods[] = {
	 { "wxOutputStream_write", (PyCFunction) _wrap_wxOutputStream_write, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_TellI", (PyCFunction) _wrap_wxInputStream_TellI, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_SeekI", (PyCFunction) _wrap_wxInputStream_SeekI, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_Ungetch", (PyCFunction) _wrap_wxInputStream_Ungetch, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_Eof", (PyCFunction) _wrap_wxInputStream_Eof, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_CanRead", (PyCFunction) _wrap_wxInputStream_CanRead, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_LastRead", (PyCFunction) _wrap_wxInputStream_LastRead, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_GetC", (PyCFunction) _wrap_wxInputStream_GetC, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_Peek", (PyCFunction) _wrap_wxInputStream_Peek, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_tell", (PyCFunction) _wrap_wxInputStream_tell, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_seek", (PyCFunction) _wrap_wxInputStream_seek, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_readlines", (PyCFunction) _wrap_wxInputStream_readlines, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_readline", (PyCFunction) _wrap_wxInputStream_readline, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_read", (PyCFunction) _wrap_wxInputStream_read, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_eof", (PyCFunction) _wrap_wxInputStream_eof, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_flush", (PyCFunction) _wrap_wxInputStream_flush, METH_VARARGS | METH_KEYWORDS },
	 { "wxInputStream_close", (PyCFunction) _wrap_wxInputStream_close, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxInputStream", (PyCFunction) _wrap_new_wxInputStream, METH_VARARGS | METH_KEYWORDS },
	 { NULL, NULL }
};
#ifdef __cplusplus
}
#endif
/*
 * This table is used by the pointer type-checker
 */
static struct { char *n1; char *n2; void *(*pcnv)(void *); } _swig_mapping[] = {
    { "_signed_long","_long",0},
    { "_wxPrintQuality","_wxCoord",0},
    { "_wxPrintQuality","_int",0},
    { "_wxPrintQuality","_signed_int",0},
    { "_wxPrintQuality","_unsigned_int",0},
    { "_wxPrintQuality","_wxWindowID",0},
    { "_wxPrintQuality","_uint",0},
    { "_wxPrintQuality","_EBool",0},
    { "_wxPrintQuality","_size_t",0},
    { "_wxPrintQuality","_time_t",0},
    { "_byte","_unsigned_char",0},
    { "_long","_unsigned_long",0},
    { "_long","_signed_long",0},
    { "_size_t","_wxCoord",0},
    { "_size_t","_wxPrintQuality",0},
    { "_size_t","_time_t",0},
    { "_size_t","_unsigned_int",0},
    { "_size_t","_int",0},
    { "_size_t","_wxWindowID",0},
    { "_size_t","_uint",0},
    { "_uint","_wxCoord",0},
    { "_uint","_wxPrintQuality",0},
    { "_uint","_time_t",0},
    { "_uint","_size_t",0},
    { "_uint","_unsigned_int",0},
    { "_uint","_int",0},
    { "_uint","_wxWindowID",0},
    { "_wxChar","_char",0},
    { "_char","_wxChar",0},
    { "_EBool","_wxCoord",0},
    { "_EBool","_wxPrintQuality",0},
    { "_EBool","_signed_int",0},
    { "_EBool","_int",0},
    { "_EBool","_wxWindowID",0},
    { "_unsigned_long","_long",0},
    { "_signed_int","_wxCoord",0},
    { "_signed_int","_wxPrintQuality",0},
    { "_signed_int","_EBool",0},
    { "_signed_int","_wxWindowID",0},
    { "_signed_int","_int",0},
    { "_WXTYPE","_short",0},
    { "_WXTYPE","_signed_short",0},
    { "_WXTYPE","_unsigned_short",0},
    { "_unsigned_short","_WXTYPE",0},
    { "_unsigned_short","_short",0},
    { "_signed_short","_WXTYPE",0},
    { "_signed_short","_short",0},
    { "_unsigned_char","_byte",0},
    { "_unsigned_int","_wxCoord",0},
    { "_unsigned_int","_wxPrintQuality",0},
    { "_unsigned_int","_time_t",0},
    { "_unsigned_int","_size_t",0},
    { "_unsigned_int","_uint",0},
    { "_unsigned_int","_wxWindowID",0},
    { "_unsigned_int","_int",0},
    { "_short","_WXTYPE",0},
    { "_short","_unsigned_short",0},
    { "_short","_signed_short",0},
    { "_wxWindowID","_wxCoord",0},
    { "_wxWindowID","_wxPrintQuality",0},
    { "_wxWindowID","_time_t",0},
    { "_wxWindowID","_size_t",0},
    { "_wxWindowID","_EBool",0},
    { "_wxWindowID","_uint",0},
    { "_wxWindowID","_int",0},
    { "_wxWindowID","_signed_int",0},
    { "_wxWindowID","_unsigned_int",0},
    { "_int","_wxCoord",0},
    { "_int","_wxPrintQuality",0},
    { "_int","_time_t",0},
    { "_int","_size_t",0},
    { "_int","_EBool",0},
    { "_int","_uint",0},
    { "_int","_wxWindowID",0},
    { "_int","_unsigned_int",0},
    { "_int","_signed_int",0},
    { "_time_t","_wxCoord",0},
    { "_time_t","_wxPrintQuality",0},
    { "_time_t","_unsigned_int",0},
    { "_time_t","_int",0},
    { "_time_t","_wxWindowID",0},
    { "_time_t","_uint",0},
    { "_time_t","_size_t",0},
    { "_wxCoord","_int",0},
    { "_wxCoord","_signed_int",0},
    { "_wxCoord","_unsigned_int",0},
    { "_wxCoord","_wxWindowID",0},
    { "_wxCoord","_uint",0},
    { "_wxCoord","_EBool",0},
    { "_wxCoord","_size_t",0},
    { "_wxCoord","_time_t",0},
    { "_wxCoord","_wxPrintQuality",0},
{0,0,0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initstreamsc() {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("streamsc", streamscMethods);
	 d = PyModule_GetDict(m);
	 PyDict_SetItemString(d,"wxFromStart", PyInt_FromLong((long) wxFromStart));
	 PyDict_SetItemString(d,"wxFromCurrent", PyInt_FromLong((long) wxFromCurrent));
	 PyDict_SetItemString(d,"wxFromEnd", PyInt_FromLong((long) wxFromEnd));

    wxPyPtrTypeMap_Add("wxInputStream", "wxPyInputStream");
{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
}
