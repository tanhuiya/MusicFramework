// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../../core/elements/NotationsChoice.h"
#include "../../core/FromXElement.h"
#include "../../core/elements/AccidentalMark.h"
#include "../../core/elements/Arpeggiate.h"
#include "../../core/elements/Articulations.h"
#include "../../core/elements/Dynamics.h"
#include "../../core/elements/Fermata.h"
#include "../../core/elements/Glissando.h"
#include "../../core/elements/NonArpeggiate.h"
#include "../../core/elements/Ornaments.h"
#include "../../core/elements/OtherNotation.h"
#include "../../core/elements/Slide.h"
#include "../../core/elements/Slur.h"
#include "../../core/elements/Technical.h"
#include "../../core/elements/Tied.h"
#include "../../core/elements/Tuplet.h"
#include <iostream>

namespace mx
{
    namespace core
    {
        NotationsChoice::NotationsChoice()
        :myChoice( Choice::tied )
        ,myTied( makeTied() )
        ,mySlur( makeSlur() )
        ,myTuplet( makeTuplet() )
        ,myGlissando( makeGlissando() )
        ,mySlide( makeSlide() )
        ,myOrnaments( makeOrnaments() )
        ,myTechnical( makeTechnical() )
        ,myArticulations( makeArticulations() )
        ,myDynamics( makeDynamics() )
        ,myFermata( makeFermata() )
        ,myArpeggiate( makeArpeggiate() )
        ,myNonArpeggiate( makeNonArpeggiate() )
        ,myAccidentalMark( makeAccidentalMark() )
        ,myOtherNotation( makeOtherNotation() )
        {}


        bool NotationsChoice::hasAttributes() const
        {
            return false;
        }


        std::ostream& NotationsChoice::streamAttributes( std::ostream& os ) const
        {
            return os;
        }


        std::ostream& NotationsChoice::streamName( std::ostream& os ) const
        {
            return os;
        }


        bool NotationsChoice::hasContents() const
        {
            return true;
        }


        std::ostream& NotationsChoice::streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const
        {
            switch ( myChoice )
            {
                case Choice::tied:
                    myTied->toStream( os, indentLevel );
                    break;
                case Choice::slur:
                    mySlur->toStream( os, indentLevel );
                    break;
                case Choice::tuplet:
                    myTuplet->toStream( os, indentLevel );
                    break;
                case Choice::glissando:
                    myGlissando->toStream( os, indentLevel );
                    break;
                case Choice::slide:
                    mySlide->toStream( os, indentLevel );
                    break;
                case Choice::ornaments:
                    myOrnaments->toStream( os, indentLevel );
                    break;
                case Choice::technical:
                    myTechnical->toStream( os, indentLevel );
                    break;
                case Choice::articulations:
                    myArticulations->toStream( os, indentLevel );
                    break;
                case Choice::dynamics:
                    myDynamics->toStream( os, indentLevel );
                    break;
                case Choice::fermata:
                    myFermata->toStream( os, indentLevel );
                    break;
                case Choice::arpeggiate:
                    myArpeggiate->toStream( os, indentLevel );
                    break;
                case Choice::nonArpeggiate:
                    myNonArpeggiate->toStream( os, indentLevel );
                    break;
                case Choice::accidentalMark:
                    myAccidentalMark->toStream( os, indentLevel );
                    break;
                case Choice::otherNotation:
                    myOtherNotation->toStream( os, indentLevel );
                    break;
                default:
                    break;
            }
            isOneLineOnly = hasContents();
            return os;
        }


        NotationsChoice::Choice NotationsChoice::getChoice() const
        {
            return myChoice;
        }


        void NotationsChoice::setChoice( const NotationsChoice::Choice value )
        {
            myChoice = value;
        }


        TiedPtr NotationsChoice::getTied() const
        {
            return myTied;
        }


        void NotationsChoice::setTied( const TiedPtr& value )
        {
            if( value )
            {
                myTied = value;
            }
        }


        SlurPtr NotationsChoice::getSlur() const
        {
            return mySlur;
        }


        void NotationsChoice::setSlur( const SlurPtr& value )
        {
            if( value )
            {
                mySlur = value;
            }
        }


        TupletPtr NotationsChoice::getTuplet() const
        {
            return myTuplet;
        }


        void NotationsChoice::setTuplet( const TupletPtr& value )
        {
            if( value )
            {
                myTuplet = value;
            }
        }


        GlissandoPtr NotationsChoice::getGlissando() const
        {
            return myGlissando;
        }


        void NotationsChoice::setGlissando( const GlissandoPtr& value )
        {
            if( value )
            {
                myGlissando = value;
            }
        }


        SlidePtr NotationsChoice::getSlide() const
        {
            return mySlide;
        }


        void NotationsChoice::setSlide( const SlidePtr& value )
        {
            if( value )
            {
                mySlide = value;
            }
        }


        OrnamentsPtr NotationsChoice::getOrnaments() const
        {
            return myOrnaments;
        }


        void NotationsChoice::setOrnaments( const OrnamentsPtr& value )
        {
            if( value )
            {
                myOrnaments = value;
            }
        }


        TechnicalPtr NotationsChoice::getTechnical() const
        {
            return myTechnical;
        }


        void NotationsChoice::setTechnical( const TechnicalPtr& value )
        {
            if( value )
            {
                myTechnical = value;
            }
        }


        ArticulationsPtr NotationsChoice::getArticulations() const
        {
            return myArticulations;
        }


        void NotationsChoice::setArticulations( const ArticulationsPtr& value )
        {
            if( value )
            {
                myArticulations = value;
            }
        }


        DynamicsPtr NotationsChoice::getDynamics() const
        {
            return myDynamics;
        }


        void NotationsChoice::setDynamics( const DynamicsPtr& value )
        {
            if( value )
            {
                myDynamics = value;
            }
        }


        FermataPtr NotationsChoice::getFermata() const
        {
            return myFermata;
        }


        void NotationsChoice::setFermata( const FermataPtr& value )
        {
            if( value )
            {
                myFermata = value;
            }
        }


        ArpeggiatePtr NotationsChoice::getArpeggiate() const
        {
            return myArpeggiate;
        }


        void NotationsChoice::setArpeggiate( const ArpeggiatePtr& value )
        {
            if( value )
            {
                myArpeggiate = value;
            }
        }


        NonArpeggiatePtr NotationsChoice::getNonArpeggiate() const
        {
            return myNonArpeggiate;
        }


        void NotationsChoice::setNonArpeggiate( const NonArpeggiatePtr& value )
        {
            if( value )
            {
                myNonArpeggiate = value;
            }
        }


        AccidentalMarkPtr NotationsChoice::getAccidentalMark() const
        {
            return myAccidentalMark;
        }


        void NotationsChoice::setAccidentalMark( const AccidentalMarkPtr& value )
        {
            if( value )
            {
                myAccidentalMark = value;
            }
        }


        OtherNotationPtr NotationsChoice::getOtherNotation() const
        {
            return myOtherNotation;
        }


        void NotationsChoice::setOtherNotation( const OtherNotationPtr& value )
        {
            if( value )
            {
                myOtherNotation = value;
            }
        }
        
        
        MX_FROM_XELEMENT_UNUSED( NotationsChoice );

    }
}
