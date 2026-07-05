//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree.cpp
//
// Identification: src/storage/index/b_plus_tree.cpp
//
// Copyright (c) 2015-2025, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "storage/index/b_plus_tree.h"
#include <cstddef>
#include <optional>
#include "buffer/traced_buffer_pool_manager.h"
#include "common/config.h"
#include "storage/index/b_plus_tree_debug.h"
#include "storage/index/generic_key.h"
#include "storage/page/b_plus_tree_header_page.h"
#include "storage/page/b_plus_tree_page.h"
#include "storage/page/page_guard.h"

namespace bustub {

FULL_INDEX_TEMPLATE_ARGUMENTS
BPLUSTREE_TYPE::BPlusTree(std::string name, page_id_t header_page_id, BufferPoolManager *buffer_pool_manager,
                          const KeyComparator &comparator, int leaf_max_size, int internal_max_size)
    : bpm_(std::make_shared<TracedBufferPoolManager>(buffer_pool_manager)),
      index_name_(std::move(name)),
      comparator_(std::move(comparator)),
      leaf_max_size_(leaf_max_size),
      internal_max_size_(internal_max_size),
      header_page_id_(header_page_id) {
  WritePageGuard guard = bpm_->WritePage(header_page_id_);
  auto root_page = guard.AsMut<BPlusTreeHeaderPage>();
  root_page->root_page_id_ = INVALID_PAGE_ID;
}

/**
 * @brief Helper function to decide whether current b+tree is empty
 * @return Returns true if this B+ tree has no keys and values.
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::IsEmpty() const -> bool { 
  //UNIMPLEMENTED("TODO(P2): Add implementation."); 
  ReadPageGuard guard = bpm_->ReadPage(header_page_id_); // acquire a read lock on the current B+ tree header.
  auto header_page = guard.As<BPlusTreeHeaderPage>(); // convert the raw bytes from the guard to a read only bplustreeheader type object
  auto root_id = header_page->root_page_id_;// get the id of the root page to check wether its valid or not (exists or not)
  if(root_id == INVALID_PAGE_ID )
  {
    return true;
  }
  else
  {
    return false;
  }
}

/*****************************************************************************
 * SEARCH
 *****************************************************************************/
/**
 * @brief Return the only value that associated with input key
 *
 * This method is used for point query
 *
 * @param key input key
 * @param[out] result vector that stores the only value that associated with input key, if the value exists
 * @return : true means key exists
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetValue(const KeyType &key, std::vector<ValueType> *result) -> bool {
  //UNIMPLEMENTED("TODO(P2): Add implementation.");
  // Declaration of context instance. Using the Context is not necessary but advised.
  Context ctx;
  ReadPageGuard header_guard = bpm_->ReadPage(header_page_id_);
  auto header_pg = header_guard.As<BPlusTreeHeaderPage>();
  auto header_root_id = header_pg->root_page_id_;
  header_guard.Drop();
  if(header_root_id == INVALID_PAGE_ID) // check if tree even exists
  {
    return false;
  }
  
  ReadPageGuard curr_guard = bpm_->ReadPage(header_root_id);
  auto curr_page = curr_guard.As<BPlusTreePage>();
  while(!curr_page->IsLeafPage())
  {
    auto internal = curr_guard.As<InternalPage>();
   
      int hi = curr_page->GetSize(),lo = 1;
      while(lo<hi)
      {
        
        int mid = lo + (hi-lo)/2;
        auto comp = comparator_(internal->KeyAt(mid),key);
        if(comp > 0) // if key is smaller than the key at current mid
        {
          hi = mid;
        }
        else  // if key is smaller than or equal to the key at current mid
        {
          lo = mid+1;
          
        }
        
      }
      page_id_t child_id = internal->ValueAt(lo -1);
      curr_guard = bpm_->ReadPage(child_id);
      curr_page = curr_guard.As<BPlusTreePage>();
      
    }
      auto leaf_pg = curr_guard.As<LeafPage>();
      int hi = curr_page->GetSize();
      int lo = 0;
      while(lo<hi)
      {
        int mid = lo + (hi-lo)/2; 
        auto comp = comparator_(leaf_pg->KeyAt(mid),key);
        if(comp > 0) // if key is smaller than the key at current mid
        {
          hi = mid;
        }
        else if (comp<0) // if key is smaller than or equal to the key at current mid
        {
          lo = mid+1;
          
        }
        else
        {
          auto tombstones = leaf_pg->GetTombstones();
          for(auto &tombs : tombstones)
          {
            if(comparator_(tombs,key) == 0)
            {
              return false; // the key has been logically deleted
            }
          }
          result->push_back(leaf_pg->ValueAt(mid));
          return true;
        }
      }
      return false;
    
  
  
}

/*****************************************************************************
 * INSERTION
 *****************************************************************************/
/**
 * @brief Insert constant key & value pair into b+ tree
 *
 * if current tree is empty, start new tree, update root page id and insert
 * entry; otherwise, insert into leaf page.
 *
 * @param key the key to insert
 * @param value the value associated with key
 * @return: since we only support unique key, if user try to insert duplicate
 * keys return false; otherwise, return true.
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Insert(const KeyType &key, const ValueType &value) -> bool {
  //UNIMPLEMENTED("TODO(P2): Add implementation.");
  // Declaration of context instance. Using the Context is not necessary but advised.
  Context ctx;
  WritePageGuard header_guard = bpm_->WritePage(header_page_id_); // get a write guard on header pg
  auto header_pg = header_guard.AsMut<BPlusTreeHeaderPage>(); // get a pointer to the guard as a headertype obj
  auto root_pg_id = header_pg->root_page_id_; // get roots page id from the header
  ctx.header_page_=std::move(header_guard); // push into ctx headerpage to propogate back if needed

  if(root_pg_id == INVALID_PAGE_ID) // tree doesnt exist
  {
    auto leaf_pg_id = bpm_->NewPage(); // since tree doesnt exist create root which would be the leaf
    WritePageGuard leaf_guard = bpm_->WritePage(leaf_pg_id); // take a writeguard on that leaf
    auto leaf_pg = leaf_guard.AsMut<LeafPage>(); // get a mutable pointer to it as a leafpage obj
    leaf_pg->Init(leaf_max_size_); // set max size
    leaf_pg->SetValueAt(0,value); // since its leaf idx 0 is valid so insert the value
    leaf_pg->SetKeyAt(0,key);     // and key at first idx(0)
    leaf_pg->ChangeSizeBy(1); // increment current size
    header_pg->root_page_id_ = leaf_pg_id; // set the node u made as the root inside the headerpg.
    ctx.header_page_ = std::nullopt; // empty the context header since no longer needed
    return true;
  }
  //tree exists
  WritePageGuard curr_guard = bpm_->WritePage(root_pg_id);
  auto curr_pg = curr_guard.AsMut<BPlusTreePage>();
  page_id_t leaf_pg_id=0;
  while(!curr_pg->IsLeafPage()) // internal page
  {
    
    auto internal = curr_guard.AsMut<InternalPage>();
    int lo = 1; 
    int hi = curr_pg->GetSize();
    
    while(lo<hi)
    {
    int mid = lo + (hi - lo)/2;
    int cmp = comparator_(internal->KeyAt(mid),key);
    if(cmp > 0) // if key is smaller than the key at current mid
      {
        hi = mid;
      }
    else  // if key is smaller than or equal to the key at current mid
      {
        lo = mid+1;
        
      }
       
    } 
    
    leaf_pg_id = internal->ValueAt(lo-1);
    ctx.write_set_.push_back(std::move(curr_guard));
    curr_guard = bpm_->WritePage(leaf_pg_id);
    curr_pg = curr_guard.AsMut<BPlusTreePage>();
    
  }
  //leaf page
  auto leaf_pg = curr_guard.AsMut<LeafPage>();
  int hi=leaf_pg->GetSize();
  int lo = 0;
  
  int mid = 0;
  while(lo<hi)
  {
    mid = lo + (hi-lo)/2;
    int cmp = comparator_(leaf_pg->KeyAt(mid),key);
    
      if(cmp>0)
      {
        hi = mid;
      }
      else if (cmp <0)
      {
        lo = mid+1;
      }
      else
      {
        return false; // duplicate key inserted
      }
    
  }
  for(int i=leaf_pg->GetSize();i>lo;i--) // shift entries to the right to make space to insert the new k-v pairs
  {
    leaf_pg->SetKeyAt(i,leaf_pg->KeyAt(i-1));
    leaf_pg->SetValueAt(i,leaf_pg->ValueAt(i-1));
  }

  leaf_pg->SetValueAt(lo,value);
  leaf_pg->SetKeyAt(lo,key);
  leaf_pg->ChangeSizeBy(1);
  if(leaf_pg->GetSize()>=leaf_pg->GetMaxSize()) // leaf has expanded beyond max allowable capacity so we must split the node
  {
    
    page_id_t new_leaf_id = bpm_->NewPage(); // create a new page for which to split right entries of previous page into
    WritePageGuard new_leaf_guard = bpm_->WritePage(new_leaf_id);
    auto new_leaf_pg = new_leaf_guard.AsMut<LeafPage>();
    new_leaf_pg->Init(leaf_max_size_);
    int split = leaf_pg->GetSize()/2;
    int idx = 0;
    
    for(int i=split;i<leaf_pg->GetSize();i++) // set right values of original leaf page to the new leaf page
    {
      
      new_leaf_pg->SetValueAt(idx,leaf_pg->ValueAt(i));
      new_leaf_pg->SetKeyAt(idx,leaf_pg->KeyAt(i));
      idx++;
    }
    new_leaf_pg->SetSize(idx); // no of inserted k-v pairs is the size of the new page
    leaf_pg->SetSize(leaf_pg->GetSize()-idx); // subtract the no of inserted pairs to the new page to get the size of old page
    KeyType up_key = new_leaf_pg->KeyAt(0); // the key on which the nodes were split (the first key of right node in leaves)
    new_leaf_pg->SetNextPageId(leaf_pg->GetNextPageId()); // connect the new node to the one pointed to by prev
    leaf_pg->SetNextPageId(new_leaf_id); // connect the prev node to the new node. struct now is old->new->olds prev next

    InsertIntoParent(ctx, leaf_pg_id, up_key, new_leaf_id);
  }
  ctx.header_page_ = std::nullopt;
  return true;
}

FULL_INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::InsertIntoParent(Context &ctx, page_id_t old_id, const KeyType &key, page_id_t new_id)
{
  if(ctx.write_set_.empty())
  {
    
    
  }
  WritePageGuard parent_guard = std::move(ctx.write_set_.back());
  ctx.write_set_.pop_back();
  
  auto parent = parent_guard.AsMut<InternalPage>();
}


/*****************************************************************************
 * REMOVE
 *****************************************************************************/
/**
 * @brief Delete key & value pair associated with input key
 * If current tree is empty, return immediately.
 * If not, User needs to first find the right leaf page as deletion target, then
 * delete entry from leaf page. Remember to deal with redistribute or merge if
 * necessary.
 *
 * @param key input key
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
void BPLUSTREE_TYPE::Remove(const KeyType &key) {
  // Declaration of context instance.
  Context ctx;
  UNIMPLEMENTED("TODO(P2): Add implementation.");
}

/*****************************************************************************
 * INDEX ITERATOR
 *****************************************************************************/
/**
 * @brief Input parameter is void, find the leftmost leaf page first, then construct
 * index iterator
 *
 * You may want to implement this while implementing Task #3.
 *
 * @return : index iterator
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin() -> INDEXITERATOR_TYPE { UNIMPLEMENTED("TODO(P2): Add implementation."); }

/**
 * @brief Input parameter is low key, find the leaf page that contains the input key
 * first, then construct index iterator
 * @return : index iterator
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::Begin(const KeyType &key) -> INDEXITERATOR_TYPE { UNIMPLEMENTED("TODO(P2): Add implementation."); }

/**
 * @brief Input parameter is void, construct an index iterator representing the end
 * of the key/value pair in the leaf node
 * @return : index iterator
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::End() -> INDEXITERATOR_TYPE { UNIMPLEMENTED("TODO(P2): Add implementation."); }

/**
 * @return Page id of the root of this tree
 *
 * You may want to implement this while implementing Task #3.
 */
FULL_INDEX_TEMPLATE_ARGUMENTS
auto BPLUSTREE_TYPE::GetRootPageId() -> page_id_t { UNIMPLEMENTED("TODO(P2): Add implementation."); }

template class BPlusTree<GenericKey<4>, RID, GenericComparator<4>>;

template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 3>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 2>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>, 1>;
template class BPlusTree<GenericKey<8>, RID, GenericComparator<8>, -1>;

template class BPlusTree<GenericKey<16>, RID, GenericComparator<16>>;

template class BPlusTree<GenericKey<32>, RID, GenericComparator<32>>;

template class BPlusTree<GenericKey<64>, RID, GenericComparator<64>>;

}  // namespace bustub
