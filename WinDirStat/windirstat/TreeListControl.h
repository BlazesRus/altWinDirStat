// TreeListControl.h - Declaration of CTreeListItem and CTreeListControl
//
// see `file_header_text.txt` for licensing & contact info.

#pragma once

#include "stdafx.h"

#ifndef WDS_TREELISTCONTROL_H
#define WDS_TREELISTCONTROL_H




#include "ownerdrawnlistcontrol.h"
#include "ChildrenHeapManager.h"
//#include "pacman.h"



class CItemBranch;


class CTreeListItem;
class CTreeListControl;
class CSortingListItem;
class CImageList;
class CDirstatDoc;

struct VISIBLEINFO {
	VISIBLEINFO( ) : indent( 0 ), isExpanded { false }, ntfs_compression_ratio { 0.0 } { }

	SRECT  rcPlusMinus;     // Coordinates of the little +/- rectangle, relative to the upper left corner of the item.
	SRECT  rcTitle;         // Coordinates of the label, relative to the upper left corner of the item.
	// cache_sortedChildren: This member contains our children (the same set of children as in CItem::m_children) and is initialized as soon as we are expanded.
	// In contrast to CItem::m_children, this array is always sorted depending on the current user-defined sort column and -order.
	std::vector<CTreeListItem *>           cache_sortedChildren;
	_Field_range_( 0, 32767 ) std::int16_t indent;  // 0 for the root item, 1 for its children, and so on.
		                      bool         isExpanded : 1; // Whether item is expanded.
							  double       ntfs_compression_ratio;

	};

//
// CTreeListItem. An item in the CTreeListControl. (CItem is derived from CTreeListItem.)
// In order to save memory, once the item is actually inserted in the List, we allocate the VISIBLEINFO structure (m_vi).
// m_vi is freed as soon as the item is removed from the List.
class CTreeListItem : public COwnerDrawnListItem {
	

		virtual bool   DrawSubitem( RANGE_ENUM_COL const column::ENUM_COL subitem, _In_ CDC& pdc, _In_ RECT rc, _In_ const UINT state, _Out_opt_ INT* const width, _Inout_ INT* const focusLeft, _In_ const COwnerDrawnListCtrl* const list ) const override final;
		virtual INT Compare( _In_ const COwnerDrawnListItem* const other, RANGE_ENUM_COL const column::ENUM_COL subitem                          ) const override final;

		inline INT     concrete_compare( _In_ const CTreeListItem* const other, RANGE_ENUM_COL const column::ENUM_COL subitem ) const;
		const bool     set_plusminus_and_title_rects( _In_ const RECT rcLabel, _In_ const RECT rc_const ) const;

		const COLORREF Concrete_ItemTextColor( ) const;

		//ItemTextColor __should__ be private!
		virtual COLORREF ItemTextColor( ) const override final {
			return Concrete_ItemTextColor( );
			}

		
	public:
		//Unconditionally called only ONCE, so we ask for inlining.
		//Encodes the attributes to fit (in) 1 byte
		__forceinline void SetAttributes( _In_ const DWORD attr ) {
			if ( attr == INVALID_FILE_ATTRIBUTES ) {
				m_attr.invalid = true;
				return;
				}
			m_attr.readonly   = ( ( attr bitand FILE_ATTRIBUTE_READONLY      ) != 0 );
			m_attr.hidden     = ( ( attr bitand FILE_ATTRIBUTE_HIDDEN        ) != 0 );
			m_attr.system     = ( ( attr bitand FILE_ATTRIBUTE_SYSTEM        ) != 0 );
			m_attr.compressed = ( ( attr bitand FILE_ATTRIBUTE_COMPRESSED    ) != 0 );
			m_attr.encrypted  = ( ( attr bitand FILE_ATTRIBUTE_ENCRYPTED     ) != 0 );
			m_attr.reparse    = ( ( attr bitand FILE_ATTRIBUTE_REPARSE_POINT ) != 0 );
			m_attr.invalid    = false;
			}

		//default constructor DOES NOT initialize jack shit.
		__forceinline CTreeListItem( ) { }

