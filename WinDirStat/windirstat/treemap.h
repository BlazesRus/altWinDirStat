// treemap.h	- Declaration of CColorSpace, CTreemap and CTreemapPreview
//
// WinDirStat - Directory Statistics
// Copyright (C) 2003-2004 Bernhard Seifert
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Author: bseifert@users.sourceforge.net, bseifert@daccord.net
//
// Last modified: $Date$

#pragma once

#ifndef TREEMAP_H_INCLUDED
#define TREEMAP_H_INCLUDED

#include "stdafx.h"

class CItemBranch;
class CTreeListItem;


//#define DBL_MAX_100 1.79769e+306
//struct setPixStruct {
//	setPixStruct( int in_x, int in_y, COLORREF in_color ) : ix( std::move( in_x ) ), iy( std::move( in_y ) ), color( std::move( in_color ) ) { }
//	int ix;
//	int iy;
//	COLORREF color;
//	static_assert( sizeof( std::int_fast32_t ) == sizeof( DWORD ), "whoops! need a different color size!" );
//	};
//
//#ifdef GRAPH_LAYOUT_DEBUG
//struct pixBitsSet {
//	
//	};
//#endif

// CTreemap. Can create a treemap. Knows 2 squarification methods: KDirStat-like, SequoiaView-like.
class CTreemap {
public:
	CTreemap( );

#ifdef DEBUG
	~CTreemap( ) {
		const double stack_v_total = ( static_cast< double >( num_times_stack_used ) / static_cast< double >( num_times_heap__used + num_times_stack_used ) );
		const double heap__v_total = ( static_cast< double >( num_times_heap__used ) / static_cast< double >( num_times_heap__used + num_times_stack_used ) );
		const double stack_size_av = ( ( num_times_stack_used != 0 ) ? ( static_cast< double >( total_size_stack_vector ) / static_cast< double >( num_times_stack_used ) ) : 0 );
		const double heap__size_av = ( ( num_times_heap__used != 0 ) ? ( static_cast< double >( total_size_heap__vector ) / static_cast< double >( num_times_heap__used ) ) : 0 );

		if ( m_is_typeview ) {
			if ( num_times_stack_used > 0 ) {
				TRACE( _T( "typeview used the stack\r\n" ) );
				}
			if ( num_times_heap__used > 0 ) {
				TRACE( _T( "typeview used the heap\r\n" ) );
				}
			}
		else {
			TRACE( _T( "number of times DrawCushion used stack: %I64u\r\n" ), std::uint64_t( num_times_stack_used ) );
			TRACE( _T( "number of times DrawCushion used heap : %I64u\r\n" ), std::uint64_t( num_times_heap__used ) );
			if ( ( stack_v_total != 1 ) && ( stack_size_av > 0 ) ) {
				TRACE( _T( "percent of stack uses vs. total       : %f\r\n" ), stack_v_total );
				}
			if ( ( heap__v_total != 1 ) && ( heap__size_av > 0 ) ) {
				TRACE( _T( "percent of heap  uses vs. total       : %f\r\n" ), heap__v_total );
				}
			if ( heap__size_av > 0 ) {
				TRACE( _T( "average size of heap allocation       : %f\r\n" ), heap__size_av );
				}
			if ( stack_size_av > 0 ){
				TRACE( _T( "average size of stack allocation      : %f\r\n" ), stack_size_av );
				}
			}
		}
#else
	~CTreemap( ) = default;
#endif

	void UpdateCushionShading      ( _In_ const bool               newVal                                   );
	void SetOptions                ( _In_ const Treemap_Options&           options                                  );
	void RecurseCheckTree          ( _In_ const CItemBranch* const item                                     ) const;
#ifdef DEBUG
	void validateRectangle         ( _In_ const CItemBranch* const child, _In_ const RECT&             rc  ) const;
#endif
	void compensateForGrid         ( _Inout_    CRect&             rc,    _In_       CDC&               pdc ) const;

