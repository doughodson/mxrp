
#ifndef __mxrp_models_AbstractAtmosphere_H__
#define __mxrp_models_AbstractAtmosphere_H__

#include "mxrp/base/Component.hpp"

namespace oe {
namespace models {

class AbstractAtmosphere : public base::Component
{
   DECLARE_SUBCLASS(AbstractAtmosphere, base::Component)

public:
   AbstractAtmosphere();
};

}
}

#endif