		CTreeListItem( _In_z_ _Readable_elements_( length ) PCWSTR const&& name, const std::uint16_t&& length, _In_ CTreeListItem* const parent, const DWORD attr, const bool done ) : COwnerDrawnListItem( name, length ), m_parent( parent ), m_rect{ 0, 0, 0, 0 }, m_childCount{ 0u } {
			SetAttributes( attr );
			m_attr.m_done = done;
			}

		CTreeListItem( CTreeListItem& in ) = delete;
		CTreeListItem& operator=( const CTreeListItem& in ) = delete;

		virtual ~CTreeListItem( ) = default;

		//unconditionally called only ONCE, so we ask for inlining.
		


		//these functions downcast `this` to a CItemBranch* to enable static polymorphism
		std::uint64_t size_recurse_( ) const;
		_Ret_range_( 0, UINT32_MAX ) std::uint32_t GetChildrenCount_( ) const;

		_Ret_maybenull_
		CItemBranch* children_ptr( ) const;
		
		_Success_( return != NULL ) _Must_inspect_result_ _Ret_maybenull_ 
		CTreeListItem* GetSortedChild   ( _In_ const size_t i                             ) const;

		_Success_( return < child_count )
		size_t  FindSortedChild                 ( _In_ const CTreeListItem* const child, _In_ const size_t child_count ) const;

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		std::int16_t  GetIndent( ) const {
			ASSERT( IsVisible( ) );
			return m_vi->indent;
			}

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		void SetExpanded( _In_ const bool expanded = true ) {
			ASSERT( IsVisible( ) );
			m_vi->isExpanded = expanded;
			}

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		void SetPlusMinusRect( _In_ const RECT& rc ) const {
			ASSERT( IsVisible( ) );
			m_vi->rcPlusMinus = SRECT( rc );
			}

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		void SetTitleRect( _In_ const RECT& rc ) const {
			ASSERT( IsVisible( ) );
			m_vi->rcTitle = SRECT( rc );
			}

		//_At_( this->m_vi, _When_( next_state_visible, _Post_valid_ ) ) _At_( this->m_vi, _When_( ( !next_state_visible ), _Post_ptr_invalid_ ) ) 
		//_At_( this->m_vi, _When_( next_state_visible == false, _Post_ptr_invalid_ ) )
		//_At_( this->m_vi, _When_( next_state_visible == false, _Post_invalid_ ) )
		void SetVisible ( _In_ const bool next_state_visible = true ) const;

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		void SortChildren                       ( _In_ const CTreeListControl* const ctrl );

		_Pre_satisfies_( this->m_parent != NULL )
		bool  HasSiblings                       (                                           ) const;
		
		void childNotNull( _In_ CItemBranch* const aTreeListChild, const size_t i );
		
		bool HasChildren ( ) const {
			return ( children_ptr( ) != NULL );
			}
		
		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		bool IsExpanded( ) const {
			ASSERT( IsVisible( ) );
			return m_vi->isExpanded; 
			}
		
		bool IsVisible( ) const {
			return ( m_vi != nullptr );
			}
	
		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		RECT GetPlusMinusRect( ) const;

		_Pre_satisfies_( this->m_vi._Myptr != nullptr )
		RECT GetTitleRect( ) const;

		RECT TmiGetRectangle(                              ) const;
		void TmiSetRectangle( _In_ const RECT& rc          ) const;

	public:
		                         const CTreeListItem*               m_parent;
		                               Children_String_Heap_Manager m_name_pool;
		                       mutable std::unique_ptr<VISIBLEINFO> m_vi = nullptr; // Data needed to display the item.
		                       mutable SRECT                        m_rect;         // Finally, this is our coordinates in the Treemap view. (For GraphView)
		                               attribs                      m_attr;
		//4,294,967,295 ( 4294967295 ) is the maximum number of files in an NTFS filesystem according to http://technet.microsoft.com/en-us/library/cc781134(v=ws.10).aspx
		//We can exploit this fact to use a 4-byte unsigned integer for the size of the array, which saves us 4 bytes on 64-bit architectures!
		_Field_range_( 0, 4294967295 ) std::uint32_t                m_childCount;
	};




//
// CTreeListControl. A CListCtrl, which additionally behaves an looks like a tree control.
//
class CTreeListControl final : public COwnerDrawnListCtrl {
	DECLARE_DYNAMIC( CTreeListControl )

