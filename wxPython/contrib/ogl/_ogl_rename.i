// A bunch of %rename directives generated by BuildRenamers in config.py
// in order to remove the wx prefix from all global scope names.

#ifndef BUILDING_RENAMERS

%rename(ShapeRegion)                        wxShapeRegion;
%rename(AttachmentPoint)                    wxAttachmentPoint;
%rename(PyShapeEvtHandler)                  wxPyShapeEvtHandler;
%rename(PyShape)                            wxPyShape;
%rename(PseudoMetaFile)                     wxPseudoMetaFile;
%rename(PyRectangleShape)                   wxPyRectangleShape;
%rename(PyControlPoint)                     wxPyControlPoint;
%rename(PyBitmapShape)                      wxPyBitmapShape;
%rename(PyDrawnShape)                       wxPyDrawnShape;
%rename(OGLConstraint)                      wxOGLConstraint;
%rename(PyCompositeShape)                   wxPyCompositeShape;
%rename(PyDividedShape)                     wxPyDividedShape;
%rename(PyDivisionShape)                    wxPyDivisionShape;
%rename(PyEllipseShape)                     wxPyEllipseShape;
%rename(PyCircleShape)                      wxPyCircleShape;
%rename(ArrowHead)                          wxArrowHead;
%rename(PyLineShape)                        wxPyLineShape;
%rename(PyPolygonShape)                     wxPyPolygonShape;
%rename(PyTextShape)                        wxPyTextShape;
%rename(Diagram)                            wxDiagram;
%rename(PyShapeCanvas)                      wxPyShapeCanvas;
%rename(OGLInitialize)                      wxOGLInitialize;
%rename(OGLCleanUp)                         wxOGLCleanUp;

#endif
