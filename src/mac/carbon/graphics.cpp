/////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/dccg.cpp
// Purpose:     wxDC class
// Author:      Stefan Csomor
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#if wxUSE_GRAPHICS_CONTEXT && wxMAC_USE_CORE_GRAPHICS

#include "wx/graphics.h"

#ifndef WX_PRECOMP
    #include "wx/dcclient.h"
    #include "wx/log.h"
    #include "wx/region.h"
#endif

#include "wx/mac/uma.h"

#ifdef __MSL__
    #if __MSL__ >= 0x6000
        #include "math.h"
        // in case our functions were defined outside std, we make it known all the same
        namespace std { }
        using namespace std;
    #endif
#endif

#include "wx/mac/private.h"

#if MAC_OS_X_VERSION_MAX_ALLOWED <= MAC_OS_X_VERSION_10_4
typedef float CGFloat;
#endif

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

#if !defined( __DARWIN__ ) || defined(__MWERKS__)
#ifndef M_PI
const double M_PI = 3.14159265358979;
#endif
#endif

static const double RAD2DEG = 180.0 / M_PI;

//
// Pen, Brushes and Fonts
//

#pragma mark -
#pragma mark wxMacCoreGraphicsPattern, ImagePattern, HatchPattern classes

// CGPattern wrapper class: always allocate on heap, never call destructor

class wxMacCoreGraphicsPattern
{
public :
    wxMacCoreGraphicsPattern() {}

    // is guaranteed to be called only with a non-Null CGContextRef
    virtual void Render( CGContextRef ctxRef ) = 0;

    operator CGPatternRef() const { return m_patternRef; }

protected :
    virtual ~wxMacCoreGraphicsPattern()
    {
        // as this is called only when the m_patternRef is been released;
        // don't release it again
    }

    static void _Render( void *info, CGContextRef ctxRef )
    {
        wxMacCoreGraphicsPattern* self = (wxMacCoreGraphicsPattern*) info;
        if ( self && ctxRef )
            self->Render( ctxRef );
    }

    static void _Dispose( void *info )
    {
        wxMacCoreGraphicsPattern* self = (wxMacCoreGraphicsPattern*) info;
        delete self;
    }

    CGPatternRef m_patternRef;

    static const CGPatternCallbacks ms_Callbacks;
};

const CGPatternCallbacks wxMacCoreGraphicsPattern::ms_Callbacks = { 0, &wxMacCoreGraphicsPattern::_Render, &wxMacCoreGraphicsPattern::_Dispose };

class ImagePattern : public wxMacCoreGraphicsPattern
{
public :
    ImagePattern( const wxBitmap* bmp , const CGAffineTransform& transform )
    {
        wxASSERT( bmp && bmp->Ok() );

        Init( (CGImageRef) bmp->CGImageCreate() , transform );
    }

    // ImagePattern takes ownership of CGImageRef passed in
    ImagePattern( CGImageRef image , const CGAffineTransform& transform )
    {
        if ( image )
            CFRetain( image );

        Init( image , transform );
    }

    virtual void Render( CGContextRef ctxRef )
    {
        if (m_image != NULL)
            HIViewDrawCGImage( ctxRef, &m_imageBounds, m_image );
    }

protected :
    void Init( CGImageRef image, const CGAffineTransform& transform )
    {
        m_image = image;
        if ( m_image )
        {
            m_imageBounds = CGRectMake( 0.0, 0.0, (CGFloat)CGImageGetWidth( m_image ), (CGFloat)CGImageGetHeight( m_image ) );
            m_patternRef = CGPatternCreate(
                this , m_imageBounds, transform ,
                m_imageBounds.size.width, m_imageBounds.size.height,
                kCGPatternTilingNoDistortion, true , &wxMacCoreGraphicsPattern::ms_Callbacks );
        }
    }

    virtual ~ImagePattern()
    {
        if ( m_image )
            CGImageRelease( m_image );
    }

    CGImageRef m_image;
    CGRect m_imageBounds;
};

class HatchPattern : public wxMacCoreGraphicsPattern
{
public :
    HatchPattern( int hatchstyle, const CGAffineTransform& transform )
    {
        m_hatch = hatchstyle;
        m_imageBounds = CGRectMake( 0.0, 0.0, 8.0 , 8.0 );
        m_patternRef = CGPatternCreate(
            this , m_imageBounds, transform ,
            m_imageBounds.size.width, m_imageBounds.size.height,
            kCGPatternTilingNoDistortion, false , &wxMacCoreGraphicsPattern::ms_Callbacks );
    }

    void StrokeLineSegments( CGContextRef ctxRef , const CGPoint pts[] , size_t count )
    {
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_4
        if ( UMAGetSystemVersion() >= 0x1040 )
        {
            CGContextStrokeLineSegments( ctxRef , pts , count );
        }
        else
#endif
        {
            CGContextBeginPath( ctxRef );
            for (size_t i = 0; i < count; i += 2)
            {
                CGContextMoveToPoint(ctxRef, pts[i].x, pts[i].y);
                CGContextAddLineToPoint(ctxRef, pts[i+1].x, pts[i+1].y);
            }
            CGContextStrokePath(ctxRef);
        }
    }

    virtual void Render( CGContextRef ctxRef )
    {
        switch ( m_hatch )
        {
            case wxBDIAGONAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 8.0 , 0.0 } , { 0.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxCROSSDIAG_HATCH :
                {
                    CGPoint pts[] =
                    {
                        { 0.0 , 0.0 } , { 8.0 , 8.0 } ,
                        { 8.0 , 0.0 } , { 0.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 4 );
                }
                break;

            case wxFDIAGONAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 0.0 } , { 8.0 , 8.0 }
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxCROSS_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 4 );
                }
                break;

            case wxHORIZONTAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 0.0 , 4.0 } , { 8.0 , 4.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            case wxVERTICAL_HATCH :
                {
                    CGPoint pts[] =
                    {
                    { 4.0 , 0.0 } , { 4.0 , 8.0 } ,
                    };
                    StrokeLineSegments( ctxRef , pts , 2 );
                }
                break;

            default:
                break;
        }
    }

protected :
    virtual ~HatchPattern() {}

    CGRect      m_imageBounds;
    int         m_hatch;
};

class wxMacCoreGraphicsPen : public wxGraphicsPen
{
public:
    wxMacCoreGraphicsPen();
    wxMacCoreGraphicsPen( wxGraphicsRenderer* renderer, const wxPen &pen );
    ~wxMacCoreGraphicsPen();
    
    void Init();
    virtual void Apply( wxGraphicsContext* context );
    virtual wxDouble GetWidth() { return m_width; }
   
protected :
    CGLineCap m_cap;
    wxMacCFRefHolder<CGColorRef> m_color;
    wxMacCFRefHolder<CGColorSpaceRef> m_colorSpace;

    CGLineJoin m_join;
    CGFloat m_width;
    
    int m_count;
    const CGFloat *m_lengths;
    CGFloat *m_userLengths;


    bool m_isPattern;
    wxMacCFRefHolder<CGPatternRef> m_pattern;
    CGFloat* m_patternColorComponents;
private :
DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsPen)
};

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsPen,wxGraphicsPen)

