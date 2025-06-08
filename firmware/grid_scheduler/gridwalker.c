#include "hwconfig.h"
#include "QuarkTS.h"
#include <stdint.h>


qTask_t taskGenerateKernel, taskGetNewKernel, taskFindOneKernel, taskAllocDone, taskKernelDone, taskDispach[Clusters], \
      taskUpdateResTable[Clusters], taskSlectSmem, taskUpdateAvailTbNum[Clusters], taskGridWalker;


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

AllocBuffer alloc_buffer[clusters]
CtaDispBuffer   cta_disp_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};
//WarpDispMsgBuffer warp_dispmsg_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};
WarpDispMsgBuffer bdmsg_buffer = {.wptr=0, .rptr=0, .full=false, .depth=BufferDepth};


bool available_tb_num_valid[Clusters] = {false};
int available_tb_num_table[Clusters][CUsInCluster] = {0};

SmemRsrcSlot smem_tables[Clusters][CUsInCluster];
tbRsrcSlot tb_slot_tables[Clusters][CUsInCluster];
WarpRsrcSlot warp_slot_tables[Clusters][CUsInCluster][PEsInCU];

int sreg_tables[Clusters][CUsInCluster][PEsInCU];
int vreg_tables[Clusters][CUsInCluster][PEsInCU];

int pe_ptr[Clusters][CUsInCluster];

int cur_ce_dispatch_num[CUsInCluster];

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


bool WalkInCube(struct Dim3 record[], struct Dim3 id_in_traverse_cube, struct Dim3 traverse_dim) {
    bool switch_cube = false;
    record[1] = id_in_traverse_cube;
    if (id_in_traverse_cube.x + 1 < traverse_dim.x) {
        record[1].x = id_in_traverse_cube.x + 1;
    } else {
        record[1].x = 0;
        if (id_in_traverse_cube.y + 1 < traverse_dim.y) {
            record[1].y = id_in_traverse_cube.y + 1;
        } else {
            record[1].y = 0;
            if (id_in_traverse_cube.z + 1 < traverse_dim.z) {
                record[1].z = id_in_traverse_cube.z + 1;
            } else {
                record[1].z = 0;
                switch_cube = true;
            }
        }
    }
    return switch_cube;
}

bool TraverseNextId(struct Dim3 grid_dim, struct Dim3 traverse_dim) {
    struct Dim3 id_in_grid = dispatch_table[cur_disp_entry_id].cur_block_id;
    struct Dim3 record[2];
    struct Dim3 cube_id_in_grid;
    struct Dim3 id_in_traverse_cube;
    struct Dim3 grid_dim_cube_align;
    struct Dim3 next_traverse_dim;
    cube_id_in_grid.x = id_in_grid.x / traverse_dim.x;
    cube_id_in_grid.y = id_in_grid.y / traverse_dim.y;
    cube_id_in_grid.z = id_in_grid.z / traverse_dim.z;
    id_in_traverse_cube.x = id_in_grid.x % traverse_dim.x;
    id_in_traverse_cube.y = id_in_grid.y % traverse_dim.y;
    id_in_traverse_cube.z = id_in_grid.z % traverse_dim.z;
    grid_dim_cube_align.x = (grid_dim.x + traverse_dim.x - 1) / traverse_dim.x;
    grid_dim_cube_align.y = (grid_dim.y + traverse_dim.y - 1) / traverse_dim.y;
    grid_dim_cube_align.z = (grid_dim.z + traverse_dim.z - 1) / traverse_dim.z;
    next_traverse_dim.x = ((cube_id_in_grid.x + 1) * traverse_dim.x) <= grid_dim.x ? traverse_dim.x : grid_dim.x - (cube_id_in_grid.x * traverse_dim.x);
    next_traverse_dim.y = ((cube_id_in_grid.y + 1) * traverse_dim.y) <= grid_dim.y ? traverse_dim.y : grid_dim.y - (cube_id_in_grid.y * traverse_dim.y);
    next_traverse_dim.z = ((cube_id_in_grid.z + 1) * traverse_dim.z) <= grid_dim.z ? traverse_dim.z : grid_dim.z - (cube_id_in_grid.z * traverse_dim.z);
    bool traverse_done = false;
    record[0] = cube_id_in_grid;
    if (TraverseInCube(record, id_in_traverse_cube, next_traverse_dim)) {
        if (cube_id_in_grid.x + 1 < grid_dim_cube_align.x) {
            record[0].x = cube_id_in_grid.x + 1;
        } else {
            record[0].x = 0;
            if (cube_id_in_grid.y + 1 < grid_dim_cube_align.y) {
                record[0].y = cube_id_in_grid.y + 1;
            } else {
                record[0].y = 0;
                if (cube_id_in_grid.z + 1 < grid_dim_cube_align.z) {
                    record[0].z = cube_id_in_grid.z + 1;
                } else {
                    record[0].z = 0;
                    traverse_done = true;
                }
            }
        }
    }
    dispatch_table[cur_disp_entry_id].cur_block_id.x = record[0].x * traverse_dim.x + record[1].x;
    dispatch_table[cur_disp_entry_id].cur_block_id.y = record[0].y * traverse_dim.y + record[1].y;
    dispatch_table[cur_disp_entry_id].cur_block_id.z = record[0].z * traverse_dim.z + record[1].z;
    return traverse_done;
}

