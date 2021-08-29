// Copyright 2014, 2017-2019, 2021 by Jon Dart.  All Rights Reserved.

#include "bookwrit.h"
#include <cassert>
#include <fstream>

BookWriter::BookWriter(int pages) :
   index_pages(pages) {
   index = new book::IndexPage *[index_pages];
   for (int i = 0; i < index_pages; i++) {
      index[i] = nullptr; // no index page allocated yet
   }
}

BookWriter::~BookWriter() {
   delete [] index;
}
                
void BookWriter::add(const hash_t hashCode, uint8_t moveIndex, uint8_t weight,
                     uint32_t win, uint32_t loss, uint32_t draw) {
   const int index_page = (int)(hashCode % index_pages);
   // fetch index page, allocate if necessary
   book::IndexPage *idx = index[index_page];
   if (idx == nullptr) {
      idx = index[index_page] = new book::IndexPage();
   }
   // see if index entry for hashcode is present already
   int found = -1;
   for (unsigned i = 0; i < idx->next_free; i++) {
      if (hashCode == idx->index[i].hashCode) {
         found = i;
         break;
      }
   }
   // create and populate new data entry
   book::DataEntry de;
   de.index = moveIndex;
   de.next = book::NO_NEXT;
   de.weight = weight;
   de.win = win;
   de.loss = loss;
   de.draw = draw;
   if (found == -1) {
      // get last data page
      book::DataPage *dp;
      int dataIndex;
      if (!data.size()) {
         dp = nullptr;
      } else {
         dp = data[data.size()-1];
      }
      if (!dp || !dp->num_free) {
         dp = new book::DataPage();
         data.push_back(dp);
      }
      const unsigned dataPage = (unsigned)data.size()-1; // last page after insertion if any
      // add data to page
      dataIndex = dp->free_list;
      int next = dp->data[dataIndex].next;
      assert(de.next != dataIndex);
      dp->data[dataIndex] = de;
      dp->free_list = next;
      dp->num_free--;
      // add index entry
      if (idx->next_free < book::INDEX_PAGE_SIZE) {
          idx->index[idx->next_free++] = book::IndexEntry(hashCode,static_cast<uint16_t>(dataPage),
                                                          static_cast<uint16_t>(dataIndex));
      } else {
         throw BookFullException();
      }
   } else {
      // at least one move exists in book with the current hash
      book::IndexEntry &existingIndex = idx->index[found];
      book::DataPage *dp = data[existingIndex.page];
      // See if the move we are adding is in the book already
      unsigned foundIndex = 0;
      unsigned endOfChain = existingIndex.index;
      unsigned dataIndex = existingIndex.index;
      while (dataIndex != book::NO_NEXT) {
         book::DataEntry &existingDe = dp->data[dataIndex];
         if (de.index == existingDe.index) {
            foundIndex++;
            break;
         }
         endOfChain = dataIndex;
         dataIndex = existingDe.next;
      }
      if (!foundIndex) {
         // Entry is not already in book, must add a new DataEntry
         if (dp->num_free) {
            // add new data entry to this page
            int nextFree = dp->data[dp->free_list].next;
            unsigned insert = (unsigned)dp->free_list;
            // link new entry to the end of the existing move chain
            assert(endOfChain != insert);
            dp->data[endOfChain].next = static_cast<uint16_t>(insert);
            // put new data into data page
            dp->data[insert] = de;
            dp->free_list = nextFree;
            dp->num_free--;
         } else {
            // We cannot allocate the new entry in the current data
            // page. Move existing entries to a new data page, reclaim
            // their space to the existing data page's free list, and
            // update the index page to point to the new data page.
            book::DataPage *newDp = new book::DataPage();
            data.push_back(newDp);
            // save existing head of the data chain in the old page.
            int oldDataIndex = existingIndex.index;
            // update index to point to new page
            existingIndex.page = static_cast<uint16_t>(data.size()-1);
            existingIndex.index = static_cast<uint16_t>(newDp->free_list);
            book::DataPage *oldDp = dp;
            // transfer entries from old to new page
            while (oldDataIndex != book::NO_NEXT) {
               book::DataEntry &oldDe = dp->data[oldDataIndex];
               assert(newDp->free_list < book::DATA_PAGE_SIZE);
               book::DataEntry &newDe = newDp->data[newDp->free_list];
               unsigned nextFree = (unsigned)newDe.next;
               // update new page with old data
               newDe = oldDe;
               if (oldDe.next != book::NO_NEXT) {
                   // We are not at the end of the list so
                   // make the next pointer go to the next
                   // free slot - we will allocate that
                   // in the next pass thru the loop.
                   newDe.next = static_cast<uint16_t>(nextFree);
               }
               // update new page free list
               newDp->free_list = nextFree;
               assert(newDp->num_free > 0);
               newDp->num_free--;
               oldDataIndex = oldDe.next;
               // link old data item into old page free list
               oldDe.next = static_cast<uint16_t>(oldDp->free_list);
               oldDp->free_list = oldDataIndex;
               oldDp->num_free++;
            }
         }
      }
   }
}

int BookWriter::write(const char* pathName) {
   std::ofstream book_file(pathName, std::ios::out | std::ios::trunc | std::ios::binary);
   book::BookHeader header;
   header.version = book::BOOK_VERSION;
   uint16_t pages = (uint16_t)index_pages;
   header.num_index_pages = (uint16_t)swapEndian16((uint8_t*)&pages);
   book_file.write((char*)&header, sizeof(book::BookHeader));
   if (book_file.fail()) return -1;

   book::IndexPage empty;
   for (int i = 0; i < index_pages; i++) {
      book::IndexPage *ip = (index[i] == nullptr) ? &empty : index[i];
      // correct for endianness before disk write
      for (unsigned j = 0; j < ip->next_free; j++) {
         ip->index[j].page = (uint16_t)swapEndian16((uint8_t*)&ip->index[j].page);
         ip->index[j].index = (uint16_t)swapEndian16((uint8_t*)&ip->index[j].index);
         ip->index[j].hashCode = (uint64_t)swapEndian64((uint8_t*)&ip->index[j].hashCode);
      }
      ip->next_free = (uint32_t)swapEndian32((uint8_t*)&ip->next_free);
      book_file.write((char*)ip, sizeof(book::IndexPage));
      if (book_file.fail()) return -1;
   }
   for (book::DataPage *dp : data) {
      // correct for endianness before disk write
      dp->free_list = (uint32_t)swapEndian32((uint8_t*)&dp->free_list);
      dp->num_free = (uint32_t)swapEndian32((uint8_t*)&dp->num_free);
      for (int j = 0; j < book::DATA_PAGE_SIZE; j++) {
         dp->data[j].next = (uint16_t)swapEndian16((uint8_t*)&dp->data[j].next);
         dp->data[j].win = (uint32_t)swapEndian32((uint8_t*)&dp->data[j].win);
         dp->data[j].loss = (uint32_t)swapEndian32((uint8_t*)&dp->data[j].loss);
         dp->data[j].draw = (uint32_t)swapEndian32((uint8_t*)&dp->data[j].draw);
      }
      book_file.write((char*)dp, sizeof(book::DataPage));
      if (book_file.fail()) return -1;
   }
   book_file.close();
   return 0;
}