wxMacCoreGraphicsPen::wxMacCoreGraphicsPen() : wxGraphicsPen( NULL )
{
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsPen::wxMacCoreGraphicsPen( wxGraphicsRenderer* renderer, const wxPen &pen ) :
    wxGraphicsPen( renderer )
{
    Init();
    
    float components[4] = { pen.GetColour().Red() / 255.0 , pen.GetColour().Green() / 255.0 , 
            pen.GetColour().Blue() / 255.0 , pen.GetColour().Alpha() / 255.0 } ;
    m_color.Set( CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ) ;

    // TODO: * m_dc->m_scaleX
    m_width = pen.GetWidth();
    if (m_width <= 0.0)
        m_width = 0.1;

    switch ( pen.GetCap() )
    {
        case wxCAP_ROUND :
            m_cap = kCGLineCapRound;
            break;

        case wxCAP_PROJECTING :
            m_cap = kCGLineCapSquare;
            break;

        case wxCAP_BUTT :
            m_cap = kCGLineCapButt;
            break;

        default :
            m_cap = kCGLineCapButt;
            break;
    }

    switch ( pen.GetJoin() )
    {
        case wxJOIN_BEVEL :
            m_join = kCGLineJoinBevel;
            break;

        case wxJOIN_MITER :
            m_join = kCGLineJoinMiter;
            break;

        case wxJOIN_ROUND :
            m_join = kCGLineJoinRound;
            break;

        default :
            m_join = kCGLineJoinMiter;
            break;
    }

    const CGFloat dashUnit = m_width < 1.0 ? 1.0 : m_width;

    const CGFloat dotted[] = { dashUnit , dashUnit + 2.0 };
    static const CGFloat short_dashed[] = { 9.0 , 6.0 };
    static const CGFloat dashed[] = { 19.0 , 9.0 };
    static const CGFloat dotted_dashed[] = { 9.0 , 6.0 , 3.0 , 3.0 };

    switch ( pen.GetStyle() )
    {
        case wxSOLID :
            break;

        case wxDOT :
            m_count = WXSIZEOF(dotted);
            m_userLengths = new CGFloat[ m_count ] ;
            memcpy( m_userLengths, dotted, sizeof(dotted) );
            m_lengths = m_userLengths;
            break;

        case wxLONG_DASH :
            m_count = WXSIZEOF(dashed);
            m_lengths = dashed;
            break;

        case wxSHORT_DASH :
            m_count = WXSIZEOF(short_dashed);
            m_lengths = short_dashed;
            break;

        case wxDOT_DASH :
            m_count = WXSIZEOF(dotted_dashed);
            m_lengths = dotted_dashed;
            break;

        case wxUSER_DASH :
            wxDash *dashes;
            m_count = pen.GetDashes( &dashes );
            if ((dashes != NULL) && (m_count > 0))
            {
                m_userLengths = new CGFloat[m_count];
                for ( int i = 0; i < m_count; ++i )
                {
                    m_userLengths[i] = dashes[i] * dashUnit;

                    if ( i % 2 == 1 && m_userLengths[i] < dashUnit + 2.0 )
                        m_userLengths[i] = dashUnit + 2.0;
                    else if ( i % 2 == 0 && m_userLengths[i] < dashUnit )
                        m_userLengths[i] = dashUnit;
                }
            }
            m_lengths = m_userLengths;
            break;

        case wxSTIPPLE :
            {
                wxBitmap* bmp = pen.GetStipple();
                if ( bmp && bmp->Ok() )
                {
                    m_colorSpace.Set( CGColorSpaceCreatePattern( NULL ) );
                    m_pattern.Set( *( new ImagePattern( bmp , CGAffineTransformMakeTranslation( 0,0 ) ) ) );
                    m_patternColorComponents = new CGFloat[1] ;
                    m_patternColorComponents[0] = 1.0;
                    m_isPattern = true;
                }
            }
            break;

        default :
            {
                m_isPattern = true;
                m_colorSpace.Set( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
                m_pattern.Set( *( new HatchPattern( pen.GetStyle() , CGAffineTransformMakeTranslation( 0,0 ) ) ) );
                m_patternColorComponents = new CGFloat[4] ;
                m_patternColorComponents[0] = pen.GetColour().Red() / 255.0;
                m_patternColorComponents[1] = pen.GetColour().Green() / 255.0;
                m_patternColorComponents[2] = pen.GetColour().Blue() / 255.0;
                m_patternColorComponents[3] =  pen.GetColour().Alpha() / 255.0;
            }
            break;
    }
    if ((m_lengths != NULL) && (m_count > 0))
    {
        // force the line cap, otherwise we get artifacts (overlaps) and just solid lines
        m_cap = kCGLineCapButt;
    }
}

wxMacCoreGraphicsPen::~wxMacCoreGraphicsPen()
{
    delete[] m_userLengths;
    delete[] m_patternColorComponents;
}

void wxMacCoreGraphicsPen::Init()
{
    m_lengths = NULL;
    m_userLengths = NULL;
    m_width = NULL;
    m_count = 0;
    m_patternColorComponents = NULL;
    m_isPattern = false;
}

void wxMacCoreGraphicsPen::Apply( wxGraphicsContext* context )
{
    CGContextRef cg = (CGContextRef) context->GetNativeContext();
    CGContextSetLineWidth( cg , m_width );
    CGContextSetLineJoin( cg , m_join );

    CGContextSetLineDash( cg , 0 , m_lengths , m_count );
    CGContextSetLineCap( cg , m_cap );

    if ( m_isPattern )
    {
        CGContextSetStrokeColorSpace( cg , m_colorSpace );
        CGContextSetStrokePattern( cg, m_pattern , m_patternColorComponents );
    }
    else
    {
        CGContextSetStrokeColorWithColor( cg , m_color );
    }
}

//
// Brush
//

class wxMacCoreGraphicsBrush : public wxGraphicsBrush
{
public:
    wxMacCoreGraphicsBrush();
    wxMacCoreGraphicsBrush( wxGraphicsRenderer* renderer );
    wxMacCoreGraphicsBrush( wxGraphicsRenderer* renderer, const wxBrush &brush );
    ~wxMacCoreGraphicsBrush ();
    
    virtual void Apply( wxGraphicsContext* context );
    void CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2 );
    void CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor );
        
    virtual bool IsShading() { return m_isShading; }
    CGShadingRef GetShading() { return m_shading; }
protected:
    CGFunctionRef CreateGradientFunction( const wxColour& c1, const wxColour& c2 );
    static void CalculateShadingValues (void *info, const CGFloat *in, CGFloat *out);
    virtual void Init();
    
    wxMacCFRefHolder<CGColorRef> m_color;
    wxMacCFRefHolder<CGColorSpaceRef> m_colorSpace;

    bool m_isPattern;
    wxMacCFRefHolder<CGPatternRef> m_pattern;
    CGFloat* m_patternColorComponents;
    
    bool m_isShading;
    CGFunctionRef m_gradientFunction;
    CGShadingRef m_shading;
    CGFloat *m_gradientComponents;
private :
DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsBrush)
};

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsBrush,wxGraphicsBrush)

wxMacCoreGraphicsBrush::wxMacCoreGraphicsBrush() : wxGraphicsBrush( NULL )
{
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsBrush::wxMacCoreGraphicsBrush( wxGraphicsRenderer* renderer) : wxGraphicsBrush( renderer )
{
    Init();
}

void wxMacCoreGraphicsBrush::CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2 )
{
    m_gradientFunction = CreateGradientFunction( c1, c2 );
    m_shading = CGShadingCreateAxial( wxMacGetGenericRGBColorSpace(), CGPointMake(x1,y1), CGPointMake(x2,y2), m_gradientFunction, true, true ) ;
    m_isShading = true ;
}
        
void wxMacCoreGraphicsBrush::CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor )
{
    m_gradientFunction = CreateGradientFunction( oColor, cColor );
    m_shading = CGShadingCreateRadial( wxMacGetGenericRGBColorSpace(), CGPointMake(xo,yo), 0, CGPointMake(xc,yc), radius, m_gradientFunction, true, true ) ;
    m_isShading = true ;
}
                