// Task Traverser per pipe
void GridWalker(qEvent_t e) {
    while (cur_disp_entry_id >= 0 && !dispatch_table[cur_disp_entry_id].walk_done && !isBufferFull(gridwalk_buffer)) {
        gridwalk_buffer.buffer[gridwalk_buffer.wptr] = dispatch_table[cur_disp_entry_id].cur_block_id;
        IncWptr(gridwalk_buffer);
        ++dispatch_table[cur_disp_entry_id].block_num;
        dispatch_table[cur_disp_entry_id].walk_done = TraverseNextId(dispatch_table[cur_disp_entry_id].k_info.grid_dim, dispatch_table[cur_disp_entry_id].k_info.disp_config.walk_dim);
        if (!qTask_HasPendingNotifications(&taskSelectTsmp)) {
            NotifyTask(taskSelectTsmp, NULL);
        }
    }
}

// caculate max warp from v/sreg or remain_slots from warp_slot_tables
int GetPEAvailableWarpNum(int cluster, int cu, int pe) {
    int vreg_num = dispatch_table[cur_disp_entry_id].k_info.vreg_num;
    int sreg_num = dispatch_table[cur_disp_entry_id].k_info.sreg_num;
    int available_warp_num = warp_slot_tables[cluster][cu][pe].remain_slots;
    if (vreg_num > 0) {
        int align_size = (vreg_num + VREG_ALIGN_SIZE - 1) / VREG_ALIGN_SIZE * VREG_ALIGN_SIZE;
        int vreg_avail_wn = vreg_tables[cluster][cu][pe] / align_size;
        if (vreg_avail_wn < available_warp_num) {
            available_warp_num = vreg_avail_wn;
        }
    }
    if (sreg_num > 0) {
        int align_size = (sreg_num + SREG_ALIGN_SIZE - 1) / SREG_ALIGN_SIZE * SREG_ALIGN_SIZE;
        int sreg_avail_wn = sreg_tables[cluster][cu][pe] / align_size;
        if (sreg_avail_wn < available_warp_num) {
            available_warp_num = sreg_avail_wn;
        }
    }
    return available_warp_num;
}

int GetWarpNumFromDim(int dim_x, int dim_y, int dim_z)
    int warp_num = (dim_x * dim_y * dim_z + 32 - 1) / 32;
}

int GetCurDispWarps() {
    int warp_num = GetWarpNumFromDim(dispatch_table[cur_disp_entry_id].k_info.block_dim.x,
                                     dispatch_table[cur_disp_entry_id].k_info.block_dim.y,
                                     dispatch_table[cur_disp_entry_id].k_info.block_dim.z);
    return warp_num;
}

// caculate CU available tbnum from by total warps num in dim or smem resource
int GetSmemAvailableTbNum(int cluster, int cu)  {
    int smem_line_num = dispatch_table[cur_disp_entry_id].k_info.smem_line_num;
    int warp_num = GetCurDispWarps();
    int available_warp_num = 0;
    int available_tb_num = tb_slot_tables[cluster][cu].remain_slots;
    int smem_res_avail_tb_num = 0;

    for (int pe = 0; pe < PEsInCU; pu++) {
        available_warp_num += GetPEAvailableWarpNum(cluster, cu, pe);
    }

    if ((available_warp_num / warp_num) < available_tb_num) {
        available_tb_num = available_warp_num / warp_num;
    }
    if (smem_tables[cluster][cu].wptr == 0) {
        smem_res_avail_tb_num = Smemlines / smem_line_num;
    } else {
        for (int i = 0; i < smem_tables[ce][cu].wptr; i++) {
            smem_res_avail_tb_num += smem_tables[ce][cu].node_table[i].free_size / smem_line_num;
        }
    }
    if (smem_res_avail_tb_num < available_tb_num) {
        available_tb_num = smem_res_avail_tb_num;
    }
    return available_tb_num;
}

