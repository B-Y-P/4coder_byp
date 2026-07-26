/*
* Mr. 4th Dimention - Allen Webster
*
* 26.08.2018
*
* Generic hash functions
*
*/

// TOP

#if !defined(FCODER_HASH_FUNCTIONS_CPP)
#define FCODER_HASH_FUNCTIONS_CPP

static u64
table_hash_u8(u8 *v, u64 size){
  u64 hash = 0;
  for (u8 *p = v, *e = v + size; p < e; p += 1){
    u8 k = *p;
    k *= 81;
    k = ((u8)(k << 4)) | ((u8)(k >> 4));
    hash ^= k;
    hash *= 11;
    hash += 237;
  }
  return(hash);
}
static u64
table_hash_u16(u16 *v, u64 size){
  u64 hash = 0;
  for (u16 *p = v, *e = v + size; p < e; p += 1){
    u16 k = *p;
    k *= 11601;
    k = ((u16)(k << 8)) | ((u16)(k >> 8));
    hash ^= k;
    hash *= 11;
    hash += 12525;
  }
  return(hash);
}
static u64
table_hash_u32(u32 *v, u64 size){
  u64 hash = 0;
  for (u32 *p = v, *e = v + size; p < e; p += 1){
    u32 k = *p;
    k *= 3432918353U;
    k = ((u32)(k << 16)) | ((u32)(k >> 16));
    hash ^= k;
    hash *= 11;
    hash += 2041000173U;
  }
  return(hash);
}
static u64
table_hash_u64(u64 *v, u64 size){
  u64 hash = 0;
  for (u64 *p = v, *e = v + size; p < e; p += 1){
    u64 k = *p;
    k *= 14744272059406101841ULL;
    k = ((u64)(k << 32)) | ((u64)(k >> 32));
    hash ^= k;
    hash *= 11;
    hash += 8766028991911375085ULL;
  }
  return(hash);
}
static u64
table_hash(void *v, i32 it_size, u64 size){
  switch (it_size){
    case 1:  return table_hash_u8((u8*)v, size);
    case 2:  return table_hash_u16((u16*)v, size);
    case 4:  return table_hash_u32((u32*)v, size);
    case 8:  return table_hash_u64((u64*)v, size);
    default: return table_hash_u8((u8*)v, it_size*size);
  }
}

static u64 table_hash(u64 key){
  return table_hash_u8((u8*)&key, sizeof(u64));
}

#endif

// BOTTOM