wxMacCoreGraphicsBrush::wxMacCoreGraphicsBrush(wxGraphicsRenderer* renderer, const wxBrush &brush) : wxGraphicsBrush( renderer )
{
    Init();
    
    if ( brush.GetStyle() == wxSOLID )
    {
        float components[4] = { brush.GetColour().Red() / 255.0 , brush.GetColour().Green() / 255.0 , 
                brush.GetColour().Blue() / 255.0 , brush.GetColour().Alpha() / 255.0 } ;
        m_color.Set( CGColorCreate( wxMacGetGenericRGBColorSpace() , components ) ) ;
    }
    else if ( brush.IsHatch() )
    {
        m_isPattern = true;
        m_colorSpace.Set( CGColorSpaceCreatePattern( wxMacGetGenericRGBColorSpace() ) );
        m_pattern.Set( *( new HatchPattern( brush.GetStyle() , CGAffineTransformMakeTranslation( 0,0 ) ) ) );

        m_patternColorComponents = new CGFloat[4] ;
        m_patternColorComponents[0] = brush.GetColour().Red() / 255.0;
        m_patternColorComponents[1] = brush.GetColour().Green() / 255.0;
        m_patternColorComponents[2] = brush.GetColour().Blue() / 255.0;
        m_patternColorComponents[3] = brush.GetColour().Alpha() / 255.0;
    }
    else
    {
        // now brush is a bitmap
        wxBitmap* bmp = brush.GetStipple();
        if ( bmp && bmp->Ok() )
        {
            m_isPattern = true;    
            m_patternColorComponents = new CGFloat[1] ;
            m_patternColorComponents[0] = 1.0;
            m_colorSpace.Set( CGColorSpaceCreatePattern( NULL ) );
            m_pattern.Set( *( new ImagePattern( bmp , CGAffineTransformMakeTranslation( 0,0 ) ) ) );
        }
    }
}

wxMacCoreGraphicsBrush::~wxMacCoreGraphicsBrush()
{
    if ( m_shading )
        CGShadingRelease(m_shading);
        
    if( m_gradientFunction )
        CGFunctionRelease(m_gradientFunction);
        
    delete[] m_gradientComponents;
    delete[] m_patternColorComponents;
}

void wxMacCoreGraphicsBrush::Init()
{
    m_patternColorComponents = NULL;
    m_gradientFunction = NULL;
    m_shading = NULL;
    m_isPattern = false;
    m_gradientComponents = NULL;
    m_isShading = false;
}

void wxMacCoreGraphicsBrush::Apply( wxGraphicsContext* context )
{
    CGContextRef cg = (CGContextRef) context->GetNativeContext();
    
    if ( m_isShading )
    {   
    }
    else
    {
        if ( m_isPattern )
        {
            CGContextSetFillColorSpace( cg , m_colorSpace );
            CGContextSetFillPattern( cg, m_pattern , m_patternColorComponents );
        }
        else
        {
            CGContextSetFillColorWithColor( cg, m_color );
        }
    }
}

void wxMacCoreGraphicsBrush::CalculateShadingValues (void *info, const CGFloat *in, CGFloat *out)
{
    CGFloat* colors = (CGFloat*) info ;
    CGFloat f = *in;
    for( int i = 0 ; i < 4 ; ++i )
    {
        out[i] = colors[i] + ( colors[4+i] - colors[i] ) * f;
    }
}

CGFunctionRef wxMacCoreGraphicsBrush::CreateGradientFunction( const wxColour& c1, const wxColour& c2 )
{
    static const CGFunctionCallbacks callbacks = { 0, &CalculateShadingValues, NULL };
    static const CGFloat input_value_range [2] = { 0, 1 };
    static const CGFloat output_value_ranges [8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
    m_gradientComponents = new CGFloat[8] ;
    m_gradientComponents[0] = c1.Red() / 255.0;
    m_gradientComponents[1] = c1.Green() / 255.0;
    m_gradientComponents[2] = c1.Blue() / 255.0;
    m_gradientComponents[3] = c1.Alpha() / 255.0;
    m_gradientComponents[4] = c2.Red() / 255.0;
    m_gradientComponents[5] = c2.Green() / 255.0;
    m_gradientComponents[6] = c2.Blue() / 255.0;
    m_gradientComponents[7] = c2.Alpha() / 255.0;

    return CGFunctionCreate ( m_gradientComponents,  1, 
                            input_value_range, 
                            4, 
                            output_value_ranges,
                            &callbacks);  
}

//
// Font
//

class wxMacCoreGraphicsFont : public wxGraphicsFont
{
public:
    wxMacCoreGraphicsFont();
    wxMacCoreGraphicsFont( wxGraphicsRenderer* renderer, const wxFont &font, const wxColour& col );
    ~wxMacCoreGraphicsFont();
    
    virtual void Apply( wxGraphicsContext* context );
    virtual ATSUStyle GetATSUStyle() { return m_macATSUIStyle; }
private :
    ATSUStyle m_macATSUIStyle;
DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsFont)
};

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsFont,wxGraphicsFont)

wxMacCoreGraphicsFont::wxMacCoreGraphicsFont() : wxGraphicsFont( NULL )
{
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsFont::wxMacCoreGraphicsFont(wxGraphicsRenderer* renderer, const wxFont &font, const wxColour& col) : wxGraphicsFont( renderer )
{
    m_macATSUIStyle = NULL;
    
    OSStatus status;

    status = ATSUCreateAndCopyStyle( (ATSUStyle) font.MacGetATSUStyle() , &m_macATSUIStyle );

    wxASSERT_MSG( status == noErr, wxT("couldn't create ATSU style") );

    // we need the scale here ...

    Fixed atsuSize = IntToFixed( int( 1 * font.MacGetFontSize()) );
    RGBColor atsuColor = MAC_WXCOLORREF( col.GetPixel() );
    ATSUAttributeTag atsuTags[] =
    {
            kATSUSizeTag ,
            kATSUColorTag ,
    };
    ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
            sizeof( Fixed ) ,
            sizeof( RGBColor ) ,
    };
    ATSUAttributeValuePtr atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
    {
            &atsuSize ,
            &atsuColor ,
    };

    status = ::ATSUSetAttributes(
        m_macATSUIStyle, sizeof(atsuTags) / sizeof(ATSUAttributeTag) ,
        atsuTags, atsuSizes, atsuValues);

    wxASSERT_MSG( status == noErr , wxT("couldn't modify ATSU style") );
}

wxMacCoreGraphicsFont::~wxMacCoreGraphicsFont()
{
    if ( m_macATSUIStyle )
    {
        ::ATSUDisposeStyle((ATSUStyle)m_macATSUIStyle);
        m_macATSUIStyle = NULL;
    }
}

void wxMacCoreGraphicsFont::Apply( wxGraphicsContext* WXUNUSED(context) )
{
    // nothing to do here
}

//
// Graphics Matrix
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsMatrix declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMacCoreGraphicsMatrix : public wxGraphicsMatrix
{
public :
    wxMacCoreGraphicsMatrix() ;
    
	wxMacCoreGraphicsMatrix(wxGraphicsRenderer* renderer, wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0) ;
	
	virtual ~wxMacCoreGraphicsMatrix() ;
	
    virtual wxGraphicsMatrix *Clone() const ;

	// concatenates the matrix
	virtual void Concat( const wxGraphicsMatrix *t );
	
	// copies the passed in matrix
	virtual void Copy( const wxGraphicsMatrix *t );

	// sets the matrix to the respective values
	virtual void Set(wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0);

	// makes this the inverse matrix
	virtual void Invert();
	
	// returns true if the elements of the transformation matrix are equal ?
    virtual bool IsEqual( const wxGraphicsMatrix* t) const ;
	
	// return true if this is the identity matrix
	virtual bool IsIdentity();
	
    //
    // transformation
    //
    
    // add the translation to this matrix
    virtual void Translate( wxDouble dx , wxDouble dy );

    // add the scale to this matrix
    virtual void Scale( wxDouble xScale , wxDouble yScale );

    // add the rotation to this matrix (radians)
    virtual void Rotate( wxDouble angle );	
	
    //
    // apply the transforms
    //
	
	// applies that matrix to the point
	virtual void TransformPoint( wxDouble *x, wxDouble *y );
	
	// applies the matrix except for translations
	virtual void TransformDistance( wxDouble *dx, wxDouble *dy );
	
	// returns the native representation
	virtual void * GetNativeMatrix() const;
	
private :
    CGAffineTransform m_matrix;
    
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsMatrix)
} ;

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsMatrix implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsMatrix, wxGraphicsMatrix)

