/////////////////////////////////////////////////////////////////////////////
// Name:        wallctrlsurface.h
// Purpose:     Base class for surfaces for wxWallCtrl
// Author:      Mokhtar M. Khorshid
// Modified by: 
// Created:     04/06/2008 04:12:40   
// Copyright:   (c) Mokhtar M. Khorshid
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef WX_WALLCTRLSURFACE_H
#define WX_WALLCTRLSURFACE_H

#include "WallCtrlDataSource.h"
#include "wx/rawbmp.h"
#include "gl/glut.h"

const int BYTES_PER_PIXEL = 4;
const int wxWallCtrlPlaneSurfaceInvalidTexture = -10;

// A map to hold the texture names for items that were previously cached
WX_DECLARE_HASH_MAP(int, int, wxIntegerHash, wxIntegerEqual, Int2IntMap);

// TODO: Check if inheriting from wxObject is correct
class wxWallCtrlSurface:
	public wxObject
{
	DECLARE_DYNAMIC_CLASS(wxWallCtrlSurface)
public:
	wxWallCtrlSurface();
	virtual ~wxWallCtrlSurface(void);

	virtual void Render(const wxSize & windowSize)=0;

	virtual void SetDataSource(wxWallCtrlDataSource * dataSource);

	virtual wxWallCtrlDataSource * GetDataSource() const;

	// Returns true if the specified item has a loaded & cached texture
	bool IsItemTextureLoaded( wxWallCtrlItemID itemID );

	virtual bool TextureLoadingNeeded();


	// TODO: After refactoring, height & width as well as x & y coordinates are not generic enough

	// TODO: Width & Height are redundant since the layer is squared

	// Spiral square layers are layers surrounding an item.
	// Returns the number of columns in the specified layer
	virtual int GetLayerWidth(int layer) const = 0;
	// Returns the number of columns in the specified layer
	virtual int GetLayerHeight(int layer) const = 0;

	// Returns the total number of elements in the specified layer, some of which may be invalid
	virtual int GetLayerItemsCount(int layer) const = 0;

	virtual wxRect GetLayerRect(int layer) const = 0;

	virtual void OnLoadingComplete() = 0;



	// Returns true if the specified coordinates make sense
	virtual bool IsValidPosition(wxPoint pos) = 0;

	wxPoint GetLayerItemPosition( unsigned index ) const;

	// Returns the index of the item with the specified ID or position
	virtual unsigned GetItemIndex(int x, int y) const = 0;
	virtual unsigned GetItemIndex(wxPoint position) const = 0;
	virtual unsigned GetItemIndex(wxWallCtrlItemID itemID) const = 0;


	// Returns the logical position of a specific index
	virtual wxPoint GetItemPosition(unsigned index) const = 0;


	void LoadNextLayerItemTexture();

	// Loads a texture from the DC into the supplied array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromDC( wxMemoryDC &dc, GLubyte * texture, const wxSize& dcSize );

	// Loads a bitmap into a texture array
	// Precondition: Texture must be at least Width * Height * BYTES_PER_PIXEL
	void CreateTextureFromBitmap( wxBitmap bitmap, GLubyte * texture );

	// Returns the texture ID for a specific item, and if it does not exist it loads it
	GLuint GetItemTexture( wxWallCtrlItemID itemID );

	// returns true if we need to load additional items
	virtual bool NeedLoading() const;

protected:
	wxWallCtrlDataSource * m_dataSource;	

	// Layer variables used for partial loading and caching
	unsigned m_currentLayer;		// The current (square) layer that needs to be loaded
	unsigned m_nextLayerItem;		// The index of the next item to be loaded from the current layer
	wxRect m_currentLayerRect;		// The bounding rectangle of the current layer, the layer is at its perimeter
	wxPoint m_nextLayerItemPos;		// The position of the next item to be loaded.
	bool m_loadingNeeded;			// True when we need to load more items and cache them
	int m_renderCount;				// Used to load textures
	int m_rendersBeforeTextureLoad;	// The value after which an attempt to load a new texture will be made
	unsigned m_maxLoadingLayers;	// The max number of layers to pre-load around the currently selected item

	Int2IntMap m_texturesCache;		// Caches the IDs of loaded textures

	// Synchronization
	wxCriticalSection m_texturesCS;	// This CS guards the textures

};

// TODO: Move this class out
class wxWallCtrlLoadingThread: public wxThread
{
public:
	wxWallCtrlLoadingThread(wxWallCtrlSurface * surface): m_surface(surface)
	{		
	};
	// thread execution starts here
	virtual void *Entry()
	{
		while (m_surface->NeedLoading())
		{
			// Load one item
			m_surface->LoadNextLayerItemTexture();

			// See if we should die now
			if (TestDestroy())
			{
				// Loading is not complete, but we must stop here
				return NULL;
			}
		}
		m_surface->OnLoadingComplete();
		return NULL;
	}

	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit()
	{
		// Nothing here
	}

private:
	// Pointer to the surface where loading will occur
	wxWallCtrlSurface * m_surface;



};


#endif