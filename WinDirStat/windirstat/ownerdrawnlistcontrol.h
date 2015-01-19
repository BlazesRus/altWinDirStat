// ownerdrawnlistcontrol.h	- Declaration of COwnerDrawnListCtrl and COwnerDrawnListItem
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

#ifndef OWNERDRAWNLISTCONTROL_H
#define OWNERDRAWNLISTCONTROL_H

#pragma once
#include "stdafx.h"
#include "mainframe.h"
#include "globalhelpers.h"
#include "options.h"
#include "windirstat.h"

class COwnerDrawnListItem;
class COwnerDrawnListCtrl;




// COwnerDrawnListItem. An item in a COwnerDrawnListCtrl. Some columns (subitems) may be owner drawn (DrawSubitem() returns true), COwnerDrawnListCtrl draws the texts (GetText()) of all others.
// DrawLabel() draws a standard label (width image, text, selection and focus rect)
class COwnerDrawnListItem {
public:

	COwnerDrawnListItem( _In_z_ _Readable_elements_( length ) PCWSTR name, const std::uint16_t length ) : m_name( name ), m_name_length( length ) { }
	COwnerDrawnListItem( const COwnerDrawnListItem& in ) = delete;
	COwnerDrawnListItem& operator=( const COwnerDrawnListItem& in ) = delete;
	COwnerDrawnListItem( ) = default;
	virtual ~COwnerDrawnListItem( ) = default;

	INT          compare_interface            ( _In_ const COwnerDrawnListItem* const other, RANGE_ENUM_COL const column::ENUM_COL subitem ) const {
		return Compare( other, subitem );
		}

	INT          CompareS                     ( _In_ const COwnerDrawnListItem* const other, _In_ const SSorting& sorting ) const {
		if ( sorting.column1 == column::COL_NAME ) {
			const auto sort_result = signum( wcscmp( m_name.get( ), other->m_name.get( ) ) );
		
			if ( sort_result != 0 ) {
				return sort_result;
				}
			}

		auto r_1 = compare_interface( other, sorting.column1 );
		if ( abs( r_1 ) < 2 && !sorting.ascending1 ) {
			r_1 = -r_1;
			}
		auto r_2 = r_1;

		if ( r_2 == 0 && sorting.column2 != sorting.column1 ) {
			r_2 = compare_interface( other, sorting.column2 );
		
			if ( abs( r_2 ) < 2 && !sorting.ascending2 ) {
				r_2 = -r_2;
				}
			}
		return r_2;
		}

	//defined at bottom of THIS file.
	void         DrawSelection                ( _In_ const COwnerDrawnListCtrl* const list, _In_ CDC& pdc,       _In_ RECT rc, _In_ const UINT state                       ) const;

	bool         DrawSubitem_                 ( RANGE_ENUM_COL const column::ENUM_COL subitem, _In_ CDC& pdc, _In_ RECT rc, _In_ const UINT state, _Out_opt_ INT* const width, _Inout_ INT* const focusLeft ) const {
		return DrawSubitem( subitem, pdc, rc, state, width, focusLeft );
		}

	COLORREF    item_text_color( ) const {
		return ItemTextColor( );
		}

	COLORREF     default_item_text_color      ( ) const {
		return GetSysColor( COLOR_WINDOWTEXT );
		}
	
	_Must_inspect_result_ _Success_( SUCCEEDED( return ) ) 
	HRESULT      GetText_WriteToStackBuffer   ( RANGE_ENUM_COL const column::ENUM_COL subitem, WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_text, _In_ const rsize_t strSize, _Out_ _On_failure_( _Post_valid_ )rsize_t& sizeBuffNeed, _Out_ rsize_t& chars_written ) const {
		const HRESULT res = Text_WriteToStackBuffer( subitem, psz_text, strSize, sizeBuffNeed, chars_written );
	#ifdef DEBUG
		if ( SUCCEEDED( res ) ) {
			const auto len_dat_str = wcslen( psz_text );
			ASSERT( chars_written == len_dat_str );
			}
	#endif
		return res;
		}

protected:
	//defined at bottom of THIS file.
	void         DrawLabel                    ( _In_ COwnerDrawnListCtrl* const list, _In_ CDC& pdc, _In_ CRect& rc, _In_ const UINT state, _Out_opt_ INT* const width, _Inout_ INT* const focusLeft, _In_ const bool indent ) const;
	
	//defined at bottom of THIS file.
	void         DrawHighlightSelectBackground( _In_ const RECT& rcLabel, _In_ const RECT& rc, _In_ const COwnerDrawnListCtrl* const list, _In_ CDC& pdc, _Inout_ COLORREF& textColor ) const;


	void         AdjustLabelForMargin         ( _In_ const RECT& rcRest, _Inout_ CRect& rcLabel ) const {
		rcLabel.InflateRect( LABEL_INFLATE_CX, 0 );
		rcLabel.top    = rcRest.top + static_cast<LONG>( LABEL_Y_MARGIN );
		rcLabel.bottom = rcRest.bottom - static_cast<LONG>( LABEL_Y_MARGIN );
		}

private:
	virtual INT          Compare( _In_ const COwnerDrawnListItem* const other, RANGE_ENUM_COL const column::ENUM_COL subitem ) const = 0;

	_Must_inspect_result_ _Success_( SUCCEEDED( return ) )
	virtual HRESULT      Text_WriteToStackBuffer( RANGE_ENUM_COL const column::ENUM_COL subitem, WDS_WRITES_TO_STACK( strSize, chars_written ) PWSTR psz_text, _In_ const rsize_t strSize, _Out_ _On_failure_( _Post_valid_ ) rsize_t& sizeBuffNeed, _Out_ rsize_t& chars_written ) const = 0;

	//CItemBranch is the only non-default behavior here!
	virtual COLORREF     ItemTextColor( ) const = 0;

	// Return value is true, if the item draws itself. width != NULL -> only determine width, do not draw. If focus rectangle shall not begin leftmost, set *focusLeft to the left edge of the desired focus rectangle.
	virtual bool         DrawSubitem            ( RANGE_ENUM_COL const column::ENUM_COL subitem, _In_ CDC& pdc, _In_ CRect rc, _In_ const UINT state, _Out_opt_ INT* const width, _Inout_ INT* const focusLeft ) const = 0;

	public:
	_Field_z_ _Field_size_( m_name_length ) std::unique_ptr<_Null_terminated_ const wchar_t[]> m_name;
	                                        std::uint16_t                                      m_name_length;
	};

namespace {
	static INT CALLBACK _CompareFunc( _In_ const LPARAM lParam1, _In_ const LPARAM lParam2, _In_ const LPARAM lParamSort ) {
		const auto sorting = reinterpret_cast<const SSorting*>( lParamSort );
		return reinterpret_cast<const COwnerDrawnListItem*>( lParam1 )->CompareS( reinterpret_cast<const COwnerDrawnListItem*>( lParam2 ), *sorting );
		}
	}


// COwnerDrawnListCtrl. Must be report view. Deals with COwnerDrawnListItems.
// Can have a grid or not (own implementation, don't set LVS_EX_GRIDLINES). Flicker-free.
class COwnerDrawnListCtrl : public CListCtrl {
	/*
	
	#define DECLARE_DYNAMIC(class_name) \
	public: \
		static const CRuntimeClass class##class_name; \
		virtual CRuntimeClass* GetRuntimeClass() const; \


	#define DECLARE_DYNAMIC(COwnerDrawnListCtrl) \
	public: \
		static const CRuntimeClass classCOwnerDrawnListCtrl; \
		virtual CRuntimeClass* GetRuntimeClass() const; \

	*/

