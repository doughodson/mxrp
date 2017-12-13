
#include "mixr/ighost/cigi/OtwModel.hpp"

#include "mixr/ighost/cigi/Otm.hpp"

#include "mixr/base/String.hpp"

#include "mixr/models/player/Player.hpp"
#include "mixr/simulation/AbstractNib.hpp"

namespace mixr {
namespace cigi {

IMPLEMENT_SUBCLASS(OtwModel, "OtwModel")
EMPTY_SLOTTABLE(OtwModel)

OtwModel::OtwModel()
{
   STANDARD_CONSTRUCTOR()
}

void OtwModel::copyData(const OtwModel& org, const bool)
{
    BaseClass::copyData(org);

    setPlayer(org.player);

    state = org.state;
    ageCount = org.ageCount;
    checked = org.checked;
    typeMapper = org.typeMapper;
    rcount = org.rcount;
    hotActive = org.hotActive;

    playerID = org.playerID;

    const base::String* pp = org.federateName;
    federateName = pp;
}

void OtwModel::deleteData()
{
    setPlayer(nullptr);
}

// ---
// OtwModel() -- set & ref the player pointer
// ---
void OtwModel::setPlayer(models::Player* const p)
{
   if (player != nullptr) {
      player->unref();
      playerID = 0;
      federateName = nullptr;
   }

   player = p;

   if (player != nullptr) {
      player->ref();
      playerID = player->getID();
      const simulation::AbstractNib* nib = player->getNib();
      if (nib != nullptr) {
         federateName = nib->getFederateName();
      }
      else {
         federateName = nullptr;
      }
   }
}

// ---
// initialize() -- initialize the model
// ---
void OtwModel::initialize(models::Player* const p, const Otm** const otwModelTable, const int numModels)
{
   setPlayer(p);
   state = OtwModel::ACTIVE;
   ageCount = 999;
   hotActive = true;
   rcount = 999;
   checked = true;

   // If the OTW model table was provided, then look for a match.
   if (otwModelTable != nullptr && numModels > 0) {
      bool found = false;
      for (int i = 0; i < numModels && !found; i++) {
         const Otm* otwTypeMapper = otwModelTable[i];
         if (otwTypeMapper != nullptr) {
            if (otwTypeMapper->isMatchingPlayerType(p)) {
               // We found a match for our player in the OTW model table!
               typeMapper = otwTypeMapper;
               found = true;
            }
         }
      }
   }
}

// ---
// clear() -- clears the model
// ---
void OtwModel::clear()
{
   setPlayer(nullptr);
   state = INACTIVE;
   ageCount = 0;
   checked = false;
   typeMapper = nullptr;
   rcount = 0;
   hotActive = false;
   playerID = 0;
   federateName = nullptr;
}

}
}