	void DrawTreemap               ( _In_ CDC& offscreen_buffer, _Inout_    CRect& rc, _In_ const CItemBranch* const root,  _In_opt_ const Treemap_Options* const options = NULL );
	void DrawTreemapDoubleBuffered ( _In_ CDC& pdc, _In_ const CRect& rc, _In_       CItemBranch* const root,  _In_opt_ const Treemap_Options* const options = NULL );
	void DrawColorPreview          ( _In_ CDC& pdc, _In_ const RECT& rc, _In_ const COLORREF           color, _In_     const Treemap_Options* const options = NULL );

	_Success_( return != NULL ) _Ret_maybenull_ _Must_inspect_result_ CItemBranch* FindItemByPoint( _In_ const CItemBranch* const root, _In_ const WTL::CPoint point ) const;


protected:

	void SetPixels        ( _In_ CDC& offscreen_buffer, _In_reads_( maxIndex ) _Pre_readable_size_( maxIndex ) const COLORREF* const pixles, _In_ const int&   yStart, _In_ const int& xStart, _In_ const int& yEnd, _In_ const int& xEnd,   _In_ const int rcWidth, _In_ const size_t offset, const size_t maxIndex, _In_ const int rcHeight ) const;

	void RecurseDrawGraph ( _In_ CDC& offscreen_buffer, _In_ const CItemBranch* const     item,   _In_ const CRect& rc,     _In_ const bool asroot, _In_ const DOUBLE ( &psurface )[ 4 ], _In_ const DOUBLE h ) const;


	void DrawCushion      ( _In_ CDC& offscreen_buffer, _In_ const RECT&              rc,        _In_ const DOUBLE ( &surface )[ 4 ], _In_                    const COLORREF col,       _In_ _In_range_( 0, 1 ) const DOUBLE  brightness ) const;
	void DrawSolidRect    ( _In_ CDC& pdc, _In_ const RECT&              rc,        _In_ const COLORREF        col,            _In_ _In_range_( 0, 1 ) const DOUBLE   brightness ) const;
	void DrawChildren     ( _In_ CDC& pdc, _In_ const CItemBranch*  const parent,    _In_ const DOUBLE ( &surface )[ 4 ], _In_                    const DOUBLE   h          ) const;
	


	//KDS -> KDirStat
	DOUBLE KDS_CalcNextRow ( _In_ const CItemBranch* const parent, _In_ _In_range_( 0, INT_MAX ) const size_t nextChild,  _In_ _In_range_( 0, 32767 ) const DOUBLE width, _Out_ size_t& childrenUsed, _Inout_ std::vector<DOUBLE>& childWidth, const std::uint64_t parentSize ) const;
		
	bool KDS_PlaceChildren ( _In_ const CItemBranch* const parent, _Inout_    std::vector<double>& childWidth, _Inout_ std::vector<double>& rows,            _Inout_    std::vector<size_t>& childrenPerRow ) const;
	void KDS_DrawChildren  ( _In_ CDC&  pdc,                       _In_ const CItemBranch* const parent,          _In_ const DOUBLE       ( &surface )[ 4 ], _In_ const DOUBLE h ) const;
	
	void KDS_DrawSingleRow( _In_ const std::vector<size_t>& childrenPerRow, _In_ _In_range_( 0, SIZE_T_MAX ) const size_t& row, _In_ const std::vector<CTreeListItem*>& parent_vector_of_children, _Inout_ _In_range_( 0, SIZE_T_MAX ) size_t& c, _In_ const std::vector<double>& childWidth, _In_ const int& width, _In_ const bool& horizontalRows, _In_ const int& bottom, _In_ const double& top, _In_ const CRect& rc, _In_ CDC& pdc, _In_ const DOUBLE( &surface )[ 4 ], _In_ const DOUBLE& h, _In_ const CItemBranch* const parent ) const;


	//SQV -> SequoiaView
	void SQV_DrawChildren  ( _In_ CDC&  pdc,                       _In_ const CItemBranch* const parent, _In_ const DOUBLE ( &surface )[ 4 ], _In_ const DOUBLE h ) const;
	void RenderLeaf        ( _In_ CDC&  offscreen_buffer,          _In_ const CItemBranch* const item,   _In_ const DOUBLE ( &surface )[ 4 ]                   ) const;
	void RenderRectangle   ( _In_ CDC&  offscreen_buffer,          _In_ const RECT&             rc,     _In_ const DOUBLE ( &surface )[ 4 ], _In_ DWORD color ) const;

