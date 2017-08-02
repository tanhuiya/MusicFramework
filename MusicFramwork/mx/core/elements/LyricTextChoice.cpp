// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../../core/elements/LyricTextChoice.h"
#include "../../core/FromXElement.h"
#include "../../core/elements/Extend.h"
#include "../../core/elements/Humming.h"
#include "../../core/elements/Laughing.h"
#include "../../core/elements/SyllabicTextGroup.h"
#include <iostream>

namespace mx
{
    namespace core
    {
        LyricTextChoice::LyricTextChoice()
        :myChoice( Choice::syllabicTextGroup )
        ,mySyllabicTextGroup( makeSyllabicTextGroup() )
        ,myExtend( makeExtend() )
        ,myLaughing( makeLaughing() )
        ,myHumming( makeHumming() )
        {}


        bool LyricTextChoice::hasAttributes() const
        {
            return false;
        }


        std::ostream& LyricTextChoice::streamAttributes( std::ostream& os ) const
        {
            return os;
        }


        std::ostream& LyricTextChoice::streamName( std::ostream& os ) const
        {
            return os;
        }


        bool LyricTextChoice::hasContents() const
        {
            return myChoice == Choice::syllabicTextGroup;
        }


        std::ostream& LyricTextChoice::streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const
        {
            switch ( myChoice )
            {
                case Choice::syllabicTextGroup:
                {
                    mySyllabicTextGroup->streamContents( os, indentLevel, isOneLineOnly );
                }
                    break;
                case Choice::extend:
                {
                    myExtend->toStream( os, indentLevel );
                }
                    break;
                case Choice::laughing:
                {
                    myLaughing->toStream( os, indentLevel );
                }
                    break;
                case Choice::humming:
                {
                    myHumming->toStream( os, indentLevel );
                }
                    break;
                default:
                    break;
            }
            return os;
        }


        LyricTextChoice::Choice LyricTextChoice::getChoice() const
        {
            return myChoice;
        }


        void LyricTextChoice::setChoice( const LyricTextChoice::Choice value )
        {
            myChoice = value;
        }


        SyllabicTextGroupPtr LyricTextChoice::getSyllabicTextGroup() const
        {
            return mySyllabicTextGroup;
        }


        void LyricTextChoice::setSyllabicTextGroup( const SyllabicTextGroupPtr& value )
        {
            if ( value )
            {
                mySyllabicTextGroup = value;
            }
        }


        ExtendPtr LyricTextChoice::getExtend() const
        {
            return myExtend;
        }


        void LyricTextChoice::setExtend( const ExtendPtr& value )
        {
            if ( value )
            {
                myExtend    = value;
            }
        }


        LaughingPtr LyricTextChoice::getLaughing() const
        {
            return myLaughing;
        }


        void LyricTextChoice::setLaughing( const LaughingPtr& value )
        {
            if ( value )
            {
                myLaughing = value;
            }
        }


        HummingPtr LyricTextChoice::getHumming() const
        {
            return myHumming;
        }


        void LyricTextChoice::setHumming( const HummingPtr& value )
        {
            if ( value )
            {
                myHumming = value;
            }
        }


        MX_FROM_XELEMENT_UNUSED( LyricTextChoice );

    }
}