	//manually expanded DECLARE_DYNAMIC
public:
	static const CRuntimeClass classCOwnerDrawnListCtrl;
	virtual CRuntimeClass* GetRuntimeClass( ) const {
		return ((CRuntimeClass*)(&COwnerDrawnListCtrl::classCOwnerDrawnListCtrl) );
		}

	//DECLARE_DYNAMIC(COwnerDrawnListCtrl)
	COwnerDrawnListCtrl ( _In_z_ PCWSTR name, _In_range_( 0, UINT_MAX ) const UINT rowHeight ) : m_persistent_name( name ), m_indicatedColumn( -1 ), m_rowHeight( rowHeight ), m_showGrid( false ), m_showStripes( false ), m_showFullRowSelection( false ), m_frameptr( GetMainFrame( ) ) {
		ASSERT( rowHeight > 0 );
		InitializeColors( );
		}

	virtual ~COwnerDrawnListCtrl( ) = default;

	COwnerDrawnListCtrl& operator=( const COwnerDrawnListCtrl& in ) = delete;
	COwnerDrawnListCtrl( const COwnerDrawnListCtrl& in ) = delete;

	void LoadPersistentAttributes( ) {
		TRACE( _T( "Loading persistent attributes....\r\n" ) );

		const auto itemCount_default_type = GetHeaderCtrl( )->GetItemCount( );
		const auto itemCount = static_cast<size_t>( itemCount_default_type );
		const rsize_t countArray = 10;
	
		if ( countArray <= itemCount ) {
			TRACE( _T( "%i <= %i !!!! Something is REALLY wrong!!!\r\n" ), static_cast<int>( countArray ), static_cast<int>( itemCount ) );
			displayWindowsMsgBoxWithMessage( std::wstring( L"countArray <= itemCount !!!! Something is REALLY wrong!!!" ) );
			std::terminate( );
			}

		ASSERT( countArray > itemCount );
	
		INT col_order_array[ countArray ] = { 0 };

		const auto res_2 = GetColumnOrderArray( col_order_array, itemCount_default_type );
		if ( res_2 == 0 ) {
			std::terminate( );
			}

		CPersistence::GetColumnOrder( m_persistent_name, col_order_array, itemCount );

		const auto res2 = SetColumnOrderArray( static_cast<int>( itemCount ), col_order_array );
		if ( res2 == 0 ) {
			std::terminate( );
			}

		for ( size_t i = 0; i < itemCount; i++ ) {
			col_order_array[ i ] = GetColumnWidth( static_cast<int>( i ) );
			}
		CPersistence::GetColumnWidths( m_persistent_name, col_order_array, itemCount );

		for ( size_t i = 0; i < itemCount; i++ ) {
			// To avoid "insane" settings we set the column width to maximal twice the default width.
			const auto maxWidth = GetColumnWidth( static_cast<int>( i ) ) * 2;
		
	#pragma push_macro("min")
	#undef min
			const auto w = std::min( col_order_array[ i ], maxWidth );
	#pragma pop_macro("min")

			VERIFY( SetColumnWidth( static_cast<int>( i ), w ) );
			}
		// We refrain from saving the sorting because it is too likely, that users start up with insane settings and don't get it.
		}

	void SavePersistentAttributes( ) {
		const rsize_t col_array_size = 128;
		int col_array[ col_array_size ] = { 0 };

		const auto itemCount = GetHeaderCtrl( )->GetItemCount( );

		if ( !( itemCount < col_array_size ) ) {
			std::terminate( );
			}

		const auto get_res = GetColumnOrderArray( col_array, itemCount );

		if ( get_res == 0 ) {
			std::terminate( );
			}


		CPersistence::SetColumnOrder( m_persistent_name, col_array, static_cast<rsize_t>( itemCount ) );

		for ( INT_PTR i = 0; i < itemCount; i++ ) {
			col_array[ i ] = GetColumnWidth( static_cast<int>( i ) );
			}
		CPersistence::SetColumnWidths( m_persistent_name, col_array, static_cast<rsize_t>( itemCount ) );
		}

	void SortItems( ) {
		VERIFY( CListCtrl::SortItems( &_CompareFunc, reinterpret_cast<DWORD_PTR>( &m_sorting ) ) );
		auto hditem =  zeroInitHDITEM( );

		auto thisHeaderCtrl = GetHeaderCtrl( );

		//http://msdn.microsoft.com/en-us/library/windows/desktop/bb775247(v=vs.85).aspx specifies 260
		const rsize_t text_char_count = 260u;


		wchar_t text_buffer_1[ text_char_count ] = { 0 };
		hditem.mask       = HDI_TEXT;
		//hditem.pszText    = text.GetBuffer( text_char_count );
		hditem.pszText    = text_buffer_1;
		hditem.cchTextMax = text_char_count;

		if ( m_indicatedColumn != -1 ) {
			VERIFY( thisHeaderCtrl->GetItem( m_indicatedColumn, &hditem ) );
			//text.ReleaseBuffer( );
			//text           = text.Mid( 2 );
			//PWSTR text_str = &( text_buffer_1[ 2 ] );

			//hditem.pszText = text.GetBuffer( text_char_count );
			hditem.pszText = &( text_buffer_1[ 2 ] );
			hditem.cchTextMax = ( text_char_count - 3 );
			VERIFY( thisHeaderCtrl->SetItem( m_indicatedColumn, &hditem ) );
			//text.ReleaseBuffer( );
			}

		//hditem.pszText = text.GetBuffer( text_char_count );
		hditem.pszText = text_buffer_1;
		hditem.cchTextMax = text_char_count;
		VERIFY( thisHeaderCtrl->GetItem( m_sorting.column1, &hditem ) );
		//text.ReleaseBuffer( );

		wchar_t text_buffer_2[ text_char_count ] = { 0 };
		const HRESULT fmt_res = StringCchPrintfW( text_buffer_2, text_char_count, L"%s%s", ( ( m_sorting.ascending1 ) ? _T( "< " ) : _T( "> " ) ), text_buffer_1 );
		//text = ( ( m_sorting.ascending1 ) ? _T( "< " ) : _T( "> " ) ) + text;
		ASSERT( SUCCEEDED( fmt_res ) );
		if ( !SUCCEEDED( fmt_res ) ) {
			_CrtDbgBreak( );
			std::terminate( );
			}
		//hditem.pszText = text.GetBuffer( text_char_count );
		hditem.pszText = text_buffer_2;
		VERIFY( thisHeaderCtrl->SetItem( m_sorting.column1, &hditem ) );

		//goddamnit, static_assert is AWESOME when combined with template metaprogramming!
		static_assert( std::is_convertible<std::underlying_type<column::ENUM_COL>::type, decltype( m_indicatedColumn )>::value, "m_sorting.column1 MUST be convertible to an ENUM_COL!" );
		//static_assert( std::is_convertible<std::underlying_type<column::ENUM_COL>::type, std::int8_t>::value, "m_sorting.column1 MUST be convertible to an ENUM_COL!" );
	
		m_indicatedColumn = static_cast<std::int8_t>( m_sorting.column1 );
		//text.ReleaseBuffer( );

		}

