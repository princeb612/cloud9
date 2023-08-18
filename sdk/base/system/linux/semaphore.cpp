/* vim: set tabstop=4 shiftwidth=4 softtabstop=4 expandtab smarttab : */
/**
 * @file {file}
 * @author Soo han, Kim (princeb612.kr@gmail.com)
 * @desc
 *
 * Revision History
 * Date         Name                Description
 */

#include <hotplace/sdk/base/system/linux/semaphore.hpp>
#include <sys/time.h>

namespace hotplace {
namespace io {

semaphore::semaphore ()
{
    sem_init (&_sem, 0, 0);
}

semaphore::~semaphore ()
{
    sem_destroy (&_sem);
}

uint32 semaphore::signal ()
{
    sem_post (&_sem);
    return 0;
}

uint32 semaphore::wait (unsigned msec)
{
    int ret_value = 0;

    if ((unsigned) -1 == msec) {
        sem_wait (&_sem);
    } else {
        struct timeval now;
        gettimeofday (&now, nullptr);
        struct timespec ts;
        ts.tv_sec = (now.tv_sec) + (msec / 1000);
        ts.tv_nsec = (now.tv_usec * 1000) + (msec % 1000) * 1000000;
        ts.tv_sec += ts.tv_nsec / 1000000000;
        ts.tv_nsec %= 1000000000;
        ret_value = sem_timedwait (&_sem, &ts);
    }
    return ret_value;
}

}
}  // namespace