bool AllCeSmemCanNotDisp(int ce) {
    for (int cu = 0; cu < CUsInCluster; cu++) {
        if (available_tb_num_table[ce][cu] > 0 && cur_ce_dispatch_num[cu] < dispatch_table[cur_disp_entry_id].k_info.disp_config.tb_per_cu) {
            return false;
        }
    }
    return true;
}

void UpdateAvailTbNum(qEvent_t e) {
    int cluster_id = *(int*)e->TaskData;
    if (cur_disp_entry_id >= 0 && isBufferEmpty(&alloc_buffer[cluster])) {
        available_tb_num_valid[cluster_id] = qFalse; 
        for (int cu=0; cu < CUsInCluster; cu++) {
            available_tb_num_table[cluster_id][cu] = GetSmemAvailableTbNum(cluster_id, cu);
        }
    }
    available_tb_num_valid[cluster_id] = qTrue;
    if (!isBufferEmpty(gridwalk_buffer) && !qTask_HasPendingNotifications(&taskSelectSmem)) {
        NofityTask(taskSelectSmem, NULL);
    }
}

void allocBuffer(int cluster_id) {
    int rptr = gridwalk_buffer.rptr;
    int wptr = alloc_buffer[cluster_id].wptr;
    alloc_buffer[cluster_id].buffer[wptr].tb_id = gridwalk_buffer.buffer[rptr];
    IncWptr(alloc_buffer[cluster_id]);
    IncRptr(gridwalk_buffer);
}

// smem Select task per pipe
void SelectSmem(qEvent_t e) {
    int start_ce = select_ptr.ce;
    bool try_all = false;
    while (!isBufferEmpty(gridwalk_buffer) && !try_all) {
        while (!try_all) {
            //Make sure allocate buffer this ce has space
            //not allow update tb num this ce when selecting
            if (isBufferFull(alloc_buffer[select_ptr.ce])) {
                return;
            }
            if (!available_tb_num_valid[select_ptr.ce]) {
                select_ptr.ce = (select_ptr.ce + 1) % Clusters;
                select_ptr.cu = 0;
                if (start_ce == select_ptr.ce) try_all = true;
                for (int cu = 0; cu < CUsInCluster; cu++) {
                    cur_ce_dispatch_num[cu] = 0;
                }
                continue;
            }
            if (available_tb_num_table[select_ptr.ce][select_ptr.cu] > 0) {
                ++cur_ce_dispatch_num[select_ptr.cu];
                --available_tb_num_table[select_ptr.ce][select_ptr.cu];

                allocBuffer(select_ptr.ce);

                if (!qTask_HasPendingNotifications(&taskUpdateResTable[select_ptr.ce])) {
                    NotifyTask(taskUpdateResTable[select_ptr.ce], NULL);
                }
                if (cur_disp_entry_id >= 0 && !dispatch_table[cur_disp_entry_id].walk_done &&  !qTask_HasPendingNotifications(&taskGridWalker)) {
                    NotifyTask(taskGridWalker, NULL);
                }

                // trigger allocate tb
                if (AllCeSmemCanNotDisp(select_ptr.ce)) {
                    //Next CE
                    select_ptr.ce = (select_ptr.ce + 1) % Clusters;
                    select_ptr.cu = 0;
                    if (start_ce == select_ptr.ce) try_all = true;
                    for (int cu = 0; cu < CUsInCluster; cu++) {
                        cur_ce_dispatch_num[cu] = 0;
                    }
                } else {
                    select_ptr.cu = (select_ptr.cu + 1) % CUsInCluster;
                }
                break;
            } else {
                if (AllCeSmemCanNotDisp(select_ptr.ce)) {
                    //Next CE
                    select_ptr.ce = (select_ptr.ce + 1) % Clusters;
                    select_ptr.cu = 0;
                    if (start_ce == select_ptr.ce) try_all = true;
                    for (int cu = 0; cu < CUsInCluster; cu++) {
                        cur_ce_dispatch_num[cu] = 0;
                    }
                } else {
                    select_ptr.cu = (select_ptr.cu + 1) % CUsInCluster;
                }
            }
        }
    }
}


