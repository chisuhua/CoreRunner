#include "hwconfig.h"
#include "QuarkTS.h"
#include <stdint.h>


qTask_t GenerateKernel, GetNewKernel, FindOneKernel, ALlocateDone, KernelDone, Dispach[Clusters], \
      UpdateResTable[Clusters], SlectSmem, UpdateAvailCtaNum[Clusters], GridWalker;


qBool_t isBufferEmpty(Buffer *b) {
    return (b->wptr == b->rptr) && !b->full);
}

qBool_t isBufferFull(Buffer *b) {
    return (b->full);
}

void incBufferRptr(Buffer *b) {
    b->rptr = (b->rptr + 1 ) % b->depth;
}

void incBufferWptr(Buffer *b) {
    b->wptr = (b->wptr + 1 ) % b->depth;
}

const char*res_table_events[]= {"Alloc", "Dealloc"};

int cluster_ids[clusters] = {0, 1};

int cur_disp_entry_id = -1;

DispEntry dispatch_table[DispEntries];
GridWalkBuffer gridwalk_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};

AllocBuffer alloc_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};
CtaDispBuffer   cta_disp_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};
WarpDispMsgBuffer warp_dispmsg_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};


bool available_tb_num_valid[Clusters] = {false};
int available_tb_num_table[Clusters][CUsInCluster] = {0};

SmemRsrcSlot smem_tables[Clusters][CUsInCluster];
CtaRsrcSlot cta_tables[Clusters][CUsInCluster];
WarpRsrcSlot warp_tables[Clusters][CUsInCluster][PEsInCU];

int sreg_tables[Clusters][CUsInCluster][PEsInCU];
int vreg_tables[Clusters][CUsInCluster][PEsInCU];

typedef struct {
    int cluster;
    int cu;
} CuPtr;

CuPtr cta_walker;


template<typename T>
void InitSlotTable(T &tab, int num) {
    tab.remain_slot_num = num;
    tab.slot_num = num;
    for (int i = 0; i < num; i++) {
        tab.table[i] = false;
    }
}

void InitBuffer(Buffer* buffer, int depth) {
  buffer->depth = depth;
}

void InitGridWalker {
    for (int i = 0; i < DispEntries; i++) {
        dispatch_table[i].valid = false;
    }
    InitBuffer(&gridwalk_buffer, BufferDepth);
    for (int cls_id = 0; cls_id < Clusters; cls_id++ ) {
        InitBuffer(&alloc_buffer, BufferDepth);
        InitBuffer(&cta_disp_buffer, BufferDepth);
        InitBuffer(&warp_dispmsg_buffer, BufferDepth);
        for (int cu=0; cu < CUsInCluster; cu++) {
            pe_ptrs[cls_id][cu].pe = 0;
            InitSlotTable(smem_tables[cls_id][cu], CtaSlots);
            InitSlotTable(cta_tables[cls_id][cu], CtaSlots);
            for (int pe=0; pe <) PEsInCU; pe++) {
                sreg_tables[cls_id][cu][pe] = SregsInPE;
                vreg_tables[cls_id][cu][pe] = VregsInPE;
                IntSlotTable(warp_tables[cls_id][cu][pe], WarpSlots);
            }
        }
    }
    cta_walker.cluster = 0
    cta_walker.cu = 0
}

void GenerateKernel(qEvent_t e) {
    for ( int i = 0; i < DispEntries; i++) {
        DispEntry* disp_entry = &dispatch_table[i];
        disp_entry->valid = true;
        disp_entry->k_info.disp_id = i;
        disp_entry->k_info.grid_dim.x = 16;
        disp_entry->k_info.grid_dim.y = 16;
        disp_entry->k_info.grid_dim.z = 64;
        disp_entry->k_info.block_dim.x = 32;
        disp_entry->k_info.block_dim.y = 1;
        disp_entry->k_info.block_dim.z = 1;
        disp_entry->k_info.vreg_num = 8;
        disp_entry->k_info.sreg_num = 32;
        disp_entry->k_info.smem_line_num = 32;
        disp_entry->k_info.disp_config.cta_per_cu = 2;
        disp_entry->k_info.disp_config.walk_dim = {4, 4, 1};
        disp_entry->cur_cta_id = {0, 0, 0};
        disp_entry->walk_done = false;
        disp_entry->cta_num = 0;
        disp_entry->cta_alloc_num = 0;
        disp_entry->cta_done_num = 0;
        dbg("Generate New Kerenl disp_id=%d\n", i);
        break; 
    }
    if (cur_disp_entry_id == -1) {
        qNofityTask(taskFindOneKernel, NULL);      
    }
}

void FindOneKernel(qEvent_t e) {
    if (cur_disp_entry_id == -1) {
        for (int i=0; i < DispEntries; i++) {
            if (dispatch_table[i].valid && !dispatch_table[i].walk_done) {
                cur_disp_entry_id = i;
                dbg("Start to walk kernel with disp_id=%d\n"; i);
                break;
            }
        }
        for (int cluster_id =0; cluster_id < Clusters; cluster_id++) {
            available_tb_num_valid[cluster_id] = false;
            qNofityTask(taskUpdateAvailCtaNum[cluster_id], NULL);
        }
        qNofityTask(taskWalk, NULL)
    } 
}

int GetSmemAvailableTbNum(int cluster, int cu)  {
    int tsm_line_num = dispatch_table[cur_dispatch_entry_id].k_info.tsm_line_num;
    int warp_num = (dispatch_table[cur_dispatch_entry_id].k_info.block_dim.x * dispatch_table[cur_dispatch_entry_id].k_info.block_dim.y * dispatch_table[cur_dispatch_entry_id].k_info.block_dim.z + 32 - 1) / 32;
    int available_warp_num = 0;
    int available_tb_num = block_slot_tables[ce][cu][tsmp].remain_slot_num;
    int tsm_res_avail_tb_num = 0;

    for (int pu = 0; pu < PU_NUM_PER_TSMP; pu++) {
        for (int we = 0; we < WE_NUM_PER_PU; we++) {
            available_warp_num += GetWeAvailableWarpNum(ce, cu, tsmp, pu, we);
        }
    }

    if ((available_warp_num / warp_num) < available_tb_num) {
        available_tb_num = available_warp_num / warp_num;
    }
    if (tsm_tables[ce][cu][tsmp].wptr == 0) {
        tsm_res_avail_tb_num = TSM_LINE_NUM_PER_TSMP / tsm_line_num;
    } else {
        for (int i = 0; i < tsm_tables[ce][cu][tsmp].wptr; i++) {
            tsm_res_avail_tb_num += tsm_tables[ce][cu][tsmp].node_table[i].free_size / tsm_line_num;
        }
    }
    if (tsm_res_avail_tb_num < available_tb_num) {
        available_tb_num = tsm_res_avail_tb_num;
    }
    return available_tb_num;
}

void UpdateAvailCtaNum(qEvent_t e) {
  int cluster_id = *(int*)e->TaskData;
  if (cur_disp_entry_id >= 0 && isBufferEmpty(&alloc_buffer[0])) {
    available_tb_num_valid = qFalse; 
    for (int cu=0; cu < CUsInCluster; cu++) {
      available_tb_num_table[cluster_id][cu] = GetSm
    }
  }

    available_tb_num_valid[ce] = true;
    if (!Empty(traverse_buffer) && !qTask_HasPendingNotifications(&taskSelectTsmp)) {
        InsertTask(taskSelectTsmp, NULL);
    }
}