	//virtual bool GetAscendingDefault( _In_ const column::ENUM_COL column ) const override final {
	//	UNREFERENCED_PARAMETER( column );
	//	return true;
	//	}

	public:
		CTreeListControl& operator=( const CTreeListControl& in ) = delete;
		CTreeListControl( const CTreeListControl& in ) = delete;

		_Pre_satisfies_( rowHeight % 2 == 0 )
		CTreeListControl( _In_range_( 0, NODE_HEIGHT ) UINT rowHeight, CDirstatDoc* docptr ) : COwnerDrawnListCtrl( global_strings::treelist_str, rowHeight ), m_pDocument( docptr ) {
			//ASSERT( _theTreeListControl == NULL );
			//_theTreeListControl = this;
			ASSERT( rowHeight <= NODE_HEIGHT );     // gr��er k�nnen wir nicht//"larger, we can not"?
			ASSERT( rowHeight % 2 == 0 );           // muss gerade sein//"must be straight"?
			}
		
		virtual ~CTreeListControl( ) = default;

#pragma warning( suppress: 4263 )
		        BOOL CreateEx( _In_ const DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ const UINT nID );

				void SysColorChanged       ( );
				bool SelectedItemCanToggle ( ) const;
				void Sort                  ( );
				void ToggleSelectedItem    ( );


		_Must_inspect_result_ _Success_( return != NULL ) _Ret_maybenull_
				CTreeListItem* GetItem( _In_ _In_range_( 0, INT_MAX ) const int i ) const;


				void SetRootItem                               ( _In_opt_ const CTreeListItem* const root                     );
		_Pre_satisfies_( !isDone )
				void OnChildAdded                              ( _In_opt_ const CTreeListItem* const parent, _In_ CTreeListItem* const child, _In_ const bool isDone );
			  //INT  GetItemScrollPosition                     ( _In_     const CTreeListItem* const item ) const;
				int  EnumNode                                  ( _In_     const CTreeListItem* const item ) const;	
				
				INT  find_item_then_show_first_try_failed      ( _In_     const CTreeListItem* const thisPath, const int i );

				void find_item_then_show                       ( _In_     const CTreeListItem* const thisPath, const int i, int& parent, _In_ const bool showWholePath, _In_ const CTreeListItem* const target_item_in_path );

				void expand_item_then_scroll_to_it                           ( _In_     const CTreeListItem* const pathZero,  _In_range_( 0, INT_MAX ) const int index, _In_ const bool showWholePath );
				void expand_item_no_scroll_then_doWhateverJDoes                           ( _In_     const CTreeListItem* const pathZero,  _In_range_( 0, INT_MAX ) const int parent );
				void handle_VK_RIGHT                           ( _In_     const CTreeListItem* const item, _In_ _In_range_( 0, INT_MAX ) const int i );
				void adjustColumnSize                          ( _In_     const CTreeListItem* const item_at_index );
				void SelectAndShowItem                         ( _In_     const CTreeListItem* const item, _In_ const bool showWholePath                                                           );
				void SelectItem                                ( _In_     const CTreeListItem* const item );
				void EnsureItemVisible                         ( _In_     const CTreeListItem* const item                                                                                    );
			  //void ExpandItem                                ( _In_     const CTreeListItem* const item                                                                                          );
				void handle_VK_LEFT                            ( _In_     const CTreeListItem* const item, _In_ _In_range_( 0, INT32_MAX ) const int i );
				
				
			  //void SetItemScrollPosition                     ( _In_     const CTreeListItem* const item, _In_ const INT top );
				
				_Pre_satisfies_( item->m_vi._Myptr != nullptr ) _Success_( return )
				const bool DrawNodeNullWidth                   ( _In_     const CTreeListItem* const item, _In_ CDC& pdc, _In_ const RECT& rcRest, _In_ CDC& dcmem, _In_ const UINT ysrc ) const;
				
				RECT DrawNode_Indented                         ( _In_     const CTreeListItem* const item, _In_ CDC& pdc, _Inout_    RECT& rc, _Inout_ RECT& rcRest ) const;

				RECT DrawNode                                  ( _In_     const CTreeListItem* const item, _In_ CDC& pdc, _Inout_    RECT& rc            ) const;

