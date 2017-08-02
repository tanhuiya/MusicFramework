// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../../core/elements/Technical.h"
#include "../../core/FromXElement.h"
#include "../../core/elements/TechnicalChoice.h"
#include "../../core/elements/Arrow.h"
#include "../../core/elements/Bend.h"
#include "../../core/elements/DoubleTongue.h"
#include "../../core/elements/DownBow.h"
#include "../../core/elements/Fingering.h"
#include "../../core/elements/Fingernails.h"
#include "../../core/elements/Fret.h"
#include "../../core/elements/HammerOn.h"
#include "../../core/elements/Handbell.h"
#include "../../core/elements/Harmonic.h"
#include "../../core/elements/Heel.h"
#include "../../core/elements/Hole.h"
#include "../../core/elements/OpenString.h"
#include "../../core/elements/OtherTechnical.h"
#include "../../core/elements/Pluck.h"
#include "../../core/elements/PullOff.h"
#include "../../core/elements/SnapPizzicato.h"
#include "../../core/elements/Stopped.h"
#include "../../core/elements/String.h"
#include "../../core/elements/Tap.h"
#include "../../core/elements/ThumbPosition.h"
#include "../../core/elements/Toe.h"
#include "../../core/elements/TripleTongue.h"
#include "../../core/elements/UpBow.h"
#include <iostream>

namespace mx
{
    namespace core
    {
        Technical::Technical()
        :myTechnicalChoiceSet()
        {}


        bool Technical::hasAttributes() const
        {
            return false;
        }


        std::ostream& Technical::streamAttributes( std::ostream& os ) const
        {
            return os;
        }


        std::ostream& Technical::streamName( std::ostream& os ) const
        {
            os << "technical";
            return os;
        }


        bool Technical::hasContents() const
        {
            return myTechnicalChoiceSet.size() > 0;
        }


        std::ostream& Technical::streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const
        {
            if ( hasContents() )
            {
                for ( auto x : myTechnicalChoiceSet )
                {
                    os << std::endl;
                    x->streamContents( os, indentLevel+1, isOneLineOnly );
                }
                os << std::endl;
            }
            else
            {
                isOneLineOnly = false;
            }
            return os;
        }


        const TechnicalChoiceSet& Technical::getTechnicalChoiceSet() const
        {
            return myTechnicalChoiceSet;
        }


        void Technical::addTechnicalChoice( const TechnicalChoicePtr& value )
        {
            if ( value )
            {
                myTechnicalChoiceSet.push_back( value );
            }
        }


        void Technical::removeTechnicalChoice( const TechnicalChoiceSetIterConst& value )
        {
            if ( value != myTechnicalChoiceSet.cend() )
            {
                myTechnicalChoiceSet.erase( value );
            }
        }


        void Technical::clearTechnicalChoiceSet()
        {
            myTechnicalChoiceSet.clear();
        }


        TechnicalChoicePtr Technical::getTechnicalChoice( const TechnicalChoiceSetIterConst& setIterator ) const
        {
            if( setIterator != myTechnicalChoiceSet.cend() )
            {
                return *setIterator;
            }
            return TechnicalChoicePtr();
        }
        
        
        bool Technical::fromXElement( std::ostream& message, xml::XElement& xelement )
        {
            bool isSuccess = true;
            xml::XElementIterator end = xelement.end();
            
            for( auto it = xelement.begin(); it != end; ++it )
            {
                const std::string elementName = it->getName();
                auto choice = makeTechnicalChoice();
                
                if( elementName == "up-bow" )
                {
                    choice->setChoice( TechnicalChoice::Choice::upBow );
                    isSuccess &= choice->getUpBow()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "down-bow" )
                {
                    choice->setChoice( TechnicalChoice::Choice::downBow );
                    isSuccess &= choice->getDownBow()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "harmonic" )
                {
                    choice->setChoice( TechnicalChoice::Choice::harmonic );
                    isSuccess &= choice->getHarmonic()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "open-string" )
                {
                    choice->setChoice( TechnicalChoice::Choice::openString );
                    isSuccess &= choice->getOpenString()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "thumb-position" )
                {
                    choice->setChoice( TechnicalChoice::Choice::thumbPosition );
                    isSuccess &= choice->getThumbPosition()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "fingering" )
                {
                    choice->setChoice( TechnicalChoice::Choice::fingering );
                    isSuccess &= choice->getFingering()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "pluck" )
                {
                    choice->setChoice( TechnicalChoice::Choice::pluck );
                    isSuccess &= choice->getPluck()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "double-tongue" )
                {
                    choice->setChoice( TechnicalChoice::Choice::doubleTongue );
                    isSuccess &= choice->getDoubleTongue()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "triple-tongue" )
                {
                    choice->setChoice( TechnicalChoice::Choice::tripleTongue );
                    isSuccess &= choice->getTripleTongue()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "stopped" )
                {
                    choice->setChoice( TechnicalChoice::Choice::stopped );
                    isSuccess &= choice->getStopped()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "snap-pizzicato" )
                {
                    choice->setChoice( TechnicalChoice::Choice::snapPizzicato );
                    isSuccess &= choice->getSnapPizzicato()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "fret" )
                {
                    choice->setChoice( TechnicalChoice::Choice::fret );
                    isSuccess &= choice->getFret()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "string" )
                {
                    choice->setChoice( TechnicalChoice::Choice::string_ );
                    isSuccess &= choice->getString()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "hammer-on" )
                {
                    choice->setChoice( TechnicalChoice::Choice::hammerOn );
                    isSuccess &= choice->getHammerOn()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "pull-off" )
                {
                    choice->setChoice( TechnicalChoice::Choice::pullOff );
                    isSuccess &= choice->getPullOff()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "bend" )
                {
                    choice->setChoice( TechnicalChoice::Choice::bend );
                    isSuccess &= choice->getBend()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "tap" )
                {
                    choice->setChoice( TechnicalChoice::Choice::tap );
                    isSuccess &= choice->getTap()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "heel" )
                {
                    choice->setChoice( TechnicalChoice::Choice::heel );
                    isSuccess &= choice->getHeel()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "toe" )
                {
                    choice->setChoice( TechnicalChoice::Choice::toe );
                    isSuccess &= choice->getToe()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "fingernails" )
                {
                    choice->setChoice( TechnicalChoice::Choice::fingernails );
                    isSuccess &= choice->getFingernails()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "hole" )
                {
                    choice->setChoice( TechnicalChoice::Choice::hole );
                    isSuccess &= choice->getHole()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "arrow" )
                {
                    choice->setChoice( TechnicalChoice::Choice::arrow );
                    isSuccess &= choice->getArrow()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "handbell" )
                {
                    choice->setChoice( TechnicalChoice::Choice::handbell );
                    isSuccess &= choice->getHandbell()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else if ( elementName == "other-technical" )
                {
                    choice->setChoice( TechnicalChoice::Choice::otherTechnical );
                    isSuccess &= choice->getOtherTechnical()->fromXElement( message, *it );
                    myTechnicalChoiceSet.push_back( choice );
                    continue;
                }
                else
                {
                    message << "Technical: unexpected element '" << elementName << "' encountered" << std::endl;
                    isSuccess = false;
                }

            }

            MX_RETURN_IS_SUCCESS;
        }

    }
}
