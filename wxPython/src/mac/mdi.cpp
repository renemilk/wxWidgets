/*
 * FILE : src/mac/mdi.cpp
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
#include "Python.h"

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
#define SWIG_init    initmdic

#define SWIG_name    "mdic"

#include "helpers.h"


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

    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(FrameNameStr);
#ifdef __cplusplus
extern "C" {
#endif
static void *SwigwxMDIParentFrameTowxFrame(void *ptr) {
    wxMDIParentFrame *src;
    wxFrame *dest;
    src = (wxMDIParentFrame *) ptr;
    dest = (wxFrame *) src;
    return (void *) dest;
}

static void *SwigwxMDIParentFrameTowxTopLevelWindow(void *ptr) {
    wxMDIParentFrame *src;
    wxTopLevelWindow *dest;
    src = (wxMDIParentFrame *) ptr;
    dest = (wxTopLevelWindow *) src;
    return (void *) dest;
}

static void *SwigwxMDIParentFrameTowxWindow(void *ptr) {
    wxMDIParentFrame *src;
    wxWindow *dest;
    src = (wxMDIParentFrame *) ptr;
    dest = (wxWindow *) src;
    return (void *) dest;
}

static void *SwigwxMDIParentFrameTowxEvtHandler(void *ptr) {
    wxMDIParentFrame *src;
    wxEvtHandler *dest;
    src = (wxMDIParentFrame *) ptr;
    dest = (wxEvtHandler *) src;
    return (void *) dest;
}

static void *SwigwxMDIParentFrameTowxObject(void *ptr) {
    wxMDIParentFrame *src;
    wxObject *dest;
    src = (wxMDIParentFrame *) ptr;
    dest = (wxObject *) src;
    return (void *) dest;
}

#define new_wxMDIParentFrame(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6) (new wxMDIParentFrame(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6))
static PyObject *_wrap_new_wxMDIParentFrame(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _result;
    wxWindow * _arg0;
    wxWindowID  _arg1;
    wxString * _arg2;
    wxPoint * _arg3 = (wxPoint *) &wxDefaultPosition;
    wxSize * _arg4 = (wxSize *) &wxDefaultSize;
    long  _arg5 = (long ) wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL;
    wxString * _arg6 = (wxString *) &wxPyFrameNameStr;
    PyObject * _argo0 = 0;
    PyObject * _obj2 = 0;
    wxPoint  temp;
    PyObject * _obj3 = 0;
    wxSize  temp0;
    PyObject * _obj4 = 0;
    PyObject * _obj6 = 0;
    char *_kwnames[] = { "parent","id","title","pos","size","style","name", NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OiO|OOlO:new_wxMDIParentFrame",_kwnames,&_argo0,&_arg1,&_obj2,&_obj3,&_obj4,&_arg5,&_obj6)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxWindow_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of new_wxMDIParentFrame. Expected _wxWindow_p.");
        return NULL;
        }
    }
{
    _arg2 = wxString_in_helper(_obj2);
    if (_arg2 == NULL)
        return NULL;
}
    if (_obj3)
{
    _arg3 = &temp;
    if (! wxPoint_helper(_obj3, &_arg3))
        return NULL;
}
    if (_obj4)
{
    _arg4 = &temp0;
    if (! wxSize_helper(_obj4, &_arg4))
        return NULL;
}
    if (_obj6)
{
    _arg6 = wxString_in_helper(_obj6);
    if (_arg6 == NULL)
        return NULL;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIParentFrame *)new_wxMDIParentFrame(_arg0,_arg1,*_arg2,*_arg3,*_arg4,_arg5,*_arg6);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIParentFrame_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
{
    if (_obj2)
        delete _arg2;
}
{
    if (_obj6)
        delete _arg6;
}
    return _resultobj;
}

#define new_wxPreMDIParentFrame() (new wxMDIParentFrame())
static PyObject *_wrap_new_wxPreMDIParentFrame(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _result;
    char *_kwnames[] = {  NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,":new_wxPreMDIParentFrame",_kwnames)) 
        return NULL;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIParentFrame *)new_wxPreMDIParentFrame();

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIParentFrame_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

#define wxMDIParentFrame_Create(_swigobj,_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6)  (_swigobj->Create(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6))
static PyObject *_wrap_wxMDIParentFrame_Create(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxMDIParentFrame * _arg0;
    wxWindow * _arg1;
    wxWindowID  _arg2;
    wxString * _arg3;
    wxPoint * _arg4 = (wxPoint *) &wxDefaultPosition;
    wxSize * _arg5 = (wxSize *) &wxDefaultSize;
    long  _arg6 = (long ) wxDEFAULT_FRAME_STYLE|wxVSCROLL|wxHSCROLL;
    wxString * _arg7 = (wxString *) &wxPyFrameNameStr;
    PyObject * _argo0 = 0;
    PyObject * _argo1 = 0;
    PyObject * _obj3 = 0;
    wxPoint  temp;
    PyObject * _obj4 = 0;
    wxSize  temp0;
    PyObject * _obj5 = 0;
    PyObject * _obj7 = 0;
    char *_kwnames[] = { "self","parent","id","title","pos","size","style","name", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OOiO|OOlO:wxMDIParentFrame_Create",_kwnames,&_argo0,&_argo1,&_arg2,&_obj3,&_obj4,&_obj5,&_arg6,&_obj7)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_Create. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
    if (_argo1) {
        if (_argo1 == Py_None) { _arg1 = NULL; }
        else if (SWIG_GetPtrObj(_argo1,(void **) &_arg1,"_wxWindow_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 2 of wxMDIParentFrame_Create. Expected _wxWindow_p.");
        return NULL;
        }
    }
{
    _arg3 = wxString_in_helper(_obj3);
    if (_arg3 == NULL)
        return NULL;
}
    if (_obj4)
{
    _arg4 = &temp;
    if (! wxPoint_helper(_obj4, &_arg4))
        return NULL;
}
    if (_obj5)
{
    _arg5 = &temp0;
    if (! wxSize_helper(_obj5, &_arg5))
        return NULL;
}
    if (_obj7)
{
    _arg7 = wxString_in_helper(_obj7);
    if (_arg7 == NULL)
        return NULL;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxMDIParentFrame_Create(_arg0,_arg1,_arg2,*_arg3,*_arg4,*_arg5,_arg6,*_arg7);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
{
    if (_obj3)
        delete _arg3;
}
{
    if (_obj7)
        delete _arg7;
}
    return _resultobj;
}

#define wxMDIParentFrame_ActivateNext(_swigobj)  (_swigobj->ActivateNext())
static PyObject *_wrap_wxMDIParentFrame_ActivateNext(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_ActivateNext",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_ActivateNext. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIParentFrame_ActivateNext(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIParentFrame_ActivatePrevious(_swigobj)  (_swigobj->ActivatePrevious())
static PyObject *_wrap_wxMDIParentFrame_ActivatePrevious(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_ActivatePrevious",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_ActivatePrevious. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIParentFrame_ActivatePrevious(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIParentFrame_ArrangeIcons(_swigobj)  (_swigobj->ArrangeIcons())
static PyObject *_wrap_wxMDIParentFrame_ArrangeIcons(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_ArrangeIcons",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_ArrangeIcons. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIParentFrame_ArrangeIcons(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIParentFrame_Cascade(_swigobj)  (_swigobj->Cascade())
static PyObject *_wrap_wxMDIParentFrame_Cascade(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_Cascade",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_Cascade. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIParentFrame_Cascade(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIParentFrame_GetActiveChild(_swigobj)  (_swigobj->GetActiveChild())
static PyObject *_wrap_wxMDIParentFrame_GetActiveChild(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _result;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_GetActiveChild",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_GetActiveChild. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIChildFrame *)wxMDIParentFrame_GetActiveChild(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{ _resultobj = wxPyMake_wxObject(_result); }
    return _resultobj;
}

#define wxMDIParentFrame_GetClientWindow(_swigobj)  (_swigobj->GetClientWindow())
static PyObject *_wrap_wxMDIParentFrame_GetClientWindow(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIClientWindow * _result;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_GetClientWindow",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_GetClientWindow. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIClientWindow *)wxMDIParentFrame_GetClientWindow(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{ _resultobj = wxPyMake_wxObject(_result); }
    return _resultobj;
}

#define wxMDIParentFrame_GetToolBar(_swigobj)  (_swigobj->GetToolBar())
static PyObject *_wrap_wxMDIParentFrame_GetToolBar(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxWindow * _result;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_GetToolBar",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_GetToolBar. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxWindow *)wxMDIParentFrame_GetToolBar(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}{ _resultobj = wxPyMake_wxObject(_result); }
    return _resultobj;
}

#define wxMDIParentFrame_Tile(_swigobj)  (_swigobj->Tile())
static PyObject *_wrap_wxMDIParentFrame_Tile(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIParentFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIParentFrame_Tile",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIParentFrame_Tile. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIParentFrame_Tile(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static void *SwigwxMDIChildFrameTowxFrame(void *ptr) {
    wxMDIChildFrame *src;
    wxFrame *dest;
    src = (wxMDIChildFrame *) ptr;
    dest = (wxFrame *) src;
    return (void *) dest;
}

static void *SwigwxMDIChildFrameTowxTopLevelWindow(void *ptr) {
    wxMDIChildFrame *src;
    wxTopLevelWindow *dest;
    src = (wxMDIChildFrame *) ptr;
    dest = (wxTopLevelWindow *) src;
    return (void *) dest;
}

static void *SwigwxMDIChildFrameTowxWindow(void *ptr) {
    wxMDIChildFrame *src;
    wxWindow *dest;
    src = (wxMDIChildFrame *) ptr;
    dest = (wxWindow *) src;
    return (void *) dest;
}

static void *SwigwxMDIChildFrameTowxEvtHandler(void *ptr) {
    wxMDIChildFrame *src;
    wxEvtHandler *dest;
    src = (wxMDIChildFrame *) ptr;
    dest = (wxEvtHandler *) src;
    return (void *) dest;
}

static void *SwigwxMDIChildFrameTowxObject(void *ptr) {
    wxMDIChildFrame *src;
    wxObject *dest;
    src = (wxMDIChildFrame *) ptr;
    dest = (wxObject *) src;
    return (void *) dest;
}

#define new_wxMDIChildFrame(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6) (new wxMDIChildFrame(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6))
static PyObject *_wrap_new_wxMDIChildFrame(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _result;
    wxMDIParentFrame * _arg0;
    wxWindowID  _arg1;
    wxString * _arg2;
    wxPoint * _arg3 = (wxPoint *) &wxDefaultPosition;
    wxSize * _arg4 = (wxSize *) &wxDefaultSize;
    long  _arg5 = (long ) wxDEFAULT_FRAME_STYLE;
    wxString * _arg6 = (wxString *) &wxPyFrameNameStr;
    PyObject * _argo0 = 0;
    PyObject * _obj2 = 0;
    wxPoint  temp;
    PyObject * _obj3 = 0;
    wxSize  temp0;
    PyObject * _obj4 = 0;
    PyObject * _obj6 = 0;
    char *_kwnames[] = { "parent","id","title","pos","size","style","name", NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OiO|OOlO:new_wxMDIChildFrame",_kwnames,&_argo0,&_arg1,&_obj2,&_obj3,&_obj4,&_arg5,&_obj6)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of new_wxMDIChildFrame. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    _arg2 = wxString_in_helper(_obj2);
    if (_arg2 == NULL)
        return NULL;
}
    if (_obj3)
{
    _arg3 = &temp;
    if (! wxPoint_helper(_obj3, &_arg3))
        return NULL;
}
    if (_obj4)
{
    _arg4 = &temp0;
    if (! wxSize_helper(_obj4, &_arg4))
        return NULL;
}
    if (_obj6)
{
    _arg6 = wxString_in_helper(_obj6);
    if (_arg6 == NULL)
        return NULL;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIChildFrame *)new_wxMDIChildFrame(_arg0,_arg1,*_arg2,*_arg3,*_arg4,_arg5,*_arg6);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIChildFrame_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
{
    if (_obj2)
        delete _arg2;
}
{
    if (_obj6)
        delete _arg6;
}
    return _resultobj;
}

#define new_wxPreMDIChildFrame() (new wxMDIChildFrame())
static PyObject *_wrap_new_wxPreMDIChildFrame(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _result;
    char *_kwnames[] = {  NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,":new_wxPreMDIChildFrame",_kwnames)) 
        return NULL;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIChildFrame *)new_wxPreMDIChildFrame();

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIChildFrame_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

#define wxMDIChildFrame_Create(_swigobj,_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6)  (_swigobj->Create(_swigarg0,_swigarg1,_swigarg2,_swigarg3,_swigarg4,_swigarg5,_swigarg6))
static PyObject *_wrap_wxMDIChildFrame_Create(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxMDIChildFrame * _arg0;
    wxMDIParentFrame * _arg1;
    wxWindowID  _arg2;
    wxString * _arg3;
    wxPoint * _arg4 = (wxPoint *) &wxDefaultPosition;
    wxSize * _arg5 = (wxSize *) &wxDefaultSize;
    long  _arg6 = (long ) wxDEFAULT_FRAME_STYLE;
    wxString * _arg7 = (wxString *) &wxPyFrameNameStr;
    PyObject * _argo0 = 0;
    PyObject * _argo1 = 0;
    PyObject * _obj3 = 0;
    wxPoint  temp;
    PyObject * _obj4 = 0;
    wxSize  temp0;
    PyObject * _obj5 = 0;
    PyObject * _obj7 = 0;
    char *_kwnames[] = { "self","parent","id","title","pos","size","style","name", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OOiO|OOlO:wxMDIChildFrame_Create",_kwnames,&_argo0,&_argo1,&_arg2,&_obj3,&_obj4,&_obj5,&_arg6,&_obj7)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIChildFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIChildFrame_Create. Expected _wxMDIChildFrame_p.");
        return NULL;
        }
    }
    if (_argo1) {
        if (_argo1 == Py_None) { _arg1 = NULL; }
        else if (SWIG_GetPtrObj(_argo1,(void **) &_arg1,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 2 of wxMDIChildFrame_Create. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    _arg3 = wxString_in_helper(_obj3);
    if (_arg3 == NULL)
        return NULL;
}
    if (_obj4)
{
    _arg4 = &temp;
    if (! wxPoint_helper(_obj4, &_arg4))
        return NULL;
}
    if (_obj5)
{
    _arg5 = &temp0;
    if (! wxSize_helper(_obj5, &_arg5))
        return NULL;
}
    if (_obj7)
{
    _arg7 = wxString_in_helper(_obj7);
    if (_arg7 == NULL)
        return NULL;
}
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxMDIChildFrame_Create(_arg0,_arg1,_arg2,*_arg3,*_arg4,*_arg5,_arg6,*_arg7);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
{
    if (_obj3)
        delete _arg3;
}
{
    if (_obj7)
        delete _arg7;
}
    return _resultobj;
}

#define wxMDIChildFrame_Activate(_swigobj)  (_swigobj->Activate())
static PyObject *_wrap_wxMDIChildFrame_Activate(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIChildFrame_Activate",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIChildFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIChildFrame_Activate. Expected _wxMDIChildFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIChildFrame_Activate(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIChildFrame_Maximize(_swigobj,_swigarg0)  (_swigobj->Maximize(_swigarg0))
static PyObject *_wrap_wxMDIChildFrame_Maximize(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _arg0;
    bool  _arg1;
    PyObject * _argo0 = 0;
    int tempbool1;
    char *_kwnames[] = { "self","maximize", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"Oi:wxMDIChildFrame_Maximize",_kwnames,&_argo0,&tempbool1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIChildFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIChildFrame_Maximize. Expected _wxMDIChildFrame_p.");
        return NULL;
        }
    }
    _arg1 = (bool ) tempbool1;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIChildFrame_Maximize(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

#define wxMDIChildFrame_Restore(_swigobj)  (_swigobj->Restore())
static PyObject *_wrap_wxMDIChildFrame_Restore(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIChildFrame * _arg0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "self", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O:wxMDIChildFrame_Restore",_kwnames,&_argo0)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIChildFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIChildFrame_Restore. Expected _wxMDIChildFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxMDIChildFrame_Restore(_arg0);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static void *SwigwxMDIClientWindowTowxWindow(void *ptr) {
    wxMDIClientWindow *src;
    wxWindow *dest;
    src = (wxMDIClientWindow *) ptr;
    dest = (wxWindow *) src;
    return (void *) dest;
}

static void *SwigwxMDIClientWindowTowxEvtHandler(void *ptr) {
    wxMDIClientWindow *src;
    wxEvtHandler *dest;
    src = (wxMDIClientWindow *) ptr;
    dest = (wxEvtHandler *) src;
    return (void *) dest;
}

static void *SwigwxMDIClientWindowTowxObject(void *ptr) {
    wxMDIClientWindow *src;
    wxObject *dest;
    src = (wxMDIClientWindow *) ptr;
    dest = (wxObject *) src;
    return (void *) dest;
}

#define new_wxMDIClientWindow(_swigarg0,_swigarg1) (new wxMDIClientWindow(_swigarg0,_swigarg1))
static PyObject *_wrap_new_wxMDIClientWindow(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIClientWindow * _result;
    wxMDIParentFrame * _arg0;
    long  _arg1 = (long ) 0;
    PyObject * _argo0 = 0;
    char *_kwnames[] = { "parent","style", NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"O|l:new_wxMDIClientWindow",_kwnames,&_argo0,&_arg1)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of new_wxMDIClientWindow. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIClientWindow *)new_wxMDIClientWindow(_arg0,_arg1);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIClientWindow_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

#define new_wxPreMDIClientWindow() (new wxMDIClientWindow())
static PyObject *_wrap_new_wxPreMDIClientWindow(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    wxMDIClientWindow * _result;
    char *_kwnames[] = {  NULL };
    char _ptemp[128];

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,":new_wxPreMDIClientWindow",_kwnames)) 
        return NULL;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (wxMDIClientWindow *)new_wxPreMDIClientWindow();

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    if (_result) {
        SWIG_MakePtr(_ptemp, (char *) _result,"_wxMDIClientWindow_p");
        _resultobj = Py_BuildValue("s",_ptemp);
    } else {
        Py_INCREF(Py_None);
        _resultobj = Py_None;
    }
    return _resultobj;
}

#define wxMDIClientWindow_Create(_swigobj,_swigarg0,_swigarg1)  (_swigobj->Create(_swigarg0,_swigarg1))
static PyObject *_wrap_wxMDIClientWindow_Create(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    bool  _result;
    wxMDIClientWindow * _arg0;
    wxMDIParentFrame * _arg1;
    long  _arg2 = (long ) 0;
    PyObject * _argo0 = 0;
    PyObject * _argo1 = 0;
    char *_kwnames[] = { "self","parent","style", NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,"OO|l:wxMDIClientWindow_Create",_kwnames,&_argo0,&_argo1,&_arg2)) 
        return NULL;
    if (_argo0) {
        if (_argo0 == Py_None) { _arg0 = NULL; }
        else if (SWIG_GetPtrObj(_argo0,(void **) &_arg0,"_wxMDIClientWindow_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 1 of wxMDIClientWindow_Create. Expected _wxMDIClientWindow_p.");
        return NULL;
        }
    }
    if (_argo1) {
        if (_argo1 == Py_None) { _arg1 = NULL; }
        else if (SWIG_GetPtrObj(_argo1,(void **) &_arg1,"_wxMDIParentFrame_p")) {
            PyErr_SetString(PyExc_TypeError,"Type error in argument 2 of wxMDIClientWindow_Create. Expected _wxMDIParentFrame_p.");
        return NULL;
        }
    }
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    _result = (bool )wxMDIClientWindow_Create(_arg0,_arg1,_arg2);

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    _resultobj = Py_BuildValue("i",_result);
    return _resultobj;
}

static PyMethodDef mdicMethods[] = {
	 { "wxMDIClientWindow_Create", (PyCFunction) _wrap_wxMDIClientWindow_Create, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxPreMDIClientWindow", (PyCFunction) _wrap_new_wxPreMDIClientWindow, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxMDIClientWindow", (PyCFunction) _wrap_new_wxMDIClientWindow, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIChildFrame_Restore", (PyCFunction) _wrap_wxMDIChildFrame_Restore, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIChildFrame_Maximize", (PyCFunction) _wrap_wxMDIChildFrame_Maximize, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIChildFrame_Activate", (PyCFunction) _wrap_wxMDIChildFrame_Activate, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIChildFrame_Create", (PyCFunction) _wrap_wxMDIChildFrame_Create, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxPreMDIChildFrame", (PyCFunction) _wrap_new_wxPreMDIChildFrame, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxMDIChildFrame", (PyCFunction) _wrap_new_wxMDIChildFrame, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_Tile", (PyCFunction) _wrap_wxMDIParentFrame_Tile, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_GetToolBar", (PyCFunction) _wrap_wxMDIParentFrame_GetToolBar, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_GetClientWindow", (PyCFunction) _wrap_wxMDIParentFrame_GetClientWindow, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_GetActiveChild", (PyCFunction) _wrap_wxMDIParentFrame_GetActiveChild, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_Cascade", (PyCFunction) _wrap_wxMDIParentFrame_Cascade, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_ArrangeIcons", (PyCFunction) _wrap_wxMDIParentFrame_ArrangeIcons, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_ActivatePrevious", (PyCFunction) _wrap_wxMDIParentFrame_ActivatePrevious, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_ActivateNext", (PyCFunction) _wrap_wxMDIParentFrame_ActivateNext, METH_VARARGS | METH_KEYWORDS },
	 { "wxMDIParentFrame_Create", (PyCFunction) _wrap_wxMDIParentFrame_Create, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxPreMDIParentFrame", (PyCFunction) _wrap_new_wxPreMDIParentFrame, METH_VARARGS | METH_KEYWORDS },
	 { "new_wxMDIParentFrame", (PyCFunction) _wrap_new_wxMDIParentFrame, METH_VARARGS | METH_KEYWORDS },
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
    { "_wxTopLevelWindow","_wxMDIChildFrame",SwigwxMDIChildFrameTowxTopLevelWindow},
    { "_wxTopLevelWindow","_wxMDIParentFrame",SwigwxMDIParentFrameTowxTopLevelWindow},
    { "_uint","_wxCoord",0},
    { "_uint","_wxPrintQuality",0},
    { "_uint","_time_t",0},
    { "_uint","_size_t",0},
    { "_uint","_unsigned_int",0},
    { "_uint","_int",0},
    { "_uint","_wxWindowID",0},
    { "_wxChar","_char",0},
    { "_char","_wxChar",0},
    { "_struct_wxNativeFontInfo","_wxNativeFontInfo",0},
    { "_EBool","_wxCoord",0},
    { "_EBool","_wxPrintQuality",0},
    { "_EBool","_signed_int",0},
    { "_EBool","_int",0},
    { "_EBool","_wxWindowID",0},
    { "_unsigned_long","_long",0},
    { "_wxNativeFontInfo","_struct_wxNativeFontInfo",0},
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
    { "_wxObject","_wxMDIClientWindow",SwigwxMDIClientWindowTowxObject},
    { "_wxObject","_wxMDIChildFrame",SwigwxMDIChildFrameTowxObject},
    { "_wxObject","_wxMDIParentFrame",SwigwxMDIParentFrameTowxObject},
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
    { "_wxFrame","_wxMDIChildFrame",SwigwxMDIChildFrameTowxFrame},
    { "_wxFrame","_wxMDIParentFrame",SwigwxMDIParentFrameTowxFrame},
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
    { "_wxEvtHandler","_wxMDIClientWindow",SwigwxMDIClientWindowTowxEvtHandler},
    { "_wxEvtHandler","_wxMDIChildFrame",SwigwxMDIChildFrameTowxEvtHandler},
    { "_wxEvtHandler","_wxMDIParentFrame",SwigwxMDIParentFrameTowxEvtHandler},
    { "_wxWindow","_wxMDIClientWindow",SwigwxMDIClientWindowTowxWindow},
    { "_wxWindow","_wxMDIChildFrame",SwigwxMDIChildFrameTowxWindow},
    { "_wxWindow","_wxMDIParentFrame",SwigwxMDIParentFrameTowxWindow},
{0,0,0}};

static PyObject *SWIG_globals;
#ifdef __cplusplus
extern "C" 
#endif
SWIGEXPORT(void) initmdic() {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("mdic", mdicMethods);
	 d = PyModule_GetDict(m);
	 PyDict_SetItemString(d,"IDM_WINDOWTILE", PyInt_FromLong((long) 4001));
	 PyDict_SetItemString(d,"IDM_WINDOWTILEHOR", PyInt_FromLong((long) 4001));
	 PyDict_SetItemString(d,"IDM_WINDOWCASCADE", PyInt_FromLong((long) 4002));
	 PyDict_SetItemString(d,"IDM_WINDOWICONS", PyInt_FromLong((long) 4003));
	 PyDict_SetItemString(d,"IDM_WINDOWNEXT", PyInt_FromLong((long) 4004));
	 PyDict_SetItemString(d,"IDM_WINDOWTILEVERT", PyInt_FromLong((long) 4005));
	 PyDict_SetItemString(d,"wxFIRST_MDI_CHILD", PyInt_FromLong((long) 4100));
	 PyDict_SetItemString(d,"wxLAST_MDI_CHILD", PyInt_FromLong((long) 4600));
{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
}
