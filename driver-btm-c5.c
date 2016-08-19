//
// Created by Padilla Miguel on 05.08.16.
// If you find more code let me know
//

#include "driver-btm-c5.h"

static void bitmain_init(struct cgpu_info *bitmain)
{
    applog(LOG_INFO, "BitMain: Opened on %s", bitmain->device_path);
}


static bool bitmain_prepare(struct thr_info *thr)
{
    struct cgpu_info *bitmain = thr->cgpu;
    struct bitmain_info *info = bitmain->device_data;

    free(bitmain->works);
    bitmain->works = calloc(BITMAIN_MAX_WORK_NUM * sizeof(struct work *),
                            BITMAIN_ARRAY_SIZE);
    if (!bitmain->works)
        quit(1, "Failed to calloc bitmain works in bitmain_prepare");

    info->thr = thr;
    mutex_init(&info->lock);
    mutex_init(&info->qlock);
    if (unlikely(pthread_cond_init(&info->qcond, NULL)))
        quit(1, "Failed to pthread_cond_init bitmain qcond");
    cgsem_init(&info->write_sem);

    if (pthread_create(&info->read_thr, NULL, bitmain_get_results, (void *)bitmain))
        quit(1, "Failed to create bitmain read_thr");

    //if (pthread_create(&info->write_thr, NULL, bitmain_send_tasks, (void *)bitmain))
    //	quit(1, "Failed to create bitmain write_thr");

    bitmain_init(bitmain);

    return true;
}