wxMacCoreGraphicsMatrix::wxMacCoreGraphicsMatrix() : wxGraphicsMatrix(NULL)
{
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsMatrix::wxMacCoreGraphicsMatrix(wxGraphicsRenderer* renderer, wxDouble a, wxDouble b, wxDouble c, wxDouble d, 
    wxDouble tx, wxDouble ty) : wxGraphicsMatrix(renderer)
{
    m_matrix = CGAffineTransformMake(a,b,c,d,tx,ty);
}

wxMacCoreGraphicsMatrix::~wxMacCoreGraphicsMatrix() 
{
}

wxGraphicsMatrix *wxMacCoreGraphicsMatrix::Clone() const 
{
    wxMacCoreGraphicsMatrix* m = new wxMacCoreGraphicsMatrix(GetRenderer()) ;
    m->m_matrix = m_matrix ;
    return m;
}

// concatenates the matrix
void wxMacCoreGraphicsMatrix::Concat( const wxGraphicsMatrix *t ) 
{
    m_matrix = CGAffineTransformConcat(m_matrix, *((CGAffineTransform*) t->GetNativeMatrix()) );
}

// copies the passed in matrix
void wxMacCoreGraphicsMatrix::Copy( const wxGraphicsMatrix *t ) 
{
    m_matrix = *((CGAffineTransform*) t->GetNativeMatrix());
}

// sets the matrix to the respective values
void wxMacCoreGraphicsMatrix::Set(wxDouble a, wxDouble b, wxDouble c, wxDouble d, 
    wxDouble tx, wxDouble ty) 
{
    m_matrix = CGAffineTransformMake(a,b,c,d,tx,ty);
}

// makes this the inverse matrix
void wxMacCoreGraphicsMatrix::Invert() 
{
    m_matrix = CGAffineTransformInvert( m_matrix );
}

// returns true if the elements of the transformation matrix are equal ?
bool wxMacCoreGraphicsMatrix::IsEqual( const wxGraphicsMatrix* t) const  
{
    return CGAffineTransformEqualToTransform(m_matrix, *((CGAffineTransform*) t->GetNativeMatrix()));
}

// return true if this is the identity matrix
bool wxMacCoreGraphicsMatrix::IsIdentity() 
{
    return CGAffineTransformIsIdentity(m_matrix);
}

//
// transformation
//

// add the translation to this matrix
void wxMacCoreGraphicsMatrix::Translate( wxDouble dx , wxDouble dy ) 
{
    m_matrix = CGAffineTransformTranslate( m_matrix, dx, dy);
}

// add the scale to this matrix
void wxMacCoreGraphicsMatrix::Scale( wxDouble xScale , wxDouble yScale ) 
{
    m_matrix = CGAffineTransformScale( m_matrix, xScale, yScale);
}

// add the rotation to this matrix (radians)
void wxMacCoreGraphicsMatrix::Rotate( wxDouble angle ) 
{
    m_matrix = CGAffineTransformRotate( m_matrix, angle);
}

//
// apply the transforms
//

// applies that matrix to the point
void wxMacCoreGraphicsMatrix::TransformPoint( wxDouble *x, wxDouble *y ) 
{
    wxDouble x1, y1 ;
    
    x1 = m_matrix.a * (*x) + m_matrix.c * (*y) + m_matrix.tx ;
    y1 = m_matrix.b * (*x) + m_matrix.d * (*y) + m_matrix.ty ;
    
    *x = x1;
    *y = y1;
}

// applies the matrix except for translations
void wxMacCoreGraphicsMatrix::TransformDistance( wxDouble *dx, wxDouble *dy ) 
{
    wxDouble x1, y1 ;
    
    x1 = m_matrix.a * (*dx) + m_matrix.c * (*dy);
    y1 = m_matrix.b * (*dx) + m_matrix.d * (*dy);
    
    *dx = x1;
    *dy = y1;
}

// returns the native representation
void * wxMacCoreGraphicsMatrix::GetNativeMatrix() const 
{
    return (void*) &m_matrix;
}

//
// Graphics Path
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsPath declaration
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMacCoreGraphicsPath : public wxGraphicsPath
{
public :
    wxMacCoreGraphicsPath( wxGraphicsRenderer* renderer, CGMutablePathRef path = NULL);
    
    wxMacCoreGraphicsPath();
    
    ~wxMacCoreGraphicsPath();

    virtual wxGraphicsPath *Clone() const;

    // begins a new subpath at (x,y)
    virtual void MoveToPoint( wxDouble x, wxDouble y );

    // adds a straight line from the current point to (x,y) 
    virtual void AddLineToPoint( wxDouble x, wxDouble y );

    // adds a cubic Bezier curve from the current point, using two control points and an end point
    virtual void AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y );

    // closes the current sub-path
    virtual void CloseSubpath();

    // gets the last point of the current path, (0,0) if not yet set
    virtual void GetCurrentPoint( wxDouble& x, wxDouble&y);

    // adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
    virtual void AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise );

    //
    // These are convenience functions which - if not available natively will be assembled 
    // using the primitives from above
    //

    // adds a quadratic Bezier curve from the current point, using a control point and an end point
    virtual void AddQuadCurveToPoint( wxDouble cx, wxDouble cy, wxDouble x, wxDouble y );

    // appends a rectangle as a new closed subpath 
    virtual void AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    // appends an ellipsis as a new closed subpath fitting the passed rectangle
    virtual void AddCircle( wxDouble x, wxDouble y, wxDouble r );

    // draws a an arc to two tangents connecting (current) to (x1,y1) and (x1,y1) to (x2,y2), also a straight line from (current) to (x1,y1)
    virtual void AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r );

	// adds another path
	virtual void AddPath( const wxGraphicsPath* path );

	// returns the native path
	virtual void * GetNativePath() const { return m_path; }
	
	// give the native path returned by GetNativePath() back (there might be some deallocations necessary)
	virtual void UnGetNativePath(void *p) {}

	// transforms each point of this path by the matrix
	virtual void Transform( wxGraphicsMatrix* matrix );
	
	// gets the bounding box enclosing all points (possibly including control points)
	virtual void GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *y);
	
	virtual bool Contains( wxDouble x, wxDouble y, int fillStyle = wxWINDING_RULE);
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsPath)
private :
    CGMutablePathRef m_path;
};

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsPath implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsPath, wxGraphicsPath)

wxMacCoreGraphicsPath::wxMacCoreGraphicsPath() :  wxGraphicsPath(NULL)
{
    wxLogDebug(wxT("Illegal Constructor called"));
}    


wxMacCoreGraphicsPath::wxMacCoreGraphicsPath( wxGraphicsRenderer* renderer, CGMutablePathRef path) : wxGraphicsPath(renderer)
{
	if ( path )
		m_path = path;
	else
		m_path = CGPathCreateMutable();
}