	_Success_( return != -1 ) _Ret_range_( -1, INT_MAX )
	INT  FindListItem( _In_ const COwnerDrawnListItem* const item ) const {
		auto fi   = zeroInitLVFINDINFO( );
		fi.flags  = LVFI_PARAM;
		fi.lParam = reinterpret_cast<LPARAM>( item );

		const auto i = static_cast<INT>( FindItem( &fi ) );

		return i;
		}

	void OnColumnsInserted( ) {
		/*
		  This method MUST be called BEFORE the Control is shown.
		*/
		// The pacmen shall not draw over our header control.
		VERIFY( ModifyStyle( 0, WS_CLIPCHILDREN ) );

		// Where does the 1st Item begin vertically?
		if ( GetItemCount( ) > 0 ) {
			RECT rc;
			VERIFY( GetItemRect( 0, &rc, LVIR_BOUNDS ) );
			m_yFirstItem = rc.top;
			}
		else {
			InsertItem( 0, _T( "_tmp" ), 0 );
			RECT rc;
			VERIFY( GetItemRect( 0, &rc, LVIR_BOUNDS ) );
			VERIFY( DeleteItem( 0 ) );
			m_yFirstItem = rc.top;
			}

		LoadPersistentAttributes( );
		}

	void AdjustColumnWidth( RANGE_ENUM_COL const column::ENUM_COL col ) {
		WTL::CWaitCursor wc;

		INT width = 10;
		const auto itemCount = GetItemCount( );
		for ( INT i = 0; i < itemCount; i++ ) {
			ASSERT( itemCount == GetItemCount( ) );
			const auto item = GetItem( i );
			if ( item == NULL ) {
				std::terminate( );
				//`/analyze` is confused.
				return;
				}
			const auto w = GetSubItemWidth( item, col );
			if ( w > width ) {
				width = w;
				}
			}
		VERIFY( SetColumnWidth( col, width + 5 ) );
		}
	
	void InsertListItem( _In_ _In_range_( 0, INT32_MAX ) const INT_PTR i, _In_ const COwnerDrawnListItem* const item ) {
		auto lvitem = partInitLVITEM( );

		lvitem.mask = LVIF_TEXT | LVIF_PARAM;
		lvitem.iItem   = static_cast<int>( i );
		lvitem.pszText = LPSTR_TEXTCALLBACKW;
		lvitem.iImage  = I_IMAGECALLBACK;
		lvitem.lParam  = reinterpret_cast< LPARAM >( item );

		VERIFY( i == CListCtrl::InsertItem( &lvitem ) );
		}
	
	void AddExtendedStyle( _In_ const DWORD exStyle ) {
		SetExtendedStyle( GetExtendedStyle( ) bitor exStyle );
		}
	
	//COLORREF GetItemSelectionBackgroundColor ( _In_ _In_range_( 0, INT_MAX )   const INT i  ) const;

	COLORREF GetItemSelectionTextColor( _In_ _In_range_( 0, INT_MAX )   const INT i ) const {
		auto selected = ( GetItemState( i, LVIS_SELECTED ) & LVIS_SELECTED ) != 0;
		if ( selected && m_showFullRowSelection && ( HasFocus( ) || IsShowSelectionAlways( ) ) ) {
			return GetHighlightTextColor( );
			}
		return GetSysColor( COLOR_WINDOWTEXT );
		}
	
	CRect GetWholeSubitemRect( _In_ const INT item, _In_ const INT subitem ) const {
		CRect rc;
		if ( subitem == 0 ) {
			// Special case column 0:
			// If we did GetSubItemRect(item 0, LVIR_LABEL, rc) and we have an image list, then we would get the rectangle excluding the image.
			HDITEM hditem = zeroInitHDITEM( );

			hditem.mask = HDI_WIDTH;
			VERIFY( GetHeaderCtrl( )->GetItem( 0, &hditem ) );

			VERIFY( GetItemRect( item, rc, LVIR_LABEL ) );
			rc.left = rc.right - hditem.cxy;
			}
		else {
			VERIFY( GetSubItemRect( item, subitem, LVIR_LABEL, rc ) );
			}

		if ( m_showGrid ) {
			rc.right--;
			rc.bottom--;
			}
		return rc;
		}

	_Must_inspect_result_ _Success_( return != NULL ) _Ret_maybenull_
	COwnerDrawnListItem* GetItem( _In_ _In_range_( 0, INT_MAX )   const int i ) const {
		ASSERT( i < GetItemCount( ) );
		const auto itemCount = GetItemCount( );
		if ( i < itemCount ) {
			return reinterpret_cast< COwnerDrawnListItem* >( GetItemData( static_cast<int>( i ) ) );
			}
		return NULL;
		}

	void SetSorting( RANGE_ENUM_COL const column::ENUM_COL sortColumn, _In_ const bool ascending ) {
		m_sorting.ascending2 = m_sorting.ascending1;
		m_sorting.column1    = sortColumn;
		m_sorting.column2    = m_sorting.column1;
		m_sorting.ascending1 = ascending;
		}

	void ShowFullRowSelection( _In_ const bool show ) {
		m_showFullRowSelection = show;
		if ( IsWindow( m_hWnd ) ) {
			InvalidateRect( NULL );
			}
		}

	void ShowGrid( _In_ const bool show ) {
		m_showGrid = show;
		if ( IsWindow( m_hWnd ) ) {
			InvalidateRect( NULL );
			}
		}

	void ShowStripes( _In_ const bool show ) {
		m_showStripes = show;
		if ( IsWindow( m_hWnd ) ) {
			InvalidateRect( NULL );
			}
		}


	COLORREF GetHighlightColor( ) const {
		if ( HasFocus( ) ) {
			return GetSysColor( COLOR_HIGHLIGHT );
			}
		return RGB( 190, 190, 190 );
		}
	
	_Success_( return != COLORREF( 0 ) )
	COLORREF GetHighlightTextColor( ) const {
		if ( HasFocus( ) ) {
			return GetSysColor( COLOR_HIGHLIGHTTEXT );
			}
		return RGB( 0, 0, 0 );
		}


	_Success_( return != COLORREF( 0 ) )
	COLORREF GetItemBackgroundColor( _In_ _In_range_( 0, UINT_MAX ) const UINT i ) const {
		return ( IsItemStripeColor( i ) ? m_stripeColor : m_windowColor );
		}

	static_assert( INT_MAX < UINT_MAX, "" );
	bool IsItemStripeColor( _In_ _In_range_( 0, UINT_MAX ) const UINT i ) const {
		return ( m_showStripes && ( i % 2 != 0 ) );
		}

	//_Success_( return != COLORREF( 0 ) )
	//COLORREF GetItemBackgroundColor( _In_ const COwnerDrawnListItem* const item ) const {
	//	const auto itemPos = FindListItem( item );
	//	if ( itemPos != -1 ) {
	//		return GetItemBackgroundColor( itemPos );
	//		}
	//	return COLORREF( 0 );
	//	}

	//_Success_( return != COLORREF( 0 ) )
	//COLORREF GetItemSelectionBackgroundColor( _In_ const COwnerDrawnListItem* const item ) const {
	//	const auto itemPos = FindListItem( item );
	//	if ( itemPos != -1 ) {
	//		return GetItemSelectionBackgroundColor( itemPos );
	//		}
	//	return COLORREF( 0 );
	//	}

