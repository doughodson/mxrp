
#include "mixr/linkage/adapters/DiscreteOutput.hpp"

#include "mixr/base/concepts/linkage/AbstractIoData.hpp"
#include "mixr/base/concepts/linkage/AbstractIoDevice.hpp"

#include "mixr/base/numeric/Boolean.hpp"
#include "mixr/base/numeric/Integer.hpp"

#include <iostream>

namespace mixr {
namespace linkage {

IMPLEMENT_SUBCLASS(DiscreteOutput, "DiscreteOutput")
EMPTY_DELETEDATA(DiscreteOutput)

BEGIN_SLOTTABLE(DiscreteOutput)
    "do",         // 1) Discrete Output location (AbstractIoData's DO channel)
    "port",       // 2) Device port number (default: 0)
    "channel",    // 3) Device channel (bit) number on the port
    "inverted",   // 4) Inverted bit flag (default: false)
END_SLOTTABLE(DiscreteOutput)

BEGIN_SLOT_MAP(DiscreteOutput)
    ON_SLOT( 1, setSlotLocation, base::Integer)
    ON_SLOT( 2, setSlotPort,     base::Integer)
    ON_SLOT( 3, setSlotChannel,  base::Integer)
    ON_SLOT( 4, setSlotInverted, base::Boolean)
END_SLOT_MAP()

DiscreteOutput::DiscreteOutput()
{
   STANDARD_CONSTRUCTOR()
}

void DiscreteOutput::copyData(const DiscreteOutput& org, const bool)
{
   BaseClass::copyData(org);

   location = org.location;
   port = org.port;
   channel = org.channel;
   invert = org.invert;
}

void DiscreteOutput::processOutputsImpl(const base::AbstractIoData* const outData, base::AbstractIoDevice* const device)
{
   if (device != nullptr) {
      bool value{};

      // Get the bit from the cockpit output data handler
      if (outData != nullptr) {
         outData->getDiscreteOutput(location, &value);
      }

      // Send the bit to the DO card
      bool value0 {value};
      if (invert)
         value0 = !value;
      device->setDiscreteOutput(value0, channel, port);
   }
}

// location: Output array index (location)
bool DiscreteOutput::setSlotLocation(const base::Integer* const msg)
{
   bool ok {};
   if (msg != nullptr) {
      const int v {msg->asInt()};
      if (v >= 0) {
         ok = setLocation(v);
      }
   }
   return ok;
}

// port: DiHandler's port number
bool DiscreteOutput::setSlotPort(const base::Integer* const msg)
{
   bool ok {};
   if (msg != nullptr) {
      const int v {msg->asInt()};
      if (v >= 0) {
         ok = setPort(v);
      }
   }
   return ok;
}

// channel: DiHandler's channel (bit) number on the port
bool DiscreteOutput::setSlotChannel(const base::Integer* const msg)
{
   bool ok {};
   if (msg != nullptr) {
      const int v {msg->asInt()};
      if (v >= 0) {
         ok = setChannel(v);
      }
   }
   return ok;
}

// invert: Inverted bit flag (default: false)
bool DiscreteOutput::setSlotInverted(const base::Boolean* const msg)
{
   bool ok {};
   if (msg != nullptr) {
      ok = setInvertFlag( msg->asBool() );
   }
   return ok;
}

}
}
