/**
 * @file        ClipsArbiter.cpp
 * @brief       Implementation of class ClipsArbiter.
 * @details     
 *
 * @author      Marsil de Athayde Costa e Silva,
 * @author      Instituto Tecnologico de Aeronautica - ITA
 * @author      Laboratorio de Comando e Controle - ITA LAB C2
 * 
 * @date        15/04/2014
 * @version     1.0
 * @pre         
 * @bug         
 * @copyright   Copyright (c) 2014
 *
 */
#include "openeaagles/ubf/clips/ClipsArbiter.h"

namespace oe {
namespace clips {

IMPLEMENT_SUBCLASS(ClipsArbiter, "ClipsArbiter")
EMPTY_COPYDATA(ClipsArbiter)
EMPTY_SERIALIZER(ClipsArbiter)

/** Slot table for this class type. */
BEGIN_SLOTTABLE(ClipsArbiter)
   "clipsFileName",                /**  1) clipsFileName */
END_SLOTTABLE(ClipsArbiter)

/** Mapping of slots to handles. */
BEGIN_SLOT_MAP(ClipsArbiter)
   ON_SLOT(1, setSlotClipsFileName, base::String)
END_SLOT_MAP()


/**
 * @brief Class constructor.
 */
ClipsArbiter::ClipsArbiter()
{
    STANDARD_CONSTRUCTOR()
    
//    clipsCppEnv         = new Clips::CLIPSCPPEnv();
    clipsCppEnv         = new clips::CLIPSCPPEnv();
    clipsFileName       = new base::String();
}

/**
 * @brief Delete data.
 */
void ClipsArbiter::deleteData()
{
    if ( clipsFileName != 0 ) { clipsFileName -> unref(); clipsFileName = 0; }
    if ( clipsCppEnv != 0 ) { clipsCppEnv -> ~CLIPSCPPEnv(); clipsCppEnv = 0; }
}

/**
 * @brief Generates an action.
 */
base::ubf::Action* ClipsArbiter::genAction(const base::ubf::State* const state, const double dt)
{
    static double l_timer = 1;
    l_timer += dt;
    // create list for action set
    base::List* actionSet = new base::List();

    // fill out list of recommended actions by behaviors
    if( l_timer >= 1.0 ) 
    {
        //clearBehaviors();
        assertFacts( (oe::behaviors::PlaneState*) state );
        clipsCppEnv -> Run();
        getFacts();
        l_timer = 0;
    }

    // return action to perform generated by our base class
    return BaseClass::genAction( state, dt );
}


/**
 * @brief Generates a complex action from a set of actions.
 * Default: select the action with the highest vote.
 */
base::ubf::Action* ClipsArbiter::genComplexAction(base::List* const actionSet)
{
    oe::behaviors::PlaneAction * complexAction = new oe::behaviors::PlaneAction;

    unsigned int maxPitchVote = 0;
    unsigned int maxRollVote = 0;
    unsigned int maxHeadingVote = 0;
    unsigned int maxFireVote = 0;
    unsigned int maxThrottleVote = 0;
    unsigned int maxPitchTrimVote = 0;

    // process entire action set
    const base::List::Item* item = actionSet->getFirstItem();
    while ( item != 0 )
    {
        const oe::behaviors::PlaneAction * action = dynamic_cast<const oe::behaviors::PlaneAction*> ( item->getValue() );
        if (action!=0)
        {
            if( action->isHeadingChanged() && action->getVote() > maxHeadingVote )
            {
                complexAction->setHeading( action->getHeading() );
                maxHeadingVote = action->getVote();
            }
            if( action->isPitchChanged() && action->getVote() > maxPitchVote )
            {
                complexAction->setPitch( action->getPitch() );
                maxPitchVote = action->getVote();
            }
            if( action->isRollChanged() && action->getVote() > maxRollVote )
            {
                complexAction->setRoll( action->getRoll() );
                maxRollVote = action->getVote();
            }
            if( action->isFireMissileChanged() && action->getVote() > maxFireVote )
            {
                complexAction->setFireMissile( action->getFireMissile() );
                maxFireVote = action->getVote();
            }
            if( action->isThrottleChanged() && action->getVote() > maxThrottleVote )
            {
                complexAction->setThrottle( action->getThrottle() );
                maxThrottleVote = action->getVote();
            }
            if( action->isPitchTrimChanged() && action->getVote() > maxPitchTrimVote )
            {
                complexAction->setPitchTrim( action->getPitchTrim() );
                maxPitchTrimVote = action->getVote();
            }
        }
        else
        {
            std::cout << "Action NOT a PlaneAction\n";
        }
        // next action
        item = item->getNext();
    }
    
    trimChangeValidation(complexAction);
    return complexAction;
}

/**
 * @brief Clear the list of behavior.
 */
void ClipsArbiter::clearBehaviors()
{
    while( getBehaviors() -> removeHead() != 0 ){}
}

/**
 * @brief Set slot ClipsFileName.
 */
 bool ClipsArbiter::setSlotClipsFileName( base::String* const x )
{
    if ( clipsFileName != 0 )
    {
       clipsFileName -> unref();
       clipsFileName = 0;
    }
    if ( x != 0)
    {
       clipsFileName = x -> clone();
    }

    if( clipsCppEnv -> Load( getClipsFileName() -> getCopyString() ) )
    {
        clipsCppEnv -> Reset();
        //clipsCppEnv -> Watch( "facts" );
        deftemplate * l_deftemplate;

        l_deftemplate = (deftemplate*) FindDeftemplate( "behavior" );
        if( l_deftemplate != 0 )
        {
            m_behaviorBucket            = l_deftemplate -> header . name -> bucket;
            m_behaviorNameBucket        = l_deftemplate -> slotList -> slotName -> bucket;
            //m_vote                    = l_deftemplate -> slotList -> next -> slotName -> bucket;
            //m_voteOnIncomingMissile   = l_deftemplate -> slotList -> next -> next -> slotName -> bucket;
            //m_voteOnCriticalAltitude  = l_deftemplate -> slotList -> next -> next -> next -> slotName -> bucket;
            //m_criticalAltitude        = l_deftemplate -> slotList -> next -> next -> next -> next -> slotName -> bucket;
            m_optParamsBucket           = l_deftemplate -> slotList -> next -> next -> next -> next -> next -> slotName -> bucket;
            m_optPairStreamBucket       = l_deftemplate -> slotList -> next -> next -> next -> next -> next -> next -> slotName -> bucket;
            l_deftemplate = 0;
        }
        
        l_deftemplate = (deftemplate*) FindDeftemplate( "remover" );
        if( l_deftemplate != 0 )
        {
            m_removeBucket          = l_deftemplate -> header . name -> bucket;
            l_deftemplate = 0;
        }
    }

    return true;
}

/**
 * @brief getSlotByIndex()
 */
base::Object* ClipsArbiter::getSlotByIndex(const int si)
{
   return BaseClass::getSlotByIndex(si);
}

/**
 * @brief trimChangeValidation()
 */
void ClipsArbiter::trimChangeValidation(base::ubf::Action* const complexAction)
{
   oe::behaviors::PlaneAction* action = (oe::behaviors::PlaneAction*)(complexAction);

   if ( action->isPitchChanged() && action->isPitchTrimChanged() ) {
      //ignore pitch trim ONLY if the change goes against the action we want to
      //take with the control stick.
      if ( action->getPitch()>0 ) {
         //std::cout<<"Pitch: "<<action->getPitch();
         if ( action->getPitchTrim()<0 ) {
            //std::cout<<" Trim: "<<action->getPitchTrim();
            action->setPitchTrim(0);
         }
      } else {
         //std::cout<<"Pitch: "<<action->getPitch();
         if ( action->getPitchTrim()>0 ) {
            //std::cout<<" Trim: "<<action->getPitchTrim();
            action->setPitchTrim(0);
         }
      }
      //std::cout<<" "<<endl;
   }
   return;
}

/**
 * @brief Assert the CLIPS' facts from PlaneState.
 * @param state Current state of the aircraft.
 * @return Nothing.
 */
void ClipsArbiter::assertFacts( const base::ubf::State * state )
{
    char l_str[ 1024 ] = {0};
    const oe::behaviors::PlaneState * l_state = dynamic_cast<const oe::behaviors::PlaneState*> ( state );

    sprintf_s( l_str, "(assert (is-alive %s))",                 ( l_state -> isAlive() == true )              ? ( "yes" ) : ( "no" ) );
    clipsCppEnv -> Eval( l_str );

    sprintf_s( l_str, "(assert (roll-is %f))",                  l_state -> getRoll() );
    clipsCppEnv -> Eval( l_str );
    
    sprintf_s( l_str, "(assert (pitch-is %f))",                 l_state -> getPitch() );
    clipsCppEnv -> Eval( l_str );
    
    sprintf_s( l_str, "(assert (heading-is %f))",               l_state -> getHeading() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (roll-rate-is %f))",             l_state -> getRollRate() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (pitch-rate-is %f))",            l_state -> getPitchRate() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (yaw-rate-is %f))",              l_state -> getYawRate() );
    clipsCppEnv -> Eval( l_str );
    