	bool IsItemStripeColor( _In_ const COwnerDrawnListItem* const item ) const {
		const auto itemPos = FindListItem( item );
		if ( itemPos >= 0 ) {
#pragma warning(suppress: 28020)
			return IsItemStripeColor( static_cast<UINT>( itemPos ) );
			}
		return COLORREF( 0 );
		}
	bool HasFocus( ) const {
		return ::GetFocus( ) == m_hWnd;
		}
	bool IsShowSelectionAlways( ) const {
		return ( GetStyle( ) bitand LVS_SHOWSELALWAYS ) != 0;
		}

	bool AscendingDefault( RANGE_ENUM_COL const column::ENUM_COL column ) const {
		return GetAscendingDefault( column );
		}

protected:

	virtual void DrawItem( _In_ PDRAWITEMSTRUCT pdis ) override final {
		const auto item = reinterpret_cast< COwnerDrawnListItem *> ( pdis->itemData );
		const auto pdc = CDC::FromHandle( pdis->hDC );
		const auto bIsFullRowSelection = m_showFullRowSelection;
		ASSERT_VALID( pdc );
		CRect rcItem( pdis->rcItem );
		if ( m_showGrid ) {
			rcItem.bottom--;
			rcItem.right--;
			}

		CDC dcmem; //compiler seems to vectorize this!

		VERIFY( dcmem.CreateCompatibleDC( pdc ) );
		CBitmap bm;
		VERIFY( bm.CreateCompatibleBitmap( pdc, ( rcItem.Width( ) ), ( rcItem.Height( ) ) ) );
		CSelectObject sobm( dcmem, bm );

		dcmem.FillSolidRect( rcItem - rcItem.TopLeft( ), GetItemBackgroundColor( pdis->itemID ) ); //NOT vectorized!

		const bool drawFocus = ( pdis->itemState bitand ODS_FOCUS ) != 0 && HasFocus( ) && bIsFullRowSelection; //partially vectorized

		std::vector<INT> order;
	
		const bool showSelectionAlways = IsShowSelectionAlways( );
		const auto thisHeaderCtrl = GetHeaderCtrl( );//HORRENDOUSLY slow. Pessimisation of memory access, iterates (with a for loop!) over a map. MAXIMUM branch prediction failures! Maximum Bad Speculation stalls!

		const auto resize_size = thisHeaderCtrl->GetItemCount( );
		if ( resize_size == -1 ) {
			std::terminate( );
			}
		order.resize( static_cast<size_t>( resize_size ) );
		ASSERT( order.size( ) < 10 );
		VERIFY( thisHeaderCtrl->GetOrderArray( order.data( ), static_cast<int>( order.size( ) ) )) ;

	#ifdef DEBUG
		for ( rsize_t i = 0; i < order.size( ) - 1; ++i ) {
			if ( static_cast<INT>( i ) != order[ i ] ) {
				TRACE( _T( "order[%i]: %i \r\n" ), static_cast<INT>( i ), order[ i ] );
				}
			}
	#endif
		const auto thisLoopSize = order.size( );
		if ( is_right_aligned_cache.empty( ) ) {
		
			is_right_aligned_cache.reserve( static_cast<size_t>( thisLoopSize ) );
			for ( size_t i = 0; i < thisLoopSize; ++i ) {
				is_right_aligned_cache.push_back( IsColumnRightAligned( static_cast<int>( i ) ) );
				}
			}
		auto rcFocus = rcItem;
		rcFocus.DeflateRect( 0, LABEL_Y_MARGIN - 1 );

		//Not vectorized: 1304, loop includes assignments of different sizes
		for ( size_t i = 0; i < thisLoopSize; i++ ) {
			//iterate over columns, properly populate fields.
			ASSERT( order[ i ] == static_cast<INT>( i ) );
			static_assert( std::is_convertible< INT, std::underlying_type<column::ENUM_COL>::type>::value, "" );
			const auto subitem = static_cast<column::ENUM_COL>( order[ i ] );
			const auto rc = GetWholeSubitemRect( static_cast<INT>( pdis->itemID ), subitem );
		
			/*
			inline CRect CRect::operator-(_In_ POINT pt) const throw()
			{
				CRect rect(*this);
				::OffsetRect(&rect, -pt.x, -pt.y);
				return rect;
			}
			*/
			//CRect rcDraw = rc - rcItem.TopLeft( );
			const auto rcItem_TopLeft_x = rcItem.TopLeft( ).x;
			const auto rcItem_TopLeft_y = rcItem.TopLeft( ).y;
			RECT temp_rc = rc;
			VERIFY( ::OffsetRect( &temp_rc, -rcItem_TopLeft_x, -rcItem_TopLeft_y ) );
			RECT rcDraw = temp_rc;

			INT focusLeft = rcDraw.left;
			if ( !item->DrawSubitem_( subitem, dcmem, rcDraw, pdis->itemState, NULL, &focusLeft ) ) {//if DrawSubItem returns true, item draws self. Therefore `!item->DrawSubitem` is true when item DOES NOT draw self
				DoDrawSubItemBecauseItCannotDrawItself( item, subitem, dcmem, rcDraw, pdis, showSelectionAlways, bIsFullRowSelection, is_right_aligned_cache );
				}

			if ( focusLeft > rcDraw.left ) {
				if ( drawFocus && i > 0 ) {
					pdc->DrawFocusRect( rcFocus );
					}
				rcFocus.left = focusLeft;
				}
			rcFocus.right = rcDraw.right;
			VERIFY( pdc->BitBlt( ( rcItem.left + rcDraw.left ), ( rcItem.top + rcDraw.top ), ( rcDraw.right - rcDraw.left ), ( rcDraw.bottom - rcDraw.top ), &dcmem, rcDraw.left, rcDraw.top, SRCCOPY ) );
			}
		if ( drawFocus ) {
			pdc->DrawFocusRect( rcFocus );
			}
		//VERIFY( dcmem.DeleteDC( ) );

		}

	void DoDrawSubItemBecauseItCannotDrawItself( _In_ const COwnerDrawnListItem* const item, _In_ _In_range_( 0, INT_MAX ) const column::ENUM_COL subitem, _In_ CDC& dcmem, _In_ RECT& rcDraw, _In_ const PDRAWITEMSTRUCT& pdis, _In_ const bool showSelectionAlways, _In_ const bool bIsFullRowSelection, const std::vector<bool>& is_right_aligned_cache ) const {
		item->DrawSelection( this, dcmem, rcDraw, pdis->itemState );
		/*
		inline void CRect::DeflateRect(
			_In_ int x,
			_In_ int y) throw()
		{
			::InflateRect(this, -x, -y);
		}
	
		*/
		RECT rcText = rcDraw;
		::InflateRect( &rcText, -TEXT_X_MARGIN, -0 );

		CSetBkMode bk( dcmem, TRANSPARENT );
		CSelectObject sofont( dcmem, *( GetFont( ) ) );
	
		//const auto align = IsColumnRightAligned( subitem ) ? DT_RIGHT : DT_LEFT;
		const auto align = is_right_aligned_cache[ static_cast<size_t>( subitem ) ] ? DT_RIGHT : DT_LEFT;

		// Get the correct color in case of compressed or encrypted items
		auto textColor = item->item_text_color( );

		if ( ( pdis->itemState bitand ODS_SELECTED ) && ( showSelectionAlways || HasFocus( ) ) && ( bIsFullRowSelection ) ) {
			textColor = GetItemSelectionTextColor( static_cast<INT>( pdis->itemID ) );
			}

		CSetTextColor tc( dcmem, textColor );

		if ( subitem == column::COL_NAME ) {
			//fastpath. No work to be done!
			dcmem.DrawTextW( item->m_name.get( ), static_cast< int >( item->m_name_length ), &rcText, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP | static_cast< UINT >( align ) );
			return;
			}

		rsize_t size_needed = 0;
		const HRESULT stackbuffer_draw_res = drawSubItem_stackbuffer( item, rcText, align, subitem, dcmem, size_needed );
		if ( !SUCCEEDED( stackbuffer_draw_res ) ) {
			DrawText_dynamic( item, rcText, align, subitem, dcmem, size_needed );
			}

		}

