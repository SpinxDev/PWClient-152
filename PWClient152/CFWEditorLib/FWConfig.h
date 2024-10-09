#define FW_FILE_VERSION 2

#define GRAVITY_ACC 9.8f


// if this flag is set all assebmly will be load to memory
// the first fweditor start
// or assembly will be load from file each time it is used
//#define _ENABLE_ASSEMBLY_CACHE

// when _ENABLE_ASSEMBLY_CACHE is set, 
// this flag is ignored, the partcount cache is always on
// when _ENABLE_ASSEMBLY_CACHE is not set
// and this flag is set, partcount will remain unchanged
// even if its gfx is changed and reloaded from file
//#define _ENABLE_PARTCOUNT_CACHE

// how many commands can be stored in the commond list
// (that means how many operations can be undone)
// set to -1 means unlimited 
#define _COMMAND_LIST_LIMIT	100