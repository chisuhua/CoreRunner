
#ifdef NDEBUG
#define dbg(fmt, ...) {}
#else
#define dbg(fmt, ...) printf(fmt, ##__VA_ARGS__);
#endif

#define NotifyTask(task, data) qTask_Notification(&task, data);
//#define qNotifyTask(task, data) qTask_Notification_Queue(&task, data);


#define BufferDepth 16
#define Clusters 16
#define CUsInCluster 4 
#define PEsInCU 16
#define CtaSlots 32
#define WarpSlots 32
#define SregAlign 32
#define VregAlign 32
#define SregsInPE 1024
#define VregsInPE 32
#define SmemLines 1024
#define DispEntries 128

#define container_of(ptr, type, member) ({                      \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);        \
    (type *)((char *)__mptr - offsetof(type, member)); })

typedef struct _Dim3 { int x; int y; int z;} Dim3

struct DispConfig { int cta_per_cu; Dim3 walk_dim;};

struct KernelInfo {
  int disp_id;
  Dim3 grid_dim;
  Dim3 block_dim;
  int vreg_num;
  int vreg_num;
  int smem_line_num;
  struct DispConfig disp_config;
};

typedef struct {
  bool valid;
  struct KernelInfo k_info;
  Dim3 cur_cta_id;
  bool walk_done;
  int cta_num;
  int cta_alloc_num;
  int cta_done_num;
} DispEntry;

typedef struct {
  int wptr;
  int rptr;
  bool full;
  int depth;
} Buffer;

typedef struct {
  Buffer buffer;
  Dim3 data[BufferDepth];
} GridWalkBuffer;

typedef struct {
  Dim3 block_id;
} AllocReq;

typedef struct {
  Buffer buffer;
  AllocReq data[BufferDepth];
} AllocBuffer;

typedef struct {
  int disp_id;
  Dim3 block_id;
  int tb_id;
  int smem_base;
  int vreg_num;
  int sreg_num;
  int smem_line_num;
  int warps_mask_pe3
  int warps_mask_pe2
  int warps_mask_pe1
  int warps_mask_pe0
} TbDispInfo;

typedef struct {
  Buffer buffer;
  CtaDispInfo data[BufferDepth];
} CtaDispBuffer;

typedef struct {
  int cu_id;
  int pe_id;
  int warp_id;
  int cta_id;
  int disp_id;
  int vreg_num;
  int sreg_num;
  int smem_line_num;
  int last_warp;
  int warp_done;
} WarpDispMsg;

typedef struct {
  Buffer buffer;
  WarpDisprMsg data[BufferDepth];
} WarpDispMsgBuffer;

typedef struct {
  int capacity;
  int free_size;
  int align;
} RsrcReg;

typedef struct { 
  int remain_slots;
  int slot_num;
  bool table[CtaSlots];
} CtaRsrcSlot;

typedef struct { 
  int remain_slots;
  int slot_num;
  bool table[WarpSlots];
} WarpRsrcSlot;

typedef struct {
  bool valid;
  int busy_size;
  int base;
  int cta_id;
  int free_size;
} RsrcSmem;

typedef struct {
  int wptr;
  int depth;
  struce RsrcSmem smem_resource[CtaSlots];
} SmemRsrcSlot ;