	_Success_( SUCCEEDED( return ) ) 
	HRESULT drawSubItem_stackbuffer( _In_ const COwnerDrawnListItem* const item, _In_ RECT& rcText, const int& align, _In_ _In_range_( 0, INT_MAX ) const column::ENUM_COL subitem, _In_ CDC& dcmem, _On_failure_( _Post_valid_ ) rsize_t& sizeNeeded ) const {
		const rsize_t subitem_text_size = 128;
		wchar_t psz_subitem_formatted_text[ subitem_text_size ] = { 0 };
		//rsize_t sizeNeeded = 0;
		rsize_t chars_written = 0;

		const HRESULT res = item->GetText_WriteToStackBuffer( subitem, psz_subitem_formatted_text, subitem_text_size, sizeNeeded, chars_written );
		if ( SUCCEEDED( res ) ) {
			dcmem.DrawTextW( psz_subitem_formatted_text, static_cast<int>( chars_written ), &rcText, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP | static_cast<UINT>( align ) );
			return res;
			}
		if ( ( MAX_PATH * 2 ) > sizeNeeded ) {
			const rsize_t subitem_text_size_2 = ( MAX_PATH * 2 );
			wchar_t psz_subitem_formatted_text_2[ subitem_text_size_2 ] = { 0 };
			rsize_t chars_written_2 = 0;
			const HRESULT res_2 = item->GetText_WriteToStackBuffer( subitem, psz_subitem_formatted_text_2, subitem_text_size_2, sizeNeeded, chars_written_2 );
			if ( SUCCEEDED( res_2 ) ) {
				dcmem.DrawTextW( psz_subitem_formatted_text_2, static_cast<int>( chars_written_2 ), &rcText, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP | static_cast<UINT>( align ) );
				return res;
				}
			//goto DoDrawSubItemBecauseItCannotDrawItself_drawText_dynamic_memory;
			//DrawText_dynamic( item, rcText, align, subitem, dcmem );
			return res;
			}
		ASSERT( !SUCCEEDED( res ) );
		return res;

		}

	_Pre_satisfies_( subitem != column::COL_NAME )
	void DrawText_dynamic( _In_ const COwnerDrawnListItem* const item, _In_ RECT& rcText, const int& align, _In_ _In_range_( 0, INT_MAX ) const column::ENUM_COL subitem, _In_ CDC& dcmem, _In_ const rsize_t size_needed ) const {
		ASSERT( size_needed < 33000 );
		std::unique_ptr<wchar_t[ ]> buffer ( new wchar_t[ size_needed + 2 ] );
		SecureZeroMemory( buffer.get( ), ( ( size_needed + 2 ) * sizeof( wchar_t ) ) );

		rsize_t new_size_needed = 0;
		rsize_t chars_written = 0;
		const HRESULT res = item->GetText_WriteToStackBuffer( subitem, buffer.get( ), size_needed, new_size_needed, chars_written );
		if ( !SUCCEEDED( res ) ) {
			abort( );
			}
		dcmem.DrawTextW( buffer.get( ), static_cast<int>( chars_written ), &rcText, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP | static_cast< UINT >( align ) );
		}

	void InitializeColors( ) {
		// I try to find a good contrast to COLOR_WINDOW (usually white or light grey).
		// This is a result of experiments. 

		const DOUBLE diff      = 0.07;		// Try to alter the brightness by diff.
		const DOUBLE threshold = 1.04;	// If result would be brighter, make color darker.
		m_windowColor          = GetSysColor( COLOR_WINDOW );

		auto b = CColorSpace::GetColorBrightness( m_windowColor );

		if ( b + diff > threshold ) {
			b -= diff;
			}
		else {
			b += diff;
			if ( b > 1.0 ) {
				b = 1.0;
				}
			}

	#ifdef COLOR_DEBUGGING
		TRACE( _T( "Setting m_stripeColor to CColorSpace::MakeBrightColor( m_windowColor: %ld, b: %f )\r\n" ), m_windowColor, b );
	#endif
		m_stripeColor = CColorSpace::MakeBrightColor( m_windowColor, b );
	#ifdef COLOR_DEBUGGING
		TRACE( _T( "m_stripeColor: %ld\r\n" ), m_stripeColor );
	#endif

		}

	bool IsColumnRightAligned( _In_ const INT col ) const {
		auto hditem = zeroInitHDITEM( );
		hditem.mask   = HDI_FORMAT;
		VERIFY( GetHeaderCtrl( )->GetItem( col, &hditem ) );
		return ( hditem.fmt bitand HDF_RIGHT ) != 0;
		}
	
	_Success_( return >= 0 ) _Ret_range_( 0, INT_MAX ) _On_failure_( _Ret_range_( -1, -1 ) )
	INT GetSubItemWidth( _In_ const COwnerDrawnListItem* const item, _In_ _In_range_( 0, INT_MAX ) const column::ENUM_COL subitem ) const {
		if ( item == NULL ) {
			return -1;
			}
		INT width = 0;

		CClientDC dc( const_cast< COwnerDrawnListCtrl* >( this ) );
		CRect rc( 0, 0, 1000, 1000 );
	
		INT dummy = rc.left;
		if ( item->DrawSubitem_( subitem, dc, rc, 0, &width, &dummy ) ) {
			return width;
			}

		if ( subitem == column::COL_NAME ) {
			//column::COL_NAME requires very little work!
			if ( item->m_name_length == 0 ) {
				return 0;
				}
			CSelectObject sofont( dc, *( GetFont( ) ) );
			const auto align = IsColumnRightAligned( subitem ) ? DT_RIGHT : DT_LEFT;
			dc.DrawTextW( item->m_name.get( ), static_cast<int>( item->m_name_length ), rc, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_NOCLIP | static_cast<UINT>( align ) );
			rc.InflateRect( TEXT_X_MARGIN, 0 );
			return rc.Width( );
			}


		const rsize_t subitem_text_size = 128;
		wchar_t psz_subitem_formatted_text[ subitem_text_size ] = { 0 };
		rsize_t sizeNeeded = 0;
		rsize_t chars_written = 0;

		const HRESULT res_1 = item->GetText_WriteToStackBuffer( subitem, psz_subitem_formatted_text, subitem_text_size, sizeNeeded, chars_written );
		if ( !SUCCEEDED( res_1 ) ) {
			ASSERT( sizeNeeded < 33000 );
			std::unique_ptr<wchar_t[ ]> buffer ( new wchar_t[ sizeNeeded + 2 ] );
			SecureZeroMemory( buffer.get( ), ( ( sizeNeeded + 2 ) * sizeof( wchar_t ) ) );

			rsize_t new_size_needed = 0;
			rsize_t chars_written_2 = 0;
			const HRESULT res_2 = item->GetText_WriteToStackBuffer( subitem, buffer.get( ), sizeNeeded, new_size_needed, chars_written_2 );
			if ( !SUCCEEDED( res_2 ) ) {
				abort( );
				}
			if ( chars_written_2 == 0 ) {
				return 0;
				}
			CSelectObject sofont( dc, *( GetFont( ) ) );
			const auto align = IsColumnRightAligned( subitem ) ? DT_RIGHT : DT_LEFT;
			dc.DrawTextW( buffer.get( ), static_cast<int>( chars_written_2 ), rc, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_NOCLIP | static_cast<UINT>( align ) );

			rc.InflateRect( TEXT_X_MARGIN, 0 );
			return rc.Width( );
			}

		if ( chars_written == 0 ) {
			return 0;
			}

		CSelectObject sofont( dc, *( GetFont( ) ) );
		const auto align = IsColumnRightAligned( subitem ) ? DT_RIGHT : DT_LEFT;
		dc.DrawTextW( psz_subitem_formatted_text, static_cast<int>( chars_written ), rc, DT_SINGLELINE | DT_VCENTER | DT_CALCRECT | DT_NOPREFIX | DT_NOCLIP | static_cast<UINT>( align ) );

		rc.InflateRect( TEXT_X_MARGIN, 0 );
		return rc.Width( );

		}

public:
	                      CMainFrame* const m_frameptr;
	                      bool        m_showGrid             : 1; // Whether to draw a grid
	                      bool        m_showStripes          : 1; // Whether to show stripes
	                      bool        m_showFullRowSelection : 1; // Whether to draw full row selection