    sprintf_s( l_str, "(assert (altitude-is %f))",              l_state -> getAltitude() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (throttle-is %f))",              l_state -> getThrottle() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (speed-is %f))",                 l_state -> getSpeed() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (pitch-trim-is %f))",            l_state -> getPitchTrim() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (num-tracks-is %d))",            l_state -> getNumTracks() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (is-tracking %s))",              ( l_state -> isTracking() == true )           ? ( "yes" ) : ( "no" ) );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (is-missile-fired %s))",         ( l_state -> isMissileFired() == true )       ? ( "yes" ) : ( "no" ) );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (target-track-is %d))",          l_state -> getTargetTrack() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (num-engines-is %d))",           l_state -> getNumEngines() );
    clipsCppEnv -> Eval( l_str );
     
    sprintf_s( l_str, "(assert (is-incoming-missile %s))",      ( l_state -> isIncomingMissile() == true )    ? ( "yes" ) : ( "no" ) );
    clipsCppEnv -> Eval( l_str );
    
    if( l_state -> isTracking() && l_state -> getTargetTrack() < l_state -> getNumTracks() )
    {
        sprintf_s( l_str, "(assert (pitch-to-tracked-is %f))",      l_state -> getPitchToTracked( l_state -> getTargetTrack() ) );
        clipsCppEnv -> Eval( l_str );
         
        sprintf_s( l_str, "(assert (heading-to-tracked-is %f))",    l_state -> getHeadingToTracked( l_state -> getTargetTrack() ) );
        clipsCppEnv -> Eval( l_str );
        
        sprintf_s( l_str, "(assert (distance-to-tracked-is %f))",   l_state -> getDistanceToTracked( l_state -> getTargetTrack() ) );
        clipsCppEnv -> Eval( l_str );
    }
    else
    {
        sprintf_s( l_str, "(assert (pitch-to-tracked-is %f))",      0 );
        clipsCppEnv -> Eval( l_str );
         
        sprintf_s( l_str, "(assert (heading-to-tracked-is %f))",    0 );
        clipsCppEnv -> Eval( l_str );
        
        sprintf_s( l_str, "(assert (distance-to-tracked-is %f))",   0 );
        clipsCppEnv -> Eval( l_str );
    }
}

