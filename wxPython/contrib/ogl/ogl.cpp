/*
 * FILE : contrib/ogl/ogl.cpp
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
#define SWIG_init    initoglc

#define SWIG_name    "oglc"

#include "export.h"
#include "oglhelpers.h"


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

//---------------------------------------------------------------------------
// This one will work for any class for the VERY generic cases, but beyond that
// the helper needs to know more about the type.

wxList* wxPy_wxListHelper(PyObject* pyList, char* className) {
    wxPyBeginBlockThreads();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPyEndBlockThreads();
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPyEndBlockThreads();
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* pyo = PyList_GetItem(pyList, x);
        wxObject* wxo = NULL;

        if (SWIG_GetPtrObj(pyo, (void **)&wxo, className)) {
            char errmsg[1024];
            sprintf(errmsg, "Type error, expected list of %s objects", className);
            PyErr_SetString(PyExc_TypeError, errmsg);
            wxPyEndBlockThreads();
            return NULL;
        }
        list->Append(wxo);
    }
    wxPyEndBlockThreads();
    return list;
}

//---------------------------------------------------------------------------

wxList* wxPy_wxRealPoint_ListHelper(PyObject* pyList) {
    wxPyBeginBlockThreads();
    if (!PyList_Check(pyList)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list object.");
        wxPyEndBlockThreads();
        return NULL;
    }
    int count = PyList_Size(pyList);
    wxList* list = new wxList;
    if (! list) {
        PyErr_SetString(PyExc_MemoryError, "Unable to allocate wxList object");
        wxPyEndBlockThreads();
        return NULL;
    }
    for (int x=0; x<count; x++) {
        PyObject* pyo = PyList_GetItem(pyList, x);

        if (PyTuple_Check(pyo)) {
            PyObject* o1 = PyNumber_Float(PyTuple_GetItem(pyo, 0));
            PyObject* o2 = PyNumber_Float(PyTuple_GetItem(pyo, 1));

            double val1 = (o1 ? PyFloat_AsDouble(o1) : 0.0);
            double val2 = (o2 ? PyFloat_AsDouble(o2) : 0.0);

            list->Append((wxObject*) new wxRealPoint(val1, val2));

        } else {
            wxRealPoint* wxo = NULL;
            if (SWIG_GetPtrObj(pyo, (void **)&wxo, "_wxRealPoint_p")) {
                PyErr_SetString(PyExc_TypeError, "Type error, expected list of wxRealPoint objects or 2-tuples");
                wxPyEndBlockThreads();
                return NULL;
            }
            list->Append((wxObject*) new wxRealPoint(*wxo));
        }
    }
    wxPyEndBlockThreads();
    return list;
}

//---------------------------------------------------------------------------

PyObject*  wxPyMake_wxShapeEvtHandler(wxShapeEvtHandler* source) {
    PyObject* target = NULL;

    if (source && wxIsKindOf(source, wxShapeEvtHandler)) {
        // If it's derived from wxShapeEvtHandler then there may
        // already be a pointer to a Python object that we can use
        // in the OOR data.
        wxShapeEvtHandler* seh = (wxShapeEvtHandler*)source;
        wxPyClientData* data = (wxPyClientData*)seh->GetClientObject();
        if (data) {
            target = data->m_obj;
            Py_INCREF(target);
        }
    }
    if (! target) {
        target = wxPyMake_wxObject2(source, FALSE);
        if (target != Py_None)
            ((wxShapeEvtHandler*)source)->SetClientObject(new wxPyClientData(target));
    }
    return target;
}



//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxPyShapeCanvas, wxShapeCanvas);
IMPLEMENT_DYNAMIC_CLASS(wxPyShapeEvtHandler, wxShapeEvtHandler);
IMPLEMENT_ABSTRACT_CLASS(wxPyShape, wxShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyRectangleShape, wxRectangleShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyControlPoint, wxControlPoint);
IMPLEMENT_DYNAMIC_CLASS(wxPyBitmapShape, wxBitmapShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDrawnShape, wxDrawnShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyCompositeShape, wxCompositeShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDividedShape, wxDividedShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyDivisionShape, wxDivisionShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyEllipseShape, wxEllipseShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyCircleShape, wxCircleShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyLineShape, wxLineShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyPolygonShape, wxPolygonShape);
IMPLEMENT_DYNAMIC_CLASS(wxPyTextShape, wxTextShape);

//---------------------------------------------------------------------------

extern "C" SWIGEXPORT(void) initoglbasicc();
extern "C" SWIGEXPORT(void) initoglshapesc();
extern "C" SWIGEXPORT(void) initoglshapes2c();
extern "C" SWIGEXPORT(void) initoglcanvasc();
#ifdef __cplusplus
extern "C" {
#endif
static PyObject *_wrap_wxOGLInitialize(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    char *_kwnames[] = {  NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,":wxOGLInitialize",_kwnames)) 
        return NULL;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxOGLInitialize();

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyObject *_wrap_wxOGLCleanUp(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject * _resultobj;
    char *_kwnames[] = {  NULL };

    self = self;
    if(!PyArg_ParseTupleAndKeywords(args,kwargs,":wxOGLCleanUp",_kwnames)) 
        return NULL;
{
    PyThreadState* __tstate = wxPyBeginAllowThreads();
    wxOGLCleanUp();

    wxPyEndAllowThreads(__tstate);
    if (PyErr_Occurred()) return NULL;
}    Py_INCREF(Py_None);
    _resultobj = Py_None;
    return _resultobj;
}

static PyMethodDef oglcMethods[] = {
	 { "wxOGLCleanUp", (PyCFunction) _wrap_wxOGLCleanUp, METH_VARARGS | METH_KEYWORDS },
	 { "wxOGLInitialize", (PyCFunction) _wrap_wxOGLInitialize, METH_VARARGS | METH_KEYWORDS },
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
    { "_WXTYPE","_wxDateTime_t",0},
    { "_WXTYPE","_short",0},
    { "_WXTYPE","_signed_short",0},
    { "_WXTYPE","_unsigned_short",0},
    { "_unsigned_short","_wxDateTime_t",0},
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
    { "_short","_wxDateTime_t",0},
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
    { "_wxDateTime_t","_unsigned_short",0},
    { "_wxDateTime_t","_short",0},
    { "_wxDateTime_t","_WXTYPE",0},
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
SWIGEXPORT(void) initoglc() {
	 PyObject *m, *d;
	 SWIG_globals = SWIG_newvarlink();
	 m = Py_InitModule("oglc", oglcMethods);
	 d = PyModule_GetDict(m);
	 PyDict_SetItemString(d,"KEY_SHIFT", PyInt_FromLong((long) KEY_SHIFT));
	 PyDict_SetItemString(d,"KEY_CTRL", PyInt_FromLong((long) KEY_CTRL));
	 PyDict_SetItemString(d,"ARROW_NONE", PyInt_FromLong((long) ARROW_NONE));
	 PyDict_SetItemString(d,"ARROW_END", PyInt_FromLong((long) ARROW_END));
	 PyDict_SetItemString(d,"ARROW_BOTH", PyInt_FromLong((long) ARROW_BOTH));
	 PyDict_SetItemString(d,"ARROW_MIDDLE", PyInt_FromLong((long) ARROW_MIDDLE));
	 PyDict_SetItemString(d,"ARROW_START", PyInt_FromLong((long) ARROW_START));
	 PyDict_SetItemString(d,"ARROW_HOLLOW_CIRCLE", PyInt_FromLong((long) ARROW_HOLLOW_CIRCLE));
	 PyDict_SetItemString(d,"ARROW_FILLED_CIRCLE", PyInt_FromLong((long) ARROW_FILLED_CIRCLE));
	 PyDict_SetItemString(d,"ARROW_ARROW", PyInt_FromLong((long) ARROW_ARROW));
	 PyDict_SetItemString(d,"ARROW_SINGLE_OBLIQUE", PyInt_FromLong((long) ARROW_SINGLE_OBLIQUE));
	 PyDict_SetItemString(d,"ARROW_DOUBLE_OBLIQUE", PyInt_FromLong((long) ARROW_DOUBLE_OBLIQUE));
	 PyDict_SetItemString(d,"ARROW_METAFILE", PyInt_FromLong((long) ARROW_METAFILE));
	 PyDict_SetItemString(d,"ARROW_POSITION_END", PyInt_FromLong((long) ARROW_POSITION_END));
	 PyDict_SetItemString(d,"ARROW_POSITION_START", PyInt_FromLong((long) ARROW_POSITION_START));
	 PyDict_SetItemString(d,"CONTROL_POINT_VERTICAL", PyInt_FromLong((long) CONTROL_POINT_VERTICAL));
	 PyDict_SetItemString(d,"CONTROL_POINT_HORIZONTAL", PyInt_FromLong((long) CONTROL_POINT_HORIZONTAL));
	 PyDict_SetItemString(d,"CONTROL_POINT_DIAGONAL", PyInt_FromLong((long) CONTROL_POINT_DIAGONAL));
	 PyDict_SetItemString(d,"CONTROL_POINT_ENDPOINT_TO", PyInt_FromLong((long) CONTROL_POINT_ENDPOINT_TO));
	 PyDict_SetItemString(d,"CONTROL_POINT_ENDPOINT_FROM", PyInt_FromLong((long) CONTROL_POINT_ENDPOINT_FROM));
	 PyDict_SetItemString(d,"CONTROL_POINT_LINE", PyInt_FromLong((long) CONTROL_POINT_LINE));
	 PyDict_SetItemString(d,"FORMAT_NONE", PyInt_FromLong((long) FORMAT_NONE));
	 PyDict_SetItemString(d,"FORMAT_CENTRE_HORIZ", PyInt_FromLong((long) FORMAT_CENTRE_HORIZ));
	 PyDict_SetItemString(d,"FORMAT_CENTRE_VERT", PyInt_FromLong((long) FORMAT_CENTRE_VERT));
	 PyDict_SetItemString(d,"FORMAT_SIZE_TO_CONTENTS", PyInt_FromLong((long) FORMAT_SIZE_TO_CONTENTS));
	 PyDict_SetItemString(d,"LINE_ALIGNMENT_HORIZ", PyInt_FromLong((long) LINE_ALIGNMENT_HORIZ));
	 PyDict_SetItemString(d,"LINE_ALIGNMENT_VERT", PyInt_FromLong((long) LINE_ALIGNMENT_VERT));
	 PyDict_SetItemString(d,"LINE_ALIGNMENT_TO_NEXT_HANDLE", PyInt_FromLong((long) LINE_ALIGNMENT_TO_NEXT_HANDLE));
	 PyDict_SetItemString(d,"LINE_ALIGNMENT_NONE", PyInt_FromLong((long) LINE_ALIGNMENT_NONE));
	 PyDict_SetItemString(d,"SHADOW_NONE", PyInt_FromLong((long) SHADOW_NONE));
	 PyDict_SetItemString(d,"SHADOW_LEFT", PyInt_FromLong((long) SHADOW_LEFT));
	 PyDict_SetItemString(d,"SHADOW_RIGHT", PyInt_FromLong((long) SHADOW_RIGHT));
	 PyDict_SetItemString(d,"OP_CLICK_LEFT", PyInt_FromLong((long) OP_CLICK_LEFT));
	 PyDict_SetItemString(d,"OP_CLICK_RIGHT", PyInt_FromLong((long) OP_CLICK_RIGHT));
	 PyDict_SetItemString(d,"OP_DRAG_LEFT", PyInt_FromLong((long) OP_DRAG_LEFT));
	 PyDict_SetItemString(d,"OP_DRAG_RIGHT", PyInt_FromLong((long) OP_DRAG_RIGHT));
	 PyDict_SetItemString(d,"OP_ALL", PyInt_FromLong((long) OP_ALL));
	 PyDict_SetItemString(d,"ATTACHMENT_MODE_NONE", PyInt_FromLong((long) ATTACHMENT_MODE_NONE));
	 PyDict_SetItemString(d,"ATTACHMENT_MODE_EDGE", PyInt_FromLong((long) ATTACHMENT_MODE_EDGE));
	 PyDict_SetItemString(d,"ATTACHMENT_MODE_BRANCHING", PyInt_FromLong((long) ATTACHMENT_MODE_BRANCHING));
	 PyDict_SetItemString(d,"BRANCHING_ATTACHMENT_NORMAL", PyInt_FromLong((long) BRANCHING_ATTACHMENT_NORMAL));
	 PyDict_SetItemString(d,"BRANCHING_ATTACHMENT_BLOB", PyInt_FromLong((long) BRANCHING_ATTACHMENT_BLOB));
	 PyDict_SetItemString(d,"gyCONSTRAINT_CENTRED_VERTICALLY", PyInt_FromLong((long) gyCONSTRAINT_CENTRED_VERTICALLY));
	 PyDict_SetItemString(d,"gyCONSTRAINT_CENTRED_HORIZONTALLY", PyInt_FromLong((long) gyCONSTRAINT_CENTRED_HORIZONTALLY));
	 PyDict_SetItemString(d,"gyCONSTRAINT_CENTRED_BOTH", PyInt_FromLong((long) gyCONSTRAINT_CENTRED_BOTH));
	 PyDict_SetItemString(d,"gyCONSTRAINT_LEFT_OF", PyInt_FromLong((long) gyCONSTRAINT_LEFT_OF));
	 PyDict_SetItemString(d,"gyCONSTRAINT_RIGHT_OF", PyInt_FromLong((long) gyCONSTRAINT_RIGHT_OF));
	 PyDict_SetItemString(d,"gyCONSTRAINT_ABOVE", PyInt_FromLong((long) gyCONSTRAINT_ABOVE));
	 PyDict_SetItemString(d,"gyCONSTRAINT_BELOW", PyInt_FromLong((long) gyCONSTRAINT_BELOW));
	 PyDict_SetItemString(d,"gyCONSTRAINT_ALIGNED_TOP", PyInt_FromLong((long) gyCONSTRAINT_ALIGNED_TOP));
	 PyDict_SetItemString(d,"gyCONSTRAINT_ALIGNED_BOTTOM", PyInt_FromLong((long) gyCONSTRAINT_ALIGNED_BOTTOM));
	 PyDict_SetItemString(d,"gyCONSTRAINT_ALIGNED_LEFT", PyInt_FromLong((long) gyCONSTRAINT_ALIGNED_LEFT));
	 PyDict_SetItemString(d,"gyCONSTRAINT_ALIGNED_RIGHT", PyInt_FromLong((long) gyCONSTRAINT_ALIGNED_RIGHT));
	 PyDict_SetItemString(d,"gyCONSTRAINT_MIDALIGNED_TOP", PyInt_FromLong((long) gyCONSTRAINT_MIDALIGNED_TOP));
	 PyDict_SetItemString(d,"gyCONSTRAINT_MIDALIGNED_BOTTOM", PyInt_FromLong((long) gyCONSTRAINT_MIDALIGNED_BOTTOM));
	 PyDict_SetItemString(d,"gyCONSTRAINT_MIDALIGNED_LEFT", PyInt_FromLong((long) gyCONSTRAINT_MIDALIGNED_LEFT));
	 PyDict_SetItemString(d,"gyCONSTRAINT_MIDALIGNED_RIGHT", PyInt_FromLong((long) gyCONSTRAINT_MIDALIGNED_RIGHT));
	 PyDict_SetItemString(d,"DIVISION_SIDE_NONE", PyInt_FromLong((long) DIVISION_SIDE_NONE));
	 PyDict_SetItemString(d,"DIVISION_SIDE_LEFT", PyInt_FromLong((long) DIVISION_SIDE_LEFT));
	 PyDict_SetItemString(d,"DIVISION_SIDE_TOP", PyInt_FromLong((long) DIVISION_SIDE_TOP));
	 PyDict_SetItemString(d,"DIVISION_SIDE_RIGHT", PyInt_FromLong((long) DIVISION_SIDE_RIGHT));
	 PyDict_SetItemString(d,"DIVISION_SIDE_BOTTOM", PyInt_FromLong((long) DIVISION_SIDE_BOTTOM));


    initoglbasicc();
    initoglshapesc();
    initoglshapes2c();
    initoglcanvasc();


    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxPyPtrTypeMap_Add("wxControlPoint", "wxPyControlPoint");
    wxPyPtrTypeMap_Add("wxShapeCanvas", "wxPyShapeCanvas");
    wxPyPtrTypeMap_Add("wxShapeEvtHandler", "wxPyShapeEvtHandler");
    wxPyPtrTypeMap_Add("wxShape", "wxPyShape");
    wxPyPtrTypeMap_Add("wxRectangleShape", "wxPyRectangleShape");
    wxPyPtrTypeMap_Add("wxDrawnShape", "wxPyDrawnShape");
    wxPyPtrTypeMap_Add("wxCompositeShape", "wxPyCompositeShape");
    wxPyPtrTypeMap_Add("wxDividedShape", "wxPyDividedShape");
    wxPyPtrTypeMap_Add("wxDivisionShape", "wxPyDivisionShape");
    wxPyPtrTypeMap_Add("wxEllipseShape", "wxPyEllipseShape");
    wxPyPtrTypeMap_Add("wxCircleShape", "wxPyCircleShape");
    wxPyPtrTypeMap_Add("wxLineShape", "wxPyLineShape");
    wxPyPtrTypeMap_Add("wxPolygonShape", "wxPyPolygonShape");
    wxPyPtrTypeMap_Add("wxTextShape", "wxPyTextShape");

{
   int i;
   for (i = 0; _swig_mapping[i].n1; i++)
        SWIG_RegisterMapping(_swig_mapping[i].n1,_swig_mapping[i].n2,_swig_mapping[i].pcnv);
}
}