wxMacCoreGraphicsPath::~wxMacCoreGraphicsPath()
{
    CGPathRelease( m_path );
}

wxGraphicsPath* wxMacCoreGraphicsPath::Clone() const 
{
	wxMacCoreGraphicsPath* clone = new wxMacCoreGraphicsPath(GetRenderer(),CGPathCreateMutableCopy(m_path));
	return clone ;
}


// opens (starts) a new subpath
void wxMacCoreGraphicsPath::MoveToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathMoveToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPath::AddLineToPoint( wxDouble x1 , wxDouble y1 )
{
    CGPathAddLineToPoint( m_path , NULL , x1 , y1 );
}

void wxMacCoreGraphicsPath::AddCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble cx2, wxDouble cy2, wxDouble x, wxDouble y )
{
    CGPathAddCurveToPoint( m_path , NULL , cx1 , cy1 , cx2, cy2, x , y );
}

void wxMacCoreGraphicsPath::AddQuadCurveToPoint( wxDouble cx1, wxDouble cy1, wxDouble x, wxDouble y )
{
    CGPathAddQuadCurveToPoint( m_path , NULL , cx1 , cy1 , x , y );
}

void wxMacCoreGraphicsPath::AddRectangle( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
    CGRect cgRect = { { x , y } , { w , h } };
    CGPathAddRect( m_path , NULL , cgRect );
}

void wxMacCoreGraphicsPath::AddCircle( wxDouble x, wxDouble y , wxDouble r )
{
    CGPathAddArc( m_path , NULL , x , y , r , 0.0 , 2 * M_PI , true );
}

// adds an arc of a circle centering at (x,y) with radius (r) from startAngle to endAngle
void wxMacCoreGraphicsPath::AddArc( wxDouble x, wxDouble y, wxDouble r, wxDouble startAngle, wxDouble endAngle, bool clockwise )
{
    // inverse direction as we the 'normal' state is a y axis pointing down, ie mirrored to the standard core graphics setup
    CGPathAddArc( m_path, NULL , x, y, r, startAngle, endAngle, !clockwise); 
}

void wxMacCoreGraphicsPath::AddArcToPoint( wxDouble x1, wxDouble y1 , wxDouble x2, wxDouble y2, wxDouble r )
{
    CGPathAddArcToPoint( m_path, NULL , x1, y1, x2, y2, r); 
}

void wxMacCoreGraphicsPath::AddPath( const wxGraphicsPath* path )
{
	CGPathAddPath( m_path , NULL, (CGPathRef) path->GetNativePath() );
}

// closes the current subpath
void wxMacCoreGraphicsPath::CloseSubpath()
{
    CGPathCloseSubpath( m_path );
}

// gets the last point of the current path, (0,0) if not yet set
void wxMacCoreGraphicsPath::GetCurrentPoint( wxDouble& x, wxDouble&y)
{
    CGPoint p = CGPathGetCurrentPoint( m_path );
    x = p.x;
    y = p.y;
}

// transforms each point of this path by the matrix
void wxMacCoreGraphicsPath::Transform( wxGraphicsMatrix* matrix )
{
	CGMutablePathRef p = CGPathCreateMutable() ;
	CGPathAddPath( p, (CGAffineTransform*) matrix->GetNativeMatrix() , m_path );
	CGPathRelease( m_path );
	m_path = p;
}

// gets the bounding box enclosing all points (possibly including control points)
void wxMacCoreGraphicsPath::GetBox(wxDouble *x, wxDouble *y, wxDouble *w, wxDouble *h)
{
	CGRect bounds = CGPathGetBoundingBox( m_path ) ;
	*x = bounds.origin.x;
	*y = bounds.origin.y;
	*w = bounds.size.width;
	*h = bounds.size.height;
}

bool wxMacCoreGraphicsPath::Contains( wxDouble x, wxDouble y, int fillStyle) 
{
	return CGPathContainsPoint( m_path, NULL, CGPointMake(x,y), fillStyle == wxODDEVEN_RULE );
}


//
// Graphics Pen
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsPen declaration
//-----------------------------------------------------------------------------


//
// Graphics Context
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsContext declaration
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMacCoreGraphicsContext : public wxGraphicsContext
{
public:
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext );
    
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window );
    
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window );
    
    wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer);
    
    wxMacCoreGraphicsContext();
    
    ~wxMacCoreGraphicsContext();

	void Init();

    // push the current state of the context, ie the transformation matrix on a stack
    virtual void PushState();

    // pops a stored state from the stack
    virtual void PopState();

    // clips drawings to the region
    virtual void Clip( const wxRegion &region );

    // clips drawings to the rect
    virtual void Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h );
    
    // resets the clipping to original extent
    virtual void ResetClip();

    virtual void * GetNativeContext();
    
    //
    // transformation
    //
    
    // translate
    virtual void Translate( wxDouble dx , wxDouble dy );

    // scale
    virtual void Scale( wxDouble xScale , wxDouble yScale );

    // rotate (radians)
    virtual void Rotate( wxDouble angle );

	// concatenates this transform with the current transform of this context
	virtual void ConcatTransform( const wxGraphicsMatrix* matrix );

	// sets the transform of this context
	virtual void SetTransform( const wxGraphicsMatrix* matrix );

	// gets the matrix of this context
	virtual void GetTransform( wxGraphicsMatrix* matrix );
    //
    // setting the paint
    //
    
    // sets the pen
    virtual void SetPen( wxGraphicsPen* pen , bool release = true );
    
    // sets the brush for filling
    virtual void SetBrush( wxGraphicsBrush* brush , bool release = true );

    // sets the font
    virtual void SetFont( wxGraphicsFont* font, bool release = true );

    // strokes along a path with the current pen
    virtual void StrokePath( const wxGraphicsPath *path );

    // fills a path with the current brush
    virtual void FillPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE );

    // draws a path by first filling and then stroking
    virtual void DrawPath( const wxGraphicsPath *path, int fillStyle = wxWINDING_RULE );

    virtual bool ShouldOffset() const
    {
        int penwidth = 0 ;
        if ( m_pen )
        {
            penwidth = m_pen->GetWidth();
            if ( penwidth == 0 )
                penwidth = 1;
        }
        return ( penwidth % 2 ) == 1; 
    }
    //
    // text
    //
    
    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y );

    virtual void DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle );

    virtual void GetTextExtent( const wxString &text, wxDouble *width, wxDouble *height,
        wxDouble *descent, wxDouble *externalLeading ) const;

    virtual void GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const;

    //
    // image support
    //

    virtual void DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    virtual void DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h );

    void SetNativeContext( CGContextRef cg );
        
    DECLARE_NO_COPY_CLASS(wxMacCoreGraphicsContext)
    DECLARE_DYNAMIC_CLASS(wxMacCoreGraphicsContext)

private:
    void EnsureIsValid();

    CGContextRef m_cgContext;
	WindowRef m_windowRef;
	int m_originX;
	int m_originY;
	wxMacCFRefHolder<HIShapeRef> m_clipRgn;
	bool m_releaseContext;
};

//-----------------------------------------------------------------------------
// device context implementation
//
// more and more of the dc functionality should be implemented by calling
// the appropricate wxMacCoreGraphicsContext, but we will have to do that step by step
// also coordinate conversions should be moved to native matrix ops
//-----------------------------------------------------------------------------

// we always stock two context states, one at entry, to be able to preserve the
// state we were called with, the other one after changing to HI Graphics orientation
// (this one is used for getting back clippings etc)

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsContext implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsContext, wxGraphicsContext)

