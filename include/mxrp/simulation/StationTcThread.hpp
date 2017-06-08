
#ifndef __mxrp_simulation_StationTcThread_H__
#define __mxrp_simulation_StationTcThread_H__

#include "mxrp/base/concurrent/PeriodicTask.hpp"

namespace oe {
namespace simulation {

// ---
// Time-critical thread
// ---
class TcThread : public base::PeriodicTask
{
   DECLARE_SUBCLASS(TcThread, base::PeriodicTask)
   public: TcThread(base::Component* const parent, const double priority, const double rate);
   private: virtual unsigned long userFunc(const double dt) override;
};

}
}

#endif