	_Field_range_( 0, UINT_MAX )
	                      UINT        m_rowHeight;                // Height of an item
	                      LONG        m_yFirstItem;               // Top of a first list item
	                      COLORREF    m_windowColor;              // The default background color if !m_showStripes
	                      COLORREF    m_stripeColor;              // The stripe color, used for every other item if m_showStripes
	            _Field_z_ PCWSTR      m_persistent_name;          // for persistence
						  SSorting    m_sorting;
	_Field_range_( 0, 8 ) std::int8_t m_indicatedColumn;
						  std::vector<bool> is_right_aligned_cache;


private:
	void buildArrayFromItemsInHeaderControl( _In_ _Pre_readable_size_( capacity ) const int* const columnOrder, _Out_ _Pre_writable_size_( capacity ) _Post_readable_size_( readable ) int* vertical, _In_ const rsize_t capacity, _Out_ rsize_t& readable, _In_ const CHeaderCtrl* header_ctrl ) const {
		ASSERT( static_cast<int>( capacity ) >= header_ctrl->GetItemCount( ) );
		readable = 0;


		auto x   = -GetScrollPos( SB_HORZ );
		auto hdi = zeroInitHDITEM( );

		hdi.mask = HDI_WIDTH;
		const auto header_ctrl_item_count = header_ctrl->GetItemCount( );
		if ( header_ctrl_item_count <= 0 ) {
			std::terminate( );
			}
		if ( static_cast<rsize_t>( header_ctrl_item_count ) > capacity ) {
			std::terminate( );
			}
	
		//Probably NOT vectorizable anyway.
		//Not vectorized: 1304, loop includes assignments of different sizes
		for ( INT i = 0; i < header_ctrl_item_count; i++ ) {
			VERIFY( header_ctrl->GetItem( columnOrder[ i ], &hdi ) );
			x += hdi.cxy;
			vertical[ i ] = x;
			++readable;
			}
		}

	// Overridables
	virtual bool GetAscendingDefault( _In_ const column::ENUM_COL column ) const = 0;

protected:
	//manually expanded DECLARE_MESSAGE_MAP
	static const AFX_MSGMAP* PASCAL GetThisMessageMap( ) {
		typedef COwnerDrawnListCtrl ThisClass;
		//typedef CListCtrl TheBaseClass;
		static const AFX_MSGMAP_ENTRY _messageEntries[] =
			{
				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_DIVIDERDBLCLICKW ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnDividerdblclick))
				},

				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_DIVIDERDBLCLICKA ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnDividerdblclick))
				},

				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMCLICKW ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemclick))
				},

				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMCLICKA ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemclick))
				},


				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMDBLCLICKW ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemdblclick))
				},

				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMDBLCLICKA ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast< void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemdblclick))
				},


				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMCHANGINGW ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemchanging))
				},

				{
					WM_NOTIFY,
					static_cast<WORD>( static_cast<int>( HDN_ITEMCHANGINGA ) ),
					static_cast<WORD>( 0 ),
					static_cast<WORD>( 0 ),
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnHdnItemchanging))
				},

				{
					WM_NOTIFY+WM_REFLECT_BASE,
					static_cast<WORD>( static_cast<int>( LVN_GETDISPINFO ) ),
					0,
					0,
					AfxSigNotify_v,
					reinterpret_cast<AFX_PMSG>(static_cast<void (AFX_MSG_CALL CCmdTarget::*)(NMHDR*, LRESULT*) >(&ThisClass::OnLvnGetdispinfo))
				},
				{
					WM_ERASEBKGND,
					0,
					0,
					0,
					AfxSig_bD,
					static_cast<AFX_PMSG>( reinterpret_cast<AFX_PMSGW>( static_cast<BOOL (AFX_MSG_CALL CWnd::*)(CDC*) >(&ThisClass::OnEraseBkgnd) ) )
				},
				{
					WM_VSCROLL,
					0,
					0,
					0,
					AfxSig_vwwW,
					static_cast<AFX_PMSG>( reinterpret_cast<AFX_PMSGW>( static_cast<void (AFX_MSG_CALL CWnd::*)(UINT, UINT, CScrollBar*) >(&ThisClass::OnVScroll) ) )
				},
				{
					WM_SHOWWINDOW,
					0,
					0,
					0,
					AfxSig_vbw,
					static_cast<AFX_PMSG>( reinterpret_cast<AFX_PMSGW>( static_cast<void (AFX_MSG_CALL CWnd::*)(BOOL, UINT) >(&ThisClass::OnShowWindow) ) )
				},
				{
					WM_DESTROY,
					0,
					0,
					0,
					AfxSig_vv,
					static_cast<AFX_PMSG>( reinterpret_cast<AFX_PMSGW>( static_cast<void (AFX_MSG_CALL CWnd::*)(void) >(&ThisClass::OnDestroy) ) )
				},
				{
					0,
					0,
					0,
					0,
					AfxSig_end,
					(AFX_PMSG)( 0 )
				}
			};
		static const AFX_MSGMAP messageMap = { &CListCtrl::GetThisMessageMap, &_messageEntries[0] };
		return &messageMap;
		}

	virtual const AFX_MSGMAP* GetMessageMap( ) const override {
		return GetThisMessageMap( );
		}

	//DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd( CDC* pDC ) {
		ASSERT_VALID( pDC );
		ASSERT( GetHeaderCtrl( )->GetItemCount( ) > 0 );
		TRACE( _T( "COwnerDrawnListCtrl::OnEraseBkgnd!\r\n" ) );
		handle_EraseBkgnd( pDC );
		return true;
		}
	afx_msg void OnHdnDividerdblclick( NMHDR *pNMHDR, LRESULT *pResult ) {
		WTL::CWaitCursor wc;
		ASSERT( pNMHDR != NULL );
		if ( pNMHDR != NULL ) {
			auto phdr = reinterpret_cast< LPNMHEADER >( pNMHDR );
			const INT subitem = phdr->iItem;
			AdjustColumnWidth( static_cast<column::ENUM_COL>( subitem ) );
			}
		ASSERT( pResult != NULL );
		if ( pResult != NULL ) {
			*pResult = 0;
			}
		}
	afx_msg void OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar ) {
		CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
		// Owner drawn list controls with LVS_EX_GRIDLINES don't repaint correctly when scrolled (under Windows XP). So we force a complete repaint here.
		InvalidateRect( NULL );
		}
	afx_msg void OnHdnItemclick( NMHDR* pNMHDR, LRESULT* pResult ) {
		const auto phdr = reinterpret_cast<LPNMHEADERW>(pNMHDR);
		*pResult = 0;
		const auto col = static_cast<column::ENUM_COL>( phdr->iItem );
		if ( col == m_sorting.column1 ) {
			m_sorting.ascending1 =  ! m_sorting.ascending1;
			}
		else {
			//SetSorting( col, true ); //GetAscendingDefault( col ) == true, unconditionally
			SetSorting( col, AscendingDefault( col ) ); //GetAscendingDefault( col ) == true, unconditionally
			}
		SortItems( );
		}
	afx_msg void OnHdnItemdblclick( NMHDR* pNMHDR, LRESULT* pResult ) {
		OnHdnItemclick( pNMHDR, pResult );
		}
	afx_msg void OnHdnItemchanging( NMHDR *pNMHDR, LRESULT *pResult ) {
		UNREFERENCED_PARAMETER( pNMHDR );
		Default( );
		InvalidateRect( NULL );
		ASSERT( pResult != NULL );
		if ( pResult != NULL ) {
			*pResult = 0;
			}
		}
	afx_msg void OnDestroy( ) {
		SavePersistentAttributes( );
		CListCtrl::OnDestroy( );
		}
	afx_msg void OnLvnGetdispinfo( NMHDR* pNMHDR, LRESULT* pResult ) {
		static_assert( sizeof( NMHDR* ) == sizeof( NMLVDISPINFOW* ), "some size issues. Good luck with that cast!" );
		ASSERT( ( pNMHDR != NULL ) && ( pResult != NULL ) );
		handle_LvnGetdispinfo( pNMHDR, pResult );
		}