/**
 * @brief Retrieves the facts from CLIPS' environment and transform them into Eaagles' objects and behaviors.
 * 
 * @return Nothing.
 * @author Marsil.
 */
void ClipsArbiter::getFacts()
{
    DATA_OBJECT returnValue;
    int end;
    void *multifieldPtr, *field;
    struct fact * l_fact;

    base::Object* l_bhv = 0;

    if( GetFactListChanged() )
    {
        GetFactList( &returnValue, NULL );

        if ( GetType( returnValue ) == MULTIFIELD )
        {
            end = GetDOEnd( returnValue );

            multifieldPtr = GetValue( returnValue );

            for ( int i = GetDOBegin( returnValue ); i <= end; i++ )
            {
                field = GetMFValue( multifieldPtr, i );
                l_fact = (struct fact *) field;

                if( l_fact -> whichDeftemplate -> header . name -> bucket == m_behaviorBucket )
                {
                    if( l_fact -> whichDeftemplate -> implied == 0 )
                    {
                        struct field *sublist;
                        struct deftemplate *theDeftemplate;
                        struct templateSlot *slotPtr;

                        theDeftemplate = l_fact -> whichDeftemplate;
                        sublist = l_fact -> theProposition . theFields;

                        slotPtr = theDeftemplate->slotList;

                        for( int k = 0; k < theDeftemplate -> numberOfSlots; k++ )
                        {
                            base::Object* l_param = 0;
                            // Behavior
                            if( slotPtr -> slotName -> bucket == m_behaviorNameBucket )
                            {
                                l_bhv = 0;
                                if( l_bhv == 0 )
                                {
                                    l_bhv =  behaviors::factory( ValueToString(sublist[k].value) );
                                }
                                if( l_bhv == 0 )
                                {
                                    l_bhv = lua::factory( ValueToString(sublist[k].value) );
                                }
                            }
                            else
                            {
                                // General SLOTS
                                switch ( sublist[k].type )
                                {
                                    case FLOAT:
                                    {
                                        l_param = new base::Number( ValueToDouble( sublist[k].value ) );
                                        break;
                                    }
                                    case INTEGER:
                                    {
                                        l_param = new base::Number( ValueToInteger( sublist[k].value ) );
                                        break;
                                    }
                                    case STRING:
                                    {
                                        l_param = new base::String( ValueToString( sublist[k].value ) );
                                        break;
                                    }
                                    case MULTIFIELD:
                                    {
                                        multifield * l_multifield = (multifield*) sublist[k].value;
                                        
                                        if( l_multifield -> multifieldLength != 0 )
                                        {
                                            base::Object* l_pairobj = 0;
                                            base::Pair* l_pair;
                                            l_param = new base::PairStream();
                                            for( int l = 0; l < l_multifield -> multifieldLength; l+=2 )
                                            {
                                                switch ( l_multifield -> theFields[ l+1 ] . type )
                                                {
                                                    case FLOAT:
                                                    {
                                                        l_pairobj = new base::Number( ValueToDouble( l_multifield -> theFields[ l+1 ] . value ) );
                                                        break;
                                                    }
                                                    case INTEGER:
                                                    {
                                                        l_pairobj = new base::Number( ValueToInteger( l_multifield -> theFields[ l+1 ] . value ) );
                                                        break;
                                                    }
                                                    case STRING:
                                                    {
                                                        l_pairobj = new base::String( ValueToString( l_multifield -> theFields[ l+1 ] . value ) );
                                                        break;
                                                    }
                                                    default:
                                                    {
                                                        break;
                                                    }
                                                }
                                                    
                                                if( slotPtr -> slotName -> bucket == m_optParamsBucket )
                                                {
                                                    l_bhv -> setSlotByName( ValueToString( l_multifield -> theFields[ l ] . value ), l_pairobj );
                                                    l_param = 0;
                                                }
                                                else if( slotPtr -> slotName -> bucket == m_optPairStreamBucket )
                                                {
                                                    l_pair = new base::Pair( ValueToString( l_multifield -> theFields[ l ] . value ), l_pairobj );
                                                    ( (base::PairStream*) l_param ) -> addTail( l_pair );
                                                }
                                            }
                                        }
                                        break;
                                    }
                                    default:
                                    {
                                        break;
                                    }
                                }

                                if( l_param != 0 )
                                {
                                    l_bhv -> setSlotByName( slotPtr -> slotName -> contents, l_param );
                                }
                            } 
                            slotPtr = slotPtr -> next;
                        }

                        addBehavior( ( base::ubf::Behavior* ) l_bhv );
                    }
                }
                else if( l_fact -> whichDeftemplate -> header . name -> bucket == m_removeBucket )
                {
                    clearBehaviors();
                }
                Retract( l_fact );
            }
        }
    }
}

}
}