void wxMacCoreGraphicsContext::Init()
{
    m_cgContext = NULL;
	m_releaseContext = false;
    m_windowRef = NULL;
	m_originX = 0;
	m_originY = 0;
    HIRect r = CGRectMake(0,0,0,0);
    m_clipRgn.Set(HIShapeCreateWithRect(&r));
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, CGContextRef cgcontext ) : wxGraphicsContext(renderer)
{
	Init();
	m_cgContext = cgcontext;
    // FIXME: This check is needed because currently we need to use a DC/GraphicsContext
    // in order to get font properties, like wxFont::GetPixelSize, but since we don't have 
    // a native window attached to use, I create a wxGraphicsContext with a NULL CGContextRef
    // for this one operation.
    
    // When wxFont::GetPixelSize on Mac no longer needs a graphics context, this check
    // can be removed. 
    if (m_cgContext)
    {
        CGContextSaveGState( m_cgContext );
        CGContextSaveGState( m_cgContext );
    }
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, WindowRef window ): wxGraphicsContext(renderer)
{
	Init();
	m_windowRef = window;
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext( wxGraphicsRenderer* renderer, wxWindow* window ): wxGraphicsContext(renderer)
{
	Init();
	m_windowRef = (WindowRef) window->MacGetTopLevelWindowRef();
    m_originX = m_originY = 0;
    window->MacWindowToRootWindow( &m_originX , &m_originY );
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext(wxGraphicsRenderer* renderer) : wxGraphicsContext(renderer)
{
	Init();
}

wxMacCoreGraphicsContext::wxMacCoreGraphicsContext() : wxGraphicsContext(NULL)
{
	Init();
    wxLogDebug(wxT("Illegal Constructor called"));
}

wxMacCoreGraphicsContext::~wxMacCoreGraphicsContext()
{
    SetPen(NULL);
    SetFont(NULL);
    SetBrush(NULL);
    
    if ( m_cgContext )
    {
        // TODO : when is this necessary - should we add a Flush() method ? CGContextSynchronize( m_cgContext );
        CGContextRestoreGState( m_cgContext );
        CGContextRestoreGState( m_cgContext );
    }

    if ( m_releaseContext )
		QDEndCGContext( GetWindowPort( m_windowRef ) , &m_cgContext);
}

void wxMacCoreGraphicsContext::EnsureIsValid()
{
	if ( !m_cgContext )
	{
		OSStatus status = QDBeginCGContext( GetWindowPort( m_windowRef ) , &m_cgContext );
		wxASSERT_MSG( status == noErr , wxT("Cannot nest wxDCs on the same window") );
		Rect bounds;
		GetWindowBounds( m_windowRef, kWindowContentRgn, &bounds );
		CGContextSaveGState( m_cgContext );
		CGContextTranslateCTM( m_cgContext , 0 , bounds.bottom - bounds.top );
		CGContextScaleCTM( m_cgContext , 1 , -1 );
		CGContextTranslateCTM( m_cgContext, m_originX, m_originY );
		CGContextSaveGState( m_cgContext );
		m_releaseContext = true;
		if ( !HIShapeIsEmpty(m_clipRgn) )
		{
			HIShapeReplacePathInCGContext( m_clipRgn, m_cgContext );
			CGContextClip( m_cgContext );
		}
	}
}


void wxMacCoreGraphicsContext::Clip( const wxRegion &region )
{
	if( m_cgContext )
	{
		HIShapeRef shape = HIShapeCreateWithQDRgn( (RgnHandle) region.GetWXHRGN() );
		HIShapeReplacePathInCGContext( shape, m_cgContext );
		CGContextClip( m_cgContext );
		CFRelease( shape );
	}
	else
	{
		m_clipRgn.Set(HIShapeCreateWithQDRgn( (RgnHandle) region.GetWXHRGN() ));
	}
}

// clips drawings to the rect
void wxMacCoreGraphicsContext::Clip( wxDouble x, wxDouble y, wxDouble w, wxDouble h )
{
	HIRect r = CGRectMake( x , y , w , h );
	if ( m_cgContext )
	{
		CGContextClipToRect( m_cgContext, r );
	}
	else
	{
		m_clipRgn.Set(HIShapeCreateWithRect(&r));
	}
}
	
	// resets the clipping to original extent
void wxMacCoreGraphicsContext::ResetClip()
{
	if ( m_cgContext )
	{
		CGContextRestoreGState( m_cgContext );
		CGContextSaveGState( m_cgContext );
	}
	else
	{
        HIRect r = CGRectMake(0,0,0,0);
        m_clipRgn.Set(HIShapeCreateWithRect(&r));
	}
}

void wxMacCoreGraphicsContext::StrokePath( const wxGraphicsPath *path )
{
    if ( m_pen == NULL )
        return ;

	EnsureIsValid();
	
    bool offset = ShouldOffset();
    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    m_pen->Apply(this);
    CGContextAddPath( m_cgContext , (CGPathRef) path->GetNativePath() );
    CGContextStrokePath( m_cgContext );

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
}

void wxMacCoreGraphicsContext::DrawPath( const wxGraphicsPath *path , int fillStyle )
{        
    if ( m_brush != NULL && ((wxMacCoreGraphicsBrush*)m_brush)->IsShading() )
    {
        // when using shading, we cannot draw pen and brush at the same time
        // revert to the base implementation of first filling and then stroking
        wxGraphicsContext::DrawPath( path, fillStyle );
        return;
    }
    
    CGPathDrawingMode mode = kCGPathFill ;
    if ( m_brush == NULL )
    {
        if ( m_pen == NULL )
            return;
        else
            mode = kCGPathStroke;
    }
    else
    {
        if ( m_pen == NULL )
        {
            if ( fillStyle == wxODDEVEN_RULE )
                mode = kCGPathEOFill;
            else
                mode = kCGPathFill;
        }
        else
        {
            if ( fillStyle == wxODDEVEN_RULE )
                mode = kCGPathEOFillStroke;
            else
                mode = kCGPathFillStroke;
        }
    }
        
	EnsureIsValid();

    if ( m_brush )
        m_brush->Apply(this);

    bool offset = ShouldOffset();
    
    if ( offset )
        CGContextTranslateCTM( m_cgContext, 0.5, 0.5 );

    CGContextAddPath( m_cgContext , (CGPathRef) path->GetNativePath() );
    CGContextDrawPath( m_cgContext , mode );

    if ( offset )
        CGContextTranslateCTM( m_cgContext, -0.5, -0.5 );
}

void wxMacCoreGraphicsContext::FillPath( const wxGraphicsPath *path , int fillStyle )
{
    if ( m_brush == NULL )
        return;
        
	EnsureIsValid();
    
    if ( ((wxMacCoreGraphicsBrush*)m_brush)->IsShading() )
    {
        CGContextSaveGState( m_cgContext );
        CGContextAddPath( m_cgContext , (CGPathRef) path->GetNativePath() );
        CGContextClip( m_cgContext );
        CGContextDrawShading( m_cgContext, ((wxMacCoreGraphicsBrush*)m_brush)->GetShading() );
        CGContextRestoreGState( m_cgContext);
    }
    else
    {	
        m_brush->Apply(this);
        CGContextAddPath( m_cgContext , (CGPathRef) path->GetNativePath() );
        if ( fillStyle == wxODDEVEN_RULE )
            CGContextEOFillPath( m_cgContext );
        else
            CGContextFillPath( m_cgContext );
    }
}

void wxMacCoreGraphicsContext::SetNativeContext( CGContextRef cg )
{
    // we allow either setting or clearing but not replacing
    wxASSERT( m_cgContext == NULL || cg == NULL );

    if ( cg )
        CGContextSaveGState( cg );
    m_cgContext = cg;
}

void wxMacCoreGraphicsContext::Translate( wxDouble dx , wxDouble dy ) 
{
	EnsureIsValid();
	
    CGContextTranslateCTM( m_cgContext, dx, dy );
}

void wxMacCoreGraphicsContext::Scale( wxDouble xScale , wxDouble yScale )
{
	EnsureIsValid();
	
    CGContextScaleCTM( m_cgContext , xScale , yScale );
}

void wxMacCoreGraphicsContext::Rotate( wxDouble angle )
{
	EnsureIsValid();
	
    CGContextRotateCTM( m_cgContext , angle );
}

void wxMacCoreGraphicsContext::DrawBitmap( const wxBitmap &bmp, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
	EnsureIsValid();
	
    CGImageRef image = (CGImageRef)( bmp.CGImageCreate() );
    HIRect r = CGRectMake( x , y , w , h );
    HIViewDrawCGImage( m_cgContext , &r , image );
    CGImageRelease( image );
}

void wxMacCoreGraphicsContext::DrawIcon( const wxIcon &icon, wxDouble x, wxDouble y, wxDouble w, wxDouble h ) 
{
	EnsureIsValid();
	
    CGRect r = CGRectMake( 00 , 00 , w , h );
    CGContextSaveGState( m_cgContext );
    CGContextTranslateCTM( m_cgContext, x , y + h );
    CGContextScaleCTM( m_cgContext, 1, -1 );
    PlotIconRefInContext( m_cgContext , &r , kAlignNone , kTransformNone ,
        NULL , kPlotIconRefNormalFlags , MAC_WXHICON( icon.GetHICON() ) );
    CGContextRestoreGState( m_cgContext );
}

void wxMacCoreGraphicsContext::PushState()
{
	EnsureIsValid();
	
    CGContextSaveGState( m_cgContext );
}

void wxMacCoreGraphicsContext::PopState() 
{
	EnsureIsValid();
	
    CGContextRestoreGState( m_cgContext );
}

// sets the pen
void wxMacCoreGraphicsContext::SetPen( wxGraphicsPen* pen , bool release )
{
    wxGraphicsContext::SetPen( pen, release );
}

// sets the brush for filling
void wxMacCoreGraphicsContext::SetBrush( wxGraphicsBrush* brush , bool release )
{
    wxGraphicsContext::SetBrush( brush, release );
}

// sets the font
void wxMacCoreGraphicsContext::SetFont( wxGraphicsFont* font, bool release )
{
    wxGraphicsContext::SetFont( font, release );
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y ) 
{
    DrawText(str, x, y, 0.0);
}

void wxMacCoreGraphicsContext::DrawText( const wxString &str, wxDouble x, wxDouble y, wxDouble angle ) 
{
    if ( m_font == NULL )
        return;
        
	EnsureIsValid();
	
    OSStatus status = noErr;
    ATSUTextLayout atsuLayout;
    UniCharCount chars = str.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str();
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    ATSUStyle style = (((wxMacCoreGraphicsFont*)m_font)->GetATSUStyle());
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the rotated text") );

    status = ::ATSUSetTransientFontMatching( atsuLayout , true );
    wxASSERT_MSG( status == noErr , wxT("couldn't setup transient font matching") );

    int iAngle = int( angle * RAD2DEG );
    if ( abs(iAngle) > 0 )
    {
        Fixed atsuAngle = IntToFixed( iAngle );
        ATSUAttributeTag atsuTags[] =
        {
            kATSULineRotationTag ,
        };
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( Fixed ) ,
        };
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &atsuAngle ,
        };
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues );
    }

    {
        ATSUAttributeTag atsuTags[] =
        {
            kATSUCGContextTag ,
        };
        ByteCount atsuSizes[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            sizeof( CGContextRef ) ,
        };
        ATSUAttributeValuePtr    atsuValues[sizeof(atsuTags) / sizeof(ATSUAttributeTag)] =
        {
            &m_cgContext ,
        };
        status = ::ATSUSetLayoutControls(atsuLayout , sizeof(atsuTags) / sizeof(ATSUAttributeTag),
            atsuTags, atsuSizes, atsuValues );
    }

    ATSUTextMeasurement textBefore, textAfter;
    ATSUTextMeasurement ascent, descent;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &ascent , &descent );

    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    Rect rect;