private:
	void handle_EraseBkgnd( _In_ CDC* pDC ) {
		// We should recalculate m_yFirstItem here (could have changed e.g. when the XP-Theme changed).
		if ( GetItemCount( ) > 0 ) {
			RECT rc;
			VERIFY( GetItemRect( GetTopIndex( ), &rc, LVIR_BOUNDS ) );
			m_yFirstItem = rc.top;
			}
		// else: if we did the same thing as in OnColumnsCreated(), we get repaint problems.

		const COLORREF gridColor = RGB( 212, 208, 200 );

		RECT rcClient;
		GetClientRect( &rcClient );

		RECT rcHeader;
		const auto header_ctrl = GetHeaderCtrl( );
		header_ctrl->GetWindowRect( &rcHeader );
		ScreenToClient( &rcHeader );

		RECT rcBetween   = rcClient;// between header and first item
		rcBetween.top    = rcHeader.bottom;
		rcBetween.bottom = m_yFirstItem;
		pDC->FillSolidRect( &rcBetween, gridColor );
		const rsize_t column_buf_size = 10;
	
		const auto header_ctrl_item_count = header_ctrl->GetItemCount( );

		ASSERT( header_ctrl_item_count < column_buf_size );
		if ( header_ctrl_item_count > column_buf_size ) {
			//too many columns!
			std::terminate( );
			}
		int column_order[ column_buf_size ] = { 0 };

		VERIFY( GetColumnOrderArray( column_order, header_ctrl_item_count ) );

		int vertical_buf[ column_buf_size ] = { 0 };
		rsize_t vertical_readable = 0;
		buildArrayFromItemsInHeaderControl( column_order, vertical_buf, column_buf_size, vertical_readable, header_ctrl );
		ASSERT( vertical_readable < column_buf_size );

		if ( m_showGrid ) {
			CPen pen( PS_SOLID, 1, gridColor );
			const CSelectObject sopen( *pDC, pen );

			const auto rowHeight = m_rowHeight;
			for ( auto y = ( m_yFirstItem + static_cast<LONG>( rowHeight ) - 1 ); y < rcClient.bottom; y += static_cast<LONG>( rowHeight ) ) {
				ASSERT( rowHeight == m_rowHeight );
				pDC->MoveTo( rcClient.left, static_cast<INT>( y ) );
				VERIFY( pDC->LineTo( rcClient.right, static_cast<INT>( y ) ) );
				}
			const auto verticalSize = vertical_readable;
			for ( size_t i = 0; i < verticalSize; i++ ) {
				pDC->MoveTo( ( vertical_buf[ i ] - 1 ), rcClient.top );
				VERIFY( pDC->LineTo( ( vertical_buf[ i ] - 1 ), rcClient.bottom ) );
				}
			}

		const auto bgcolor    = GetSysColor( COLOR_WINDOW );
		const int  gridWidth  = ( m_showGrid ? 1 : 0 );
		const auto lineCount  = GetCountPerPage( ) + 1;
		const auto firstItem  = GetTopIndex( );
		//((( a ) < ( b )) ? ( a ) : ( b ))
		//((( firstItem + lineCount ) < ( CListCtrl::GetItemCount( ) )) ? ( firstItem + lineCount ) : ( CListCtrl::GetItemCount( ) ))
		//const auto lastItem   = ( min( firstItem + lineCount, CListCtrl::GetItemCount( ) ) - 1 );
		const auto lastItem = ( ( ( firstItem + lineCount ) < ( GetItemCount( ) ) ) ? ( firstItem + lineCount ) : ( GetItemCount( ) ) );

		ASSERT( GetItemCount( ) == 0 || firstItem < GetItemCount( ) );
		ASSERT( GetItemCount( ) == 0 || lastItem <= GetItemCount( ) );
		ASSERT( GetItemCount( ) == 0 || lastItem >= firstItem );

		const auto itemCount = ( lastItem - firstItem + 1 );

		RECT fill;
		fill.left   = vertical_buf[ vertical_readable - 1 ];
		fill.right  = rcClient.right;
		fill.top    = m_yFirstItem;
		fill.bottom = fill.top + static_cast<LONG>( m_rowHeight ) - static_cast<LONG>( gridWidth );
		for ( INT i = 0; i < itemCount; i++ ) {
			pDC->FillSolidRect( &fill, bgcolor );
			VERIFY( OffsetRect( &fill, 0, static_cast<int>( m_rowHeight ) ) );
			}

		const auto rowHeight = m_rowHeight;
		auto top = fill.top;
		while ( top < rcClient.bottom ) {
			fill.top    = top;
			fill.bottom = top + static_cast<LONG>( m_rowHeight ) - static_cast<LONG>( gridWidth );
		
			INT left = 0;
			const auto verticalSize = vertical_readable;
			for ( size_t i = 0; i < verticalSize; i++ ) {
				fill.left = left;
				fill.right = vertical_buf[ i ] - gridWidth;
				pDC->FillSolidRect( &fill, bgcolor );
				left = vertical_buf[ i ];
				}
			fill.left  = left;
			fill.right = rcClient.right;
			pDC->FillSolidRect( &fill, bgcolor );

			ASSERT( rowHeight == m_rowHeight );
			top += rowHeight;
			}

		}

	void handle_LvnGetdispinfo( _In_ NMHDR* pNMHDR, _In_ LRESULT* pResult ) {
		auto di = reinterpret_cast< NMLVDISPINFOW* >( pNMHDR );
		*pResult = 0;
		auto item = reinterpret_cast<COwnerDrawnListItem*>( di->item.lParam );
		ASSERT( item != NULL );
		if ( item != NULL ) {
			if ( ( di->item.mask bitand LVIF_TEXT ) != 0 ) {
				if ( static_cast< column::ENUM_COL >( di->item.iSubItem ) == column::COL_NAME ) {
					//easy fastpath!
					if ( item->m_name.get( ) == nullptr ) {
						return;
						}
					size_t chars_remaining = 0;
					const HRESULT res = StringCchCopyExW( di->item.pszText, static_cast< rsize_t >( di->item.cchTextMax ), item->m_name.get( ), NULL, &chars_remaining, 0 );
					ASSERT( SUCCEEDED( res ) );
					if ( !SUCCEEDED( res ) ) {
						displayWindowsMsgBoxWithMessage( global_strings::COwnerDrawnListCtrl_handle_LvnGetdispinfo_err );
						std::terminate( );
						}
					return;
					}

				rsize_t chars_needed = 0;
				rsize_t chars_written = 0;
				const HRESULT text_res = item->GetText_WriteToStackBuffer( static_cast< column::ENUM_COL >( di->item.iSubItem ), di->item.pszText, static_cast< rsize_t >( di->item.cchTextMax ), chars_needed, chars_written );
				if ( !( SUCCEEDED( text_res ) ) ) {
					if ( text_res == STRSAFE_E_INVALID_PARAMETER ) {
						displayWindowsMsgBoxWithMessage( std::move( std::wstring( L"STRSAFE_E_INVALID_PARAMETER" ) ) );
						}
					if ( text_res == STRSAFE_E_INSUFFICIENT_BUFFER ) {
						displayWindowsMsgBoxWithMessage( std::move( std::wstring( L"STRSAFE_E_INSUFFICIENT_BUFFER" ) ) );
						}
					ASSERT( false );
					std::terminate( );
					}
				}
			}

		}
	};

