# Project 1

## TASK1: ARC

---

## ArcReplacer Functions

### 1. RecordAccess()

- Start off with recordaccess function of the cpp file  
- Implementation logic given on website  
- `.count` counts the no of the objects instances in the map  
- Since duplicate entries are ensured to not exist can be used as a check wether page/frame exists in ghost lists or actual lists  
- Further check for mfu or mru using arcstatus class  
- Remove and pushfront for removing and adding respectfully  
- Set arcstatus after ops  
- According to the website tests for performance will fail unless additional data structure is added  
- No idea what the bottleneck could be  
- A few yt videos on std lists later specifically this one:  
  https://www.youtube.com/watch?v=DMqlCXrFY0k  
- An iterator will speed up the searching through the whole list process so implement that  
- Replace:
  - `.remove(frame_id)`  
  - with `.erase(obj->iterator)`  
- And also update the iterator after adding the element at front of list with `.begin()`  
- Make an obj of type framestatus that can store the new element to add it to the alive map for all of the miss cases  
  - ( we use make_shared instead of shared_ptr here because we’re creating a new object not referencing the framestatus class )  

- Shared_ptr basically makes the pointer to the location in memory, shared  
  - This means that we can have multiple objects pointing to the same location in memory  

---

### 2. SetEvictable()

- Changes the state of a frame from evictable to nonevictable and vice versa  
- Check if frame_id argument is valid by assuring it lies in the replacer list  
  - If it is invalid throw exception  
- Check if frame exists by finding it in alive map  

- If it was previously not evict able:
  - set the bool evictable_ to 1  
  - increment curr_size_  

- Curr_size_ keeps track of number of evictable frames  

- Vice virsa if we want to set a frame from evictable to not evictable  

---

### 3. Remove()

- Removes pages from a frame and decrements the curr_size(the number of evictable frames) of the replacer accordingly  

- Frames only exist in mfu or mru not ghost so we check if the frameid is valid for the alive map  

- If valid:
  - check wether in mfu or mru  
  - erase it from both the list it is in (mfu or mru) and the alive map  

- Throw exception if frame is not evictable  

- If frame is not found in alive map just return nothing  

---

### 4. Size()

- Just return the no of evictable frames  

- return curr_size  

---