/*
    // TODO
    if ( m_backgroundMode == wxSOLID )
    {
        wxGraphicsPath* path = m_graphicContext->CreatePath();
        path->MoveToPoint( drawX , drawY );
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent)) );
        path->AddLineToPoint(
            (int) (drawX + sin(angle / RAD2DEG) * FixedToInt(ascent + descent ) + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY + cos(angle / RAD2DEG) * FixedToInt(ascent + descent) - sin(angle / RAD2DEG) * FixedToInt(textAfter)) );
        path->AddLineToPoint(
            (int) (drawX + cos(angle / RAD2DEG) * FixedToInt(textAfter)) ,
            (int) (drawY - sin(angle / RAD2DEG) * FixedToInt(textAfter)) );

        m_graphicContext->FillPath( path , m_textBackgroundColour );
        delete path;
    }
*/
    x += (int)(sin(angle / RAD2DEG) * FixedToInt(ascent));
    y += (int)(cos(angle / RAD2DEG) * FixedToInt(ascent));

    status = ::ATSUMeasureTextImage( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(x) , IntToFixed(y) , &rect );
    wxASSERT_MSG( status == noErr , wxT("couldn't measure the rotated text") );

    CGContextSaveGState(m_cgContext);
    CGContextTranslateCTM(m_cgContext, x, y);
    CGContextScaleCTM(m_cgContext, 1, -1);
    status = ::ATSUDrawText( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        IntToFixed(0) , IntToFixed(0) );

    wxASSERT_MSG( status == noErr , wxT("couldn't draw the rotated text") );

    CGContextRestoreGState(m_cgContext);

    ::ATSUDisposeTextLayout(atsuLayout);

#if SIZEOF_WCHAR_T == 4
    free( ubuf );
#endif
}
    
void wxMacCoreGraphicsContext::GetTextExtent( const wxString &str, wxDouble *width, wxDouble *height,
                            wxDouble *descent, wxDouble *externalLeading ) const
{
    wxCHECK_RET( m_font != NULL, wxT("wxDC(cg)::DoGetTextExtent - no valid font set") );

    OSStatus status = noErr;
    
    ATSUTextLayout atsuLayout;
    UniCharCount chars = str.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , str.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , str.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) str.wc_str();
#else
    wxWCharBuffer wchar = str.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    ATSUStyle style = (((wxMacCoreGraphicsFont*)m_font)->GetATSUStyle());
    status = ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

    wxASSERT_MSG( status == noErr , wxT("couldn't create the layout of the text") );

    ATSUTextMeasurement textBefore, textAfter;
    ATSUTextMeasurement textAscent, textDescent;

    status = ::ATSUGetUnjustifiedBounds( atsuLayout, kATSUFromTextBeginning, kATSUToTextEnd,
        &textBefore , &textAfter, &textAscent , &textDescent );

    if ( height )
        *height = FixedToInt(textAscent + textDescent);
    if ( descent )
        *descent = FixedToInt(textDescent);
    if ( externalLeading )
        *externalLeading = 0;
    if ( width )
        *width = FixedToInt(textAfter - textBefore);

    ::ATSUDisposeTextLayout(atsuLayout);
}