// Allocate/Deallocate Task
void UpdateResTable(qEvent_t e) {
    int ce = *(int*)e->TaskData;
    while (!isBufferEmpty(alloc_buffer[ce]) && !isBufferFull(dispatch_buffer[ce])) {
        struct TbDispInfo* tb_info = &dispatch_buffer[ce].buffer[dispatch_buffer[ce].wptr];
        int smem_line_num = dispatch_table[cur_disp_entry_id].k_info.smem_line_num;
        tb_info->disp_id = dispatch_table[cur_disp_entry_id].k_info.disp_id;
        tb_info->block_id = alloc_buffer[ce].buffer[alloc_buffer[ce].rptr].block_id;
        tb_info->vreg_num = dispatch_table[cur_disp_entry_id].k_info.vreg_num;
        tb_info->sreg_num = dispatch_table[cur_disp_entry_id].k_info.sreg_num;
        tb_info->smem_line_num = smem_line_num;
        tb_info->warps_mask_pe0 = 0;
        tb_info->warps_mask_pe1 = 0;
        tb_info->warps_mask_pe2 = 0;
        tb_info->warps_mask_pe3 = 0;
        int cu = tb_info->cu;
        IncRptr(alloc_buffer[ce]);
        if (!isBufferEmpty(gridwalk_buffer) && !qTask_HasPendingNotifications(&taskSelectSmem)) {
            NotifyTask(taskSelectSmem, NULL);
        }
        // allocate block_slot and tsm
        for (int i = 0; i < tb_slot_tables[ce][cu].slot_num; i++) {
            if (!tb_slot_tables[ce][cu].table[i]) {
                tb_info->tb_id = i;
                tb_slot_tables[ce][cu].table[i] = true;
                --tb_slot_tables[ce][cu].remain_slot_num;
                break;
            }
        }

        if (smem_line_num != 0) {
            int wptr = smem_tables[ce][cu].wptr;
            if (wptr == 0) {
                smem_tables[ce][cu].node_table[0].valid = true;
                smem_tables[ce][cu].node_table[0].busy_size = smem_line_num;
                smem_tables[ce][cu].node_table[0].base = 0;
                smem_tables[ce][cu].node_table[0].tb_id = tb_info->tb_id;
                smem_tables[ce][cu].node_table[0].free_size = Smemlines - smem_line_num;
                ++smem_tables[ce][cu].wptr;
                tb_info->smem_base = 0;
            } else {
                for (int ptr = wptr - 1; ptr >= 0; ptr--) {
                    if (smem_tables[ce][cu].node_table[ptr].free_size > smem_line_num) {
                        for (int i = wptr - 1; i > ptr; i--) {
                            smem_tables[ce][cu].node_table[i+1] = smem_tables[ce][cu].node_table[i];
                        }
                        smem_tables[ce][cu].node_table[ptr+1].valid = true;
                        smem_tables[ce][cu].node_table[ptr+1].busy_size = smem_line_num;
                        smem_tables[ce][cu].node_table[ptr+1].base = smem_tables[ce][cu].node_table[ptr].base + smem_tables[ce][cu].node_table[ptr].busy_size;
                        smem_tables[ce][cu].node_table[ptr+1].tb_id = tb_info->tb_id;
                        smem_tables[ce][cu].node_table[ptr+1].free_size = smem_tables[ce][cu].node_table[ptr].free_size - smem_line_num;
                        ++smem_tables[ce][cu].wptr;
                        tb_info->smem_base = smem_tables[ce][cu].node_table[ptr+1].base;
                        break;
                    }
                }
            }
        }

        // allocate pe resource
        int warp_num = GetCurDispWarps();
        int align_vreg_num = (dispatch_table[cur_disp_entry_id].k_info.vreg_num + VREG_ALIGN_SIZE - 1) / VREG_ALIGN_SIZE * VREG_ALIGN_SIZE;
        int align_sreg_num = (dispatch_table[cur_disp_entry_id].k_info.sreg_num + SREG_ALIGN_SIZE - 1) / SREG_ALIGN_SIZE * SREG_ALIGN_SIZE;
        for (int warp_id = 0; warp_id < warp_num; warp_id++) {
            while(true) {
                bool can_dispatch = true;
                int pu = pu_ptrs[ce][cu];
                can_dispatch &= warp_slot_tables[ce][cu][pu].remain_slots > 0;
                can_dispatch &= vreg_tables[ce][cu][pu] >= align_vreg_num;
                can_dispatch &= sreg_tables[ce][cu][pu] >= align_sreg_num;
                if (can_dispatch) {
                    // allocate warp slot
                    for (int i = 0; i < warp_slot_tables[ce][cu][pu].slot_num; i++) {
                        if (!warp_slot_tables[ce][cu][pu].table[i]) {
                            warp_slot_tables[ce][cu][pu].table[i] = true;
                            --warp_slot_tables[ce][cu][pu].remain_slotnum;
                            switch (we_ptr.pu * WE_NUM_PER_PU + we_ptr.we) {
                                case 0: {
                                    tb_info->warps_mask_pu0 |= 1 << i;
                                    break;
                                }
                                case 1: {
                                    tb_info->warps_mask_pu1 |= 1 << i;
                                    break;
                                }
                                case 2: {
                                    tb_info->warps_mask_pu2 |= 1 << i;
                                    break;
                                }
                                case 3: {
                                    tb_info->warps_mask_pu3 |= 1 << i;
                                    break;
                                }
                                default:
                                    break;
                            }
                            break;
                        }
                    }
                    // allocate reg resource
                    vreg_tables[ce][cu][we_ptr.pe] -= align_vreg_num;
                    sreg_tables[ce][cu][we_ptr.pe] -= align_sreg_num;
                    we_ptrs[ce][cu].pe = (we_ptrs[ce][cu].pu + 1) % PU_NUM_PER_TSMP;
                    if (we_ptrs[ce][cu].pu == 0) {
                        we_ptrs[ce][cu].we = (we_ptrs[ce][cu].we + 1) % WE_NUM_PER_PU;
                    }
                    break;
                } else {
                    we_ptrs[ce][cu].pu = (we_ptrs[ce][cu].pu + 1) % PU_NUM_PER_TSMP;
                    if (we_ptrs[ce][cu].pu == 0) {
                        we_ptrs[ce][cu].we = (we_ptrs[ce][cu].we + 1) % WE_NUM_PER_PU;
                    }
                }
            }
        }
        IncWptr(dispatch_buffer[ce]);
        if (!qTask_HasPendingNotifications(&taskDispatch[ce])) {
            NotifyTask(taskDispatch[ce], NULL);
        }
        dbgPrint("Dispatch tb: disp_id= %d block_id=(%d,%d,%d) to (%d,%d,%d)\n", tb_info->disp_id, tb_info->block_id.x, tb_info->block_id.y, tb_info->block_id.z, ce, tb_info->tsmpid.cu, tb_info->tsmpid.tsmp);
        ++dispatch_table[cur_disp_entry_id].block_allocate_num;
        if (dispatch_table[cur_disp_entry_id].walk_done && dispatch_table[cur_disp_entry_id].block_num == dispatch_table[cur_disp_entry_id].block_allocate_num) {
            //TODO trigger allocate done
            NotifyTask(taskAllocateDone, NULL);
        }
    }
}