		_Pre_satisfies_( ( parent + 1 ) < index ) _Ret_range_( -1, INT_MAX ) 
				int collapse_parent_plus_one_through_index     ( _In_     const CTreeListItem*       thisPath, const int index, _In_range_( 0, INT_MAX ) const int parent );
				
				void handle_VK_ESCAPE                          ( const UINT nChar, const UINT nRepCnt, const UINT nFlags );
				void handle_VK_TAB                             ( const UINT nChar, const UINT nRepCnt, const UINT nFlags );
				void handle_remaining_keys                     ( const UINT nChar, const UINT nRepCnt, const UINT nFlags );
	private:
				void ExpandItemAndScroll( _In_ _In_range_( 0, INT_MAX ) const int i ) {
					ExpandItem( i, true );
					}

				void ExpandItemNoScroll( _In_ _In_range_( 0, INT_MAX ) const int i ) {
					ExpandItem( i, false );
					}


	protected:
				void ExpandItemInsertChildren                  ( _In_     const CTreeListItem* const item, _In_ _In_range_( 0, INT32_MAX ) const INT_PTR i, _In_ const bool scroll  );
				void InsertItem                                ( _In_     const CTreeListItem* const item, _In_ _In_range_( 0, INT32_MAX ) const INT_PTR i );
				void insertItemsAdjustWidths                   ( _In_     const CTreeListItem* const item, _In_ _In_range_( 1, SIZE_T_MAX ) const size_t count, _Inout_ _Out_range_( 0, INT_MAX ) INT& maxwidth, _In_ const bool scroll, _In_ _In_range_( 0, INT_MAX ) const INT_PTR i );
				int  countItemsToDelete                        ( _In_     const CTreeListItem* const item, bool& selectNode, _In_ _In_range_( 0, INT_MAX ) const int& i );

				void PrepareDefaultMenu                        ( _In_     const CItemBranch*   const item, _Out_ CMenu* const menu ) const;

				void OnItemDoubleClick                         ( _In_ _In_range_( 0, INT_MAX ) const int i );
				void ExpandItem                                ( _In_ _In_range_( 0, INT_MAX ) const int i, _In_ const bool scroll /*= true*/ );
				
				void DeleteItem( _In_ _In_range_( 0, INT_MAX ) const int i ) {
					ASSERT( i < CListCtrl::GetItemCount( ) );
					auto const anItem = GetItem( i );
					if ( anItem != NULL ) {
						anItem->SetExpanded( false );
						anItem->SetVisible( false );
						anItem->m_vi.reset( );
						//auto newVI = anItem->m_vi->rcTitle;
						}
					VERIFY( CListCtrl::DeleteItem( i ) );
					}


				void ToggleExpansion                           ( _In_ _In_range_( 0, INT_MAX ) const INT i                           );
				void SelectItem                                ( _In_ _In_range_( 0, INT_MAX ) const INT i );
				//                                            #define WDS_IN_POS_INT
		_Success_( return == true )
				bool CollapseItem                              ( _In_ _In_range_( 0, INT_MAX ) const int i                           );

		_Must_inspect_result_ _Success_( return != -1 ) _Ret_range_( 0, INT_MAX )
				INT  GetSelectedItem( ) const;
				void InitializeNodeBitmaps                     (             ) const;

				void handle_OnContextMenu( CPoint pt ) const;

	   mutable CBitmap           m_bmNodes0;                   // The bitmaps needed to draw the treecontrol-like branches
	   mutable CBitmap           m_bmNodes1;                   // The same bitmaps with stripe-background color
			   INT               m_lButtonDownItem;            // Set in OnLButtonDown(). -1 if not item hit.
			   //C4820: 'CTreeListControl' : '3' bytes padding added after data member 'CTreeListControl::m_lButtonDownOnPlusMinusRect'
			   bool              m_lButtonDownOnPlusMinusRect; // Set in OnLButtonDown(). True, if plus-minus-rect hit.

	public:
			   CDirstatDoc*      m_pDocument;
	protected:
		DECLARE_MESSAGE_MAP()
		afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
		afx_msg void MeasureItem( _In_ PMEASUREITEMSTRUCT mis ) {
			mis->itemHeight = static_cast<UINT>( m_rowHeight );
			}
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
		afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		afx_msg void OnSetFocus( _In_ CWnd* pOldWnd );
};



#else

#endif