void wxMacCoreGraphicsContext::GetPartialTextExtents(const wxString& text, wxArrayDouble& widths) const 
{
    widths.Empty();
    widths.Add(0, text.length());

    if (text.empty())
        return;

    ATSUTextLayout atsuLayout;
    UniCharCount chars = text.length();
    UniChar* ubuf = NULL;

#if SIZEOF_WCHAR_T == 4
    wxMBConvUTF16 converter;
#if wxUSE_UNICODE
    size_t unicharlen = converter.WC2MB( NULL , text.wc_str() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , text.wc_str(), unicharlen + 2 );
#else
    const wxWCharBuffer wchar = text.wc_str( wxConvLocal );
    size_t unicharlen = converter.WC2MB( NULL , wchar.data() , 0 );
    ubuf = (UniChar*) malloc( unicharlen + 2 );
    converter.WC2MB( (char*) ubuf , wchar.data() , unicharlen + 2 );
#endif
    chars = unicharlen / 2;
#else
#if wxUSE_UNICODE
    ubuf = (UniChar*) text.wc_str();
#else
    wxWCharBuffer wchar = text.wc_str( wxConvLocal );
    chars = wxWcslen( wchar.data() );
    ubuf = (UniChar*) wchar.data();
#endif
#endif

    ATSUStyle style = (((wxMacCoreGraphicsFont*)m_font)->GetATSUStyle());
    ::ATSUCreateTextLayoutWithTextPtr( (UniCharArrayPtr) ubuf , 0 , chars , chars , 1 ,
        &chars , &style , &atsuLayout );

    for ( int pos = 0; pos < (int)chars; pos ++ )
    {
        unsigned long actualNumberOfBounds = 0;
        ATSTrapezoid glyphBounds;

        // We get a single bound, since the text should only require one. If it requires more, there is an issue
        OSStatus result;
        result = ATSUGetGlyphBounds( atsuLayout, 0, 0, kATSUFromTextBeginning, pos + 1,
            kATSUseDeviceOrigins, 1, &glyphBounds, &actualNumberOfBounds );
        if (result != noErr || actualNumberOfBounds != 1 )
            return;

        widths[pos] = FixedToInt( glyphBounds.upperRight.x - glyphBounds.upperLeft.x );
        //unsigned char uch = s[i];
    }

    ::ATSUDisposeTextLayout(atsuLayout);
}

void * wxMacCoreGraphicsContext::GetNativeContext() 
{
	return m_cgContext;
}

// concatenates this transform with the current transform of this context
void wxMacCoreGraphicsContext::ConcatTransform( const wxGraphicsMatrix* matrix )
{
}

// sets the transform of this context
void wxMacCoreGraphicsContext::SetTransform( const wxGraphicsMatrix* matrix )
{
}

// gets the matrix of this context
void wxMacCoreGraphicsContext::GetTransform( wxGraphicsMatrix* matrix )
{
}

//
// Renderer
//

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsRenderer declaration
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMacCoreGraphicsRenderer : public wxGraphicsRenderer
{
public :
    wxMacCoreGraphicsRenderer() {}

    virtual ~wxMacCoreGraphicsRenderer() {}

	// Context

	virtual wxGraphicsContext * CreateContext( const wxWindowDC& dc);
	
	virtual wxGraphicsContext * CreateContextFromNativeContext( void * context );

	virtual wxGraphicsContext * CreateContextFromNativeWindow( void * window );

	virtual wxGraphicsContext * CreateContext( wxWindow* window );

	// Path
	
    virtual wxGraphicsPath * CreatePath();

	// Matrix
	
	virtual wxGraphicsMatrix * CreateMatrix( wxDouble a=1.0, wxDouble b=0.0, wxDouble c=0.0, wxDouble d=1.0, 
		wxDouble tx=0.0, wxDouble ty=0.0);


    virtual wxGraphicsPen* CreatePen(const wxPen& pen) ;
    
    virtual wxGraphicsBrush* CreateBrush(const wxBrush& brush ) ;
    
    // sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
    virtual wxGraphicsBrush* CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
        const wxColour&c1, const wxColour&c2) ;

    // sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
    // with radius r and color cColor
    virtual wxGraphicsBrush* CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
        const wxColour &oColor, const wxColour &cColor) ;

   // sets the font
    virtual wxGraphicsFont* CreateFont( const wxFont &font , const wxColour &col = *wxBLACK ) ;

private :
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMacCoreGraphicsRenderer)
} ;

//-----------------------------------------------------------------------------
// wxMacCoreGraphicsRenderer implementation
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxMacCoreGraphicsRenderer,wxGraphicsRenderer)

static wxMacCoreGraphicsRenderer gs_MacCoreGraphicsRenderer;

wxGraphicsRenderer* wxGraphicsRenderer::GetDefaultRenderer()
{
	return &gs_MacCoreGraphicsRenderer;
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( const wxWindowDC& dc)
{
   return new wxMacCoreGraphicsContext(this,(CGContextRef)dc.GetWindow()->MacGetCGContextRef() );
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContextFromNativeContext( void * context )
{
    return new wxMacCoreGraphicsContext(this,(CGContextRef)context);
}


wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContextFromNativeWindow( void * window )
{
    return new wxMacCoreGraphicsContext(this,(WindowRef)window);
}

wxGraphicsContext * wxMacCoreGraphicsRenderer::CreateContext( wxWindow* window )
{
	return new wxMacCoreGraphicsContext(this, window );
}

// Path

wxGraphicsPath * wxMacCoreGraphicsRenderer::CreatePath()
{
	return new wxMacCoreGraphicsPath( this );
}


// Matrix

wxGraphicsMatrix * wxMacCoreGraphicsRenderer::CreateMatrix( wxDouble a, wxDouble b, wxDouble c, wxDouble d, 
	wxDouble tx, wxDouble ty)
	
{
    wxMacCoreGraphicsMatrix* m = new wxMacCoreGraphicsMatrix( this );
    m->Set( a,b,c,d,tx,ty ) ;
    return m;
}

wxGraphicsPen* wxMacCoreGraphicsRenderer::CreatePen(const wxPen& pen) 
{
    if ( !pen.Ok() || pen.GetStyle() == wxTRANSPARENT )
        return NULL;
    else
        return new wxMacCoreGraphicsPen( this, pen );
}

wxGraphicsBrush* wxMacCoreGraphicsRenderer::CreateBrush(const wxBrush& brush ) 
{
    if ( !brush.Ok() || brush.GetStyle() == wxTRANSPARENT )
        return NULL;
    else
        return new wxMacCoreGraphicsBrush( this, brush );
}

// sets the brush to a linear gradient, starting at (x1,y1) with color c1 to (x2,y2) with color c2
wxGraphicsBrush* wxMacCoreGraphicsRenderer::CreateLinearGradientBrush( wxDouble x1, wxDouble y1, wxDouble x2, wxDouble y2, 
    const wxColour&c1, const wxColour&c2) 
{
    wxMacCoreGraphicsBrush* brush = new wxMacCoreGraphicsBrush(this);
    brush->CreateLinearGradientBrush(x1, y1, x2, y2, c1, c2);
    return brush;
}

// sets the brush to a radial gradient originating at (xo,yc) with color oColor and ends on a circle around (xc,yc) 
// with radius r and color cColor
wxGraphicsBrush* wxMacCoreGraphicsRenderer::CreateRadialGradientBrush( wxDouble xo, wxDouble yo, wxDouble xc, wxDouble yc, wxDouble radius,
    const wxColour &oColor, const wxColour &cColor) 
{
    wxMacCoreGraphicsBrush* brush = new wxMacCoreGraphicsBrush(this);
    brush->CreateRadialGradientBrush(xo,yo,xc,yc,radius,oColor,cColor);
    return brush;
}

// sets the font
wxGraphicsFont* wxMacCoreGraphicsRenderer::CreateFont( const wxFont &font , const wxColour &col ) 
{
    if ( font.Ok() )
        return new wxMacCoreGraphicsFont( this , font, col );
    else
        return NULL;
}



#endif // wxMAC_USE_CORE_GRAPHICS