void UpdateResforDealloc(qEvent_t e) {
    int rptr = bdmsg_buffer[ce].rptr;
    BdGdMsg* msg = &bdmsg_buffer[ce].buffer[rptr];

    while (!isBufferEmpty(bdmsg_buffer[ce])) {
        IncRptr(bdmsg_buffer[ce]);

        if (!isBufferEmpty(dispatch_buffer[ce]) && !qTask_HasPendingNotifications(&taskDispatch[ce])) {
            NotifyTask(taskDispatch[ce], NULL);
        }

        if (msg->warp_done) {
            int align_vreg_num = (msg->vreg_num + VREG_ALIGN_SIZE - 1) / VREG_ALIGN_SIZE * VREG_ALIGN_SIZE;
            int align_sreg_num = (msg->sreg_num + SREG_ALIGN_SIZE - 1) / SREG_ALIGN_SIZE * SREG_ALIGN_SIZE;
            warp_slot_tables[ce][msg->cu_id][msg->pu_id].table[msg->warp_id] = false;
            ++warp_slot_tables[ce][msg->cu_id][msg->pu_id].remain_slot_num;
            vreg_tables[ce][msg->cu_id][msg->pu_id] += align_vreg_num;
            sreg_tables[ce][msg->cu_id][msg->pu_id] += align_sreg_num;
            dbgPrint("Warp Done: disp_id= %d tb_id %d weid=(%d,%d) warp_id= %d\n", msg->disp_id, ce, msg->cu_id, msg->tb_id, msg->pu_id, msg->we_id, msg->warp_id);
            if (msg->last_warp) {
                tb_slot_tables[ce][msg->cu_id].table[msg->tb_id] = false;
                ++tb_slot_tables[ce][msg->cu_id].remain_slot_num;
                if (msg->smem_line_num != 0) {
                    if (smem_tables[ce][msg->cu_id].wptr == 1) {
                        smem_tables[ce][msg->cu_id].node_table[0].valid = false;
                        --smem_tables[ce][msg->cu_id].wptr;
                    } else {
                        for (int ptr = 0; ptr < smem_tables[ce][msg->cu_id].wptr; ptr++) {
                            if (smem_tables[ce][msg->cu_id].node_table[ptr].valid && (smem_tables[ce][msg->cu_id].node_table[ptr].tb_id == msg->tb_id)) {
                                int merge_ptr = (ptr == 0) ? smem_tables[ce][msg->cu_id].wptr - 1 : ptr - 1;
                                smem_tables[ce][msg->cu_id].node_table[merge_ptr].free_size += smem_tables[ce][msg->cu_id].node_table[ptr].busy_size;
                                for (int i = ptr; i < smem_tables[ce][msg->cu_id].wptr; i++) {
                                    smem_tables[ce][msg->cu_id].node_table[i] = smem_tables[ce][msg->cu_id].node_table[i+1];
                                    smem_tables[ce][msg->cu_id].node_table[i+1].valid = false;
                                }
                                break;
                            }
                        }
                        --smem_tables[ce][msg->cu_id].wptr;
                    }
                }
                ++dispatch_table[msg->disp_id].block_done_num;
                dbgPrint("TB Done: disp_id= %d tb_id= %d\n", msg->disp_id, ce, msg->cu_id, msg->tb_id);
                if (dispatch_table[msg->disp_id].traverse_done && dispatch_table[msg->disp_id].block_num == dispatch_table[msg->disp_id].block_done_num) {
                    //TODO trigger kernel done
                    NotifyTask(KernelDone, &dispatch_table[msg->disp_id].k_info.disp_id);
                }
            }
            if (!qTask_HasPendingNotifications(&taskUpdateAvailTbNum[ce])) {
                NotifyTask(taskUpdateAvailTbNum[ce], NULL);
            }
        }
    }
}