//need to explicitly ask for inlining else compiler bitches about ODR
inline void COwnerDrawnListItem::DrawHighlightSelectBackground( _In_ const RECT& rcLabel, _In_ const RECT& rc, _In_ const COwnerDrawnListCtrl* const list, _In_ CDC& pdc, _Inout_ COLORREF& textColor ) const {
	// Color for the text in a highlighted item (usually white)
	textColor = list->GetHighlightTextColor( );

	RECT selection = rcLabel;
	// Depending on "FullRowSelection" style
	if ( list->m_showFullRowSelection ) {
		selection.right = rc.right;
		}
	// Fill the selection rectangle background (usually dark blue)
	pdc.FillSolidRect( &selection, list->GetHighlightColor( ) );
	
	}

//need to explicitly ask for inlining else compiler bitches about ODR
inline void COwnerDrawnListItem::DrawLabel( _In_ COwnerDrawnListCtrl* const list, _In_ CDC& pdc, _In_ CRect& rc, _In_ const UINT state, _Out_opt_ INT* const width, _Inout_ INT* const focusLeft, _In_ const bool indent ) const {
	/*
	  Draws an item label (icon, text) in all parts of the WinDirStat view. The rest is drawn by DrawItem()
	  */

	const auto tRc = rc;
	auto rcRest = rc;
	// Increase indentation according to tree-level
	if ( indent ) {
		rcRest.left += GENERAL_INDENT;
		}

	CSelectObject sofont( pdc, *( list->GetFont( ) ) );

	rcRest.DeflateRect( TEXT_X_MARGIN, 0 );

	auto rcLabel = rcRest;
	pdc.DrawTextW( m_name.get( ), static_cast<int>( m_name_length ), rcLabel, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_CALCRECT | DT_NOPREFIX | DT_NOCLIP );//DT_CALCRECT modifies rcLabel!!!

	AdjustLabelForMargin( rcRest, rcLabel );

	CSetBkMode bk( pdc, TRANSPARENT );
	auto textColor = GetSysColor( COLOR_WINDOWTEXT );

	if ( width == NULL && ( state bitand ODS_SELECTED ) != 0 && ( list->HasFocus( ) || list->IsShowSelectionAlways( ) ) ) {
		DrawHighlightSelectBackground( rcLabel, rc, list, pdc, textColor );
		}
	else {
		textColor = item_text_color( ); // Use the color designated for this item. This is currently only for encrypted and compressed items
		}

	// Set text color for device context
	CSetTextColor stc( pdc, textColor );

	if ( width == NULL ) {
		pdc.DrawTextW( m_name.get( ), static_cast<int>( m_name_length ), rcRest, DT_SINGLELINE | DT_VCENTER | DT_WORD_ELLIPSIS | DT_NOPREFIX | DT_NOCLIP );
		}

	rcLabel.InflateRect( 1, 1 );

	*focusLeft = rcLabel.left;

	if ( ( ( state bitand ODS_FOCUS ) != 0 ) && list->HasFocus( ) && ( width == NULL ) && ( !list->m_showFullRowSelection ) ) {
		pdc.DrawFocusRect( rcLabel );
		}


	rcLabel.left = rc.left;
	rc = rcLabel;
	if ( width != NULL ) {
		*width = ( rcLabel.Width( ) ) + 5; // +5 because GENERAL_INDENT?
		}
	}

//need to explicitly ask for inlining else compiler bitches about ODR
inline void COwnerDrawnListItem::DrawSelection( _In_ const COwnerDrawnListCtrl* const list, _In_ CDC& pdc, _In_ RECT rc, _In_ const UINT state ) const {
	ASSERT( list != NULL );
	if ( !list->m_showFullRowSelection ) {
		return;
		}
	if ( ( !list->HasFocus( ) ) && ( !list->IsShowSelectionAlways( ) ) ) {
		return;
		}
	if ( ( state bitand ODS_SELECTED ) == 0 ) {
		return;
		}

	/*
	inline void CRect::DeflateRect(
		_In_ int x,
		_In_ int y) throw()
	{
		::InflateRect(this, -x, -y);
	}
	*/

	::InflateRect( &rc, -0, -static_cast<int>( LABEL_Y_MARGIN ) );
	pdc.FillSolidRect( &rc, list->GetHighlightColor( ) );
	}

	AFX_COMDAT const CRuntimeClass COwnerDrawnListCtrl::classCOwnerDrawnListCtrl =
	{
		"COwnerDrawnListCtrl",
		sizeof(class COwnerDrawnListCtrl),
		0xFFFF,
		NULL,
		((CRuntimeClass*)(&CListCtrl::classCListCtrl)),
		NULL,
		NULL
	};


#else
#error ass
#endif



// $Log$
// Revision 1.9  2004/11/12 22:14:16  bseifert
// Eliminated CLR_NONE. Minor corrections.
//
// Revision 1.8  2004/11/12 00:47:42  assarbad
// - Fixed the code for coloring of compressed/encrypted items. Now the coloring spans the full row!
//
// Revision 1.7  2004/11/08 00:46:26  assarbad
// - Added feature to distinguish compressed and encrypted files/folders by color as in the Windows 2000/XP explorer.
//   Same rules apply. (Green = encrypted / Blue = compressed)
//
// Revision 1.6  2004/11/07 23:28:14  assarbad
// - Partial implementation for coloring of compressed/encrypted files
//
// Revision 1.5  2004/11/05 16:53:07  assarbad
// Added Date and History tag where appropriate.
//