	//if we pass horizontal by reference, compiler produces `cmp    BYTE PTR [r15], 0` for `if ( horizontal )`, pass by value generates `test    r15b, r15b`
	void SQV_put_children_into_their_places( _In_ const size_t& rowBegin, _In_ const size_t& rowEnd, _In_ const std::vector<CTreeListItem*>& parent_vector_of_children, _Inout_ std::map<std::uint64_t, std::uint64_t>& sizes, _In_ const std::uint64_t& sumOfSizesOfChildrenInRow, _In_ const int& heightOfNewRow, _In_ const bool horizontal, _In_ const CRect& remaining, _In_ CDC& pdc, _In_ const DOUBLE( &surface )[ 4 ], _In_ const DOUBLE& scaleFactor, _In_ const DOUBLE h, _In_ const int& widthOfRow ) const;

	void AddRidge( _In_ const RECT& rc, _Inout_ DOUBLE ( &surface )[ 4 ], _In_ const DOUBLE h ) const;
	
	bool IsCushionShading( ) const;

private:

	void DrawCushion_with_heap( _In_ const size_t loop_rect_start_outer, _In_ const size_t loop_rect__end__outer, _In_ const size_t loop_rect_start_inner, _In_ const size_t loop_rect__end__inner, _In_ const size_t inner_stride, _In_ const size_t offset, _In_ _In_range_( 512, SIZE_T_MAX ) const size_t vecSize, _In_ CDC& offscreen_buffer, const _In_ CRect& rc, _In_ _In_range_( 0, 1 ) const DOUBLE brightness, _In_ const size_t largestIndexWritten, _In_ const DOUBLE surface_0, _In_ const DOUBLE surface_1, _In_ const DOUBLE surface_2, _In_ const DOUBLE surface_3, _In_ const DOUBLE Is, _In_ const DOUBLE Ia, _In_ const DOUBLE colR, _In_ const DOUBLE colG, _In_ const DOUBLE colB ) const;

	void DrawCushion_with_stack( _In_ const size_t loop_rect_start_outer, _In_ const size_t loop_rect__end__outer, _In_ const size_t loop_rect_start_inner, _In_ const size_t loop_rect__end__inner, _In_ const size_t inner_stride, _In_ const size_t offset, _In_ _In_range_( 1, 512 ) const size_t vecSize, _In_ CDC& offscreen_buffer, const _In_ CRect& rc, _In_ _In_range_( 0, 1 ) const DOUBLE brightness, _In_ const size_t largestIndexWritten, _In_ const DOUBLE surface_0, _In_ const DOUBLE surface_1, _In_ const DOUBLE surface_2, _In_ const DOUBLE surface_3, _In_ const DOUBLE Is, _In_ const DOUBLE Ia, _In_ const DOUBLE colR, _In_ const DOUBLE colG, _In_ const DOUBLE colB ) const;

public:
	
	//C4820: 'CTreemap' : '7' bytes padding added after data member 'CTreemap::IsCushionShading_current'
	bool IsCushionShading_current : 1;

	Treemap_Options   m_options;	// Current options
#ifdef DEBUG
	mutable std::uint64_t total_size_stack_vector = 0;
	mutable std::uint64_t total_size_heap__vector = 0;
	mutable rsize_t       num_times_heap__used    = 0;
	mutable rsize_t       num_times_stack_used    = 0;
	bool                  m_is_typeview           = false;
#endif

protected:

	DOUBLE    m_Lx;// Derived parameters
	DOUBLE    m_Ly;
	DOUBLE    m_Lz;
public:

#ifdef GRAPH_LAYOUT_DEBUG
	void debugSetPixel( CDC& pdc, int a, int b, COLORREF c ) const;
	mutable std::unique_ptr<std::vector<std::vector<bool>>> bitSetMask;
	mutable int numCalls;
#endif

	};


#else
#error 555
#endif

// $Log$
// Revision 1.6  2004/11/29 07:07:47  bseifert
// Introduced SRECT. Saves 8 Bytes in sizeof(CItem). Formatting changes.