// dispatch task
void Dispatch(qEvent_t e) {
    // send tb to bd
    int ce = *(int*)e->TaskData;
    int rptr = dispatch_buffer[ce].rptr;
    int wprt = bdmsg_buffer[ce].wptr;
    TbDispInfo* tb_info = &dispatch_buffer[ce].buffer[rptr];
    WarrpDispMsgBuffer* bd_msg = &bdmsg_buffer[ce].buffer[wptr];
    while (!isBufferEmpty(dispatch_buffer[ce]) && !isBufferFull(bdmsg_buffer[ce])) {
        int pu_id;
        int warp_id;
        if (tb_info->warps_mask_pu0) {
            pu_id = 0;
            warp_id = __builtin_ctz(tb_info->warps_mask_pu0);
            tb_info->warps_mask_pu0 -= 1 << warp_id;
        } else if (tb_info->warps_mask_pu1) {
            pu_id = 1;
            warp_id = __builtin_ctz(tb_info->warps_mask_pu1);
            tb_info->warps_mask_pu1 -= 1 << warp_id;
        } else if (tb_info->warps_mask_pu2) {
            pu_id = 2;
            warp_id = __builtin_ctz(tb_info->warps_mask_pu2);
            tb_info->warps_mask_pu2 -= 1 << warp_id;
        } else if (tb_info->warps_mask_pu3) {
            pu_id = 3;
            warp_id = __builtin_ctz(tb_info->warps_mask_pu3);
            tb_info->warps_mask_pu3 -= 1 << warp_id;
        }
        bd_msg->cu_id = tb_info->cu;
        bd_msg->pu_id = pu_id;
        bd_msg->warp_id = warp_id;
        bd_msg->tb_id = tb_info->tb_id;
        bd_msg->disp_id = tb_info->disp_id;
        bd_msg->vreg_num = tb_info->vreg_num;
        bd_msg->sreg_num = tb_info->sreg_num;

        bd_msg->smem_line_num = tb_info->smem_line_num;
        bd_msg->warp_done = true;
        bd_msg->last_warp = !(tb_info->warps_mask_pu0 | tb_info->warps_mask_pu1 | tb_info->warps_mask_pu2 | tb_info->warps_mask_pu3);
        IncWptr(bdmsg_buffer[ce]);

        //TODO trigger n warp done
        if (bd_msg->last_warp) {
            IncRptr(dispatch_buffer[ce]);
        }
        NotifyTask(taskUpdateResforDealloc[ce], NULL);
    }
}


// allocation done or kernel done task
void KernelDone(qEvent_t e) {
    int disp_id = *(int*)e->EventData;
    dispatch_table[disp_id].valid = false;
}

void AllocateDone(qEvent_t e) {
    dbgPrint("Kernel Allocate Done disp_id=%d\n", cur_disp_entry_id);
    //send alloc done to cp
    cur_disp_entry_id = -1;
    //TODO trigger find kernel
    qTask_Notification_Queue( &taskFindOneKernel, NULL);
}
// get cp new kernel
void GetNewKernel(qEvent_t e) {
    struct KernelInfo* k_info_ptr = (struct KernelInfo*)e->EventData;
    dispatch_table[k_info_ptr->disp_id].valid = true;
    dispatch_table[k_info_ptr->disp_id].k_info = *k_info_ptr;
    dispatch_table[k_info_ptr->disp_id].cur_block_id = (struct Dim3){0,0,0};
    dispatch_table[k_info_ptr->disp_id].traverse_done = false;
    dispatch_table[k_info_ptr->disp_id].block_num = 0;
    dispatch_table[k_info_ptr->disp_id].block_allocate_num = 0;
    dispatch_table[k_info_ptr->disp_id].block_done_num = 0;
    free(e->EventData);
    if (cur_disp_entry_id == -1) {
        //TODO trigger find kernel
        qTask_Notification_Queue( &taskFindOneKernel, NULL);
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
            NofityTask(taskUpdateAvailTbNum[cluster_id], NULL);
        }
        NofityTask(taskGridWalker, NULL)
    } 
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
        disp_entry->k_info.disp_config.tb_per_cu = 2;
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
        NofityTask(taskFindOneKernel, NULL);      
    }
}

void putUART(void *sp, const char c) {
    putchar(c);
}

void main() {
    qTrace_Set_OutputFcn(putUART);

    InitGridWalker();
    qOS_Setup(NULL, &TaskIdle);

    qOS_Add_Task( &taskGenerateKernel, GenerateKernel, 0, 2.0f, 10, qEnabled, NULL );
    // qOS_Add_EventTask( &taskGetNewKernel, GetNewKernel, qHigh_Priority, NULL );
    qOS_Add_EventTask( &taskFindAKernel, FindOneKernel, qHigh_Priority, NULL );
    qOS_Add_EventTask( &taskAllocateDone, AllocateDone, qHigh_Priority, NULL );
    qOS_Add_EventTask( &taskKernelDone, KernelDone, qHigh_Priority, NULL );
    qOS_Add_EventTask( &taskSelectTsmp, SelectTsmp, qHigh_Priority, NULL );
    qOS_Add_EventTask( &taskTraverse, Traverser, qHigh_Priority, NULL );
    dbgPrint("add global task done\n");
    for (int ce = 0; ce < CE_NUM_PER_DIE; ce++) {
        qOS_Add_EventTask( &taskDispatch[ce], Dispatch, qHigh_Priority, &ce_ids[ce] );
        qOS_Add_EventTask( &taskUpdateResTable[ce], UpdateResourceTable, qHigh_Priority, &ce_ids[ce] );
        qOS_Add_EventTask( &taskUpdateAvailTbNum[ce], UpdateCeAvailTbNum, qHigh_Priority, &ce_ids[ce] );
    }
    qOS_Run();
}

