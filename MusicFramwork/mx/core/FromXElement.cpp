// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../core/FromXElement.h"
#include "../core/elements/DisplayStep.h"
#include "../core/elements/DisplayStepOctaveGroup.h"
#include "../core/elements/DisplayOctave.h"
#include "../core/elements/EditorialGroup.h"
#include "../core/elements/EditorialVoiceGroup.h"
#include "../core/elements/EditorialVoiceDirectionGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/LayoutGroup.h"
#include "../core/elements/Level.h"
#include "../core/elements/PageLayout.h"
#include "../core/elements/StaffLayout.h"
#include "../core/elements/SystemLayout.h"
#include "../core/elements/Voice.h"

#include "../core/elements/SyllabicTextGroup.h"
#include "../core/elements/Elision.h"
#include "../core/elements/ElisionSyllabicGroup.h"
#include "../core/elements/ElisionSyllabicTextGroup.h"
#include "../core/elements/Extend.h"
#include "../core/elements/Syllabic.h"
#include "../core/elements/Text.h"

#include "../core/elements/BeatUnitGroup.h"
#include "../core/elements/BeatUnit.h"
#include "../core/elements/BeatUnitDot.h"

#include "../core/elements/MetronomeRelationGroup.h"
#include "../core/elements/MetronomeNote.h"
#include "../core/elements/MetronomeRelation.h"

#include "../core/Color.h"
#include "../core/Date.h"
#include "../core/PositiveIntegerOrEmpty.h"
#include "../core/YesNoNumber.h"

#include "../core/elements/TimeSignatureGroup.h"
#include "../core/elements/BeatType.h"
#include "../core/elements/Beats.h"
#include "../core/elements/MidiInstrument.h"
#include "../core/elements/Interchangeable.h"
#include "../core/elements/Elevation.h"
#include "../core/elements/MidiBank.h"
#include "../core/elements/MidiChannel.h"
#include "../core/elements/MidiName.h"
#include "../core/elements/MidiProgram.h"
#include "../core/elements/MidiUnpitched.h"
#include "../core/elements/Pan.h"
#include "../core/elements/Volume.h"

#include "../core/elements/PerMinuteOrBeatUnitChoice.h"

namespace mx
{
    namespace core
    {

        bool importElement(
            std::ostream& message,
            xml::XElement& xelement,
            bool& isSuccess,
            ElementInterface& element,
            bool& isMatch )
        {
            if( xelement.getName() != element.getElementName() )
            {
                return false;
            }
            isMatch = true;
            isSuccess &= element.fromXElement( message, xelement );
            return true;
        }

        bool isEndWithDecrementIfNeeded(
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool isDecrementNeeded )
        {
            if( iter == endIter )
            {
                if( isDecrementNeeded )
                {
                    --iter;
                }
                return true;
            }
            return false;
        }

        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            EditorialGroupPtr& group )
        {
            bool doDecrementIter = false;
            if( iter == endIter )
            {
                return;
            }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialGroup::getFootnote, &EditorialGroup::setHasFootnote );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialGroup::getLevel, &EditorialGroup::setHasLevel );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }
        }

        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            EditorialVoiceGroupPtr& group )
        {
            bool doDecrementIter = false;
            if( iter == endIter )
            {
                return;
            }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceGroup::getFootnote, &EditorialVoiceGroup::setHasFootnote );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceGroup::getLevel, &EditorialVoiceGroup::setHasLevel );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceGroup::getVoice, &EditorialVoiceGroup::setHasVoice );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            EditorialVoiceDirectionGroupPtr& group )
        {
            bool doDecrementIter = false;
            if( iter == endIter )
            {
                return;
            }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceDirectionGroup::getFootnote, &EditorialVoiceDirectionGroup::setHasFootnote );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceDirectionGroup::getLevel, &EditorialVoiceDirectionGroup::setHasLevel );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, doDecrementIter, group, &EditorialVoiceDirectionGroup::getVoice, &EditorialVoiceDirectionGroup::setHasVoice );
            if( isEndWithDecrementIfNeeded( iter, endIter, doDecrementIter ) ) { return ; }
        }

        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            LayoutGroupPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                return;
            }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &LayoutGroup::getPageLayout, &LayoutGroup::setHasPageLayout );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &LayoutGroup::getSystemLayout, &LayoutGroup::setHasSystemLayout );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            checkSetGroupMemberSetAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, "staff-layout", group, &LayoutGroup::getStaffLayoutSet, &LayoutGroup::addStaffLayout, &LayoutGroup::removeStaffLayout );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            BeatUnitGroupPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                return;
            }

            bool dummy = false;

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &BeatUnitGroup::getBeatUnit, dummy );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            checkSetGroupMemberSetAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, "beat-unit-dot", group, &BeatUnitGroup::getBeatUnitDotSet, &BeatUnitGroup::addBeatUnitDot, &BeatUnitGroup::removeBeatUnitDot );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            MetronomeRelationGroupPtr& group,
            bool& hasGroup )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                return;
            }

            bool isFound;

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &MetronomeRelationGroup::getMetronomeRelation, isFound );
            hasGroup |= isFound;
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &MetronomeRelationGroup::getMetronomeNote, isFound );
            hasGroup |= isFound;
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            DisplayStepOctaveGroupPtr& group,
            bool& hasGroup )
        {
            hasGroup = false;
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                return;
            }

            if( iter->getName() == group->getDisplayStep()->getElementName() )
            {
                isSuccess &= group->getDisplayStep()->fromXElement( message, *iter );
                isIterIncremented = true;
                hasGroup = true;
                ++iter;
            }
            
            if( ( iter != endIter ) && ( iter->getName() == group->getDisplayOctave()->getElementName() ) )
            {
                isSuccess &= group->getDisplayOctave()->fromXElement( message, *iter );
                isIterIncremented = true;
                hasGroup = true;
                ++iter;
            }
            
            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            SyllabicTextGroupPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                return;
            }

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &SyllabicTextGroup::getSyllabic, &SyllabicTextGroup::setHasSyllabic );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }

            bool hasText = false;
            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &SyllabicTextGroup::getText, hasText );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            
            // Something seems slightly off about this 'if' statement. originally I expected to always
            // find a 'text' element here, but it turns out that 'end-line' and 'end-paragraph'
            // are valid to find here as well.  It seems to be working.
            if( iter->getName() != "end-line" && iter->getName() != "end-paragraph" && !hasText )
            {
                isSuccess = false;
                message << "SyllabicTextGroup: 'text' was not found where expected, '" << iter->getName() << "' was found instead" << std::endl;
            }

            while( iter != endIter && ( iter->getName() == "elision" || iter->getName() == "syllabic" || iter->getName() == "text" ) )
            {
                auto elisionSyllabicTextGroup = makeElisionSyllabicTextGroup();
                group->addElisionSyllabicTextGroup( elisionSyllabicTextGroup );

                if( iter->getName() == "elision" )
                {
                    
                    isSuccess &= elisionSyllabicTextGroup->getElisionSyllabicGroup()->getElision()->fromXElement( message, *iter );
                    elisionSyllabicTextGroup->setHasElisionSyllabicGroup( true );
                    ++iter;
                    isIterIncremented = true;
                }
                if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }

                if( iter->getName() == "syllabic" )
                {
                    
                    isSuccess &= elisionSyllabicTextGroup->getElisionSyllabicGroup()->getSyllabic()->fromXElement( message, *iter );
                    elisionSyllabicTextGroup->setHasElisionSyllabicGroup( true );
                    ++iter;
                    isIterIncremented = true;
                }
                if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }

                if( iter->getName() == "text" )
                {
                    
                    isSuccess &= elisionSyllabicTextGroup->getText()->fromXElement( message, *iter );
                    ++iter;
                    isIterIncremented = true;
                }
                if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }
            } 

            checkSetGroupMemberAndIncrementIter( message, iter, endIter, isSuccess, isIterIncremented, group, &SyllabicTextGroup::getExtend, &SyllabicTextGroup::setHasExtend );
            if( isEndWithDecrementIfNeeded( iter, endIter, isIterIncremented ) ) { return ; }


            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            TimeSignatureGroupPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                isSuccess = false;
                return;
            }

            bool isBeatsFound = false;
            bool isBeatTypeFound = false;

            if( iter->getName() == group->getBeats()->getElementName() )
            {
                isSuccess &= group->getBeats()->fromXElement( message, *iter );
                isIterIncremented = true;
                isBeatsFound = true;
                ++iter;
            }
            
            if( ( iter != endIter ) && ( iter->getName() == group->getBeatType()->getElementName() ) )
            {
                isSuccess &= group->getBeatType()->fromXElement( message, *iter );
                isIterIncremented = true;
                isBeatTypeFound = true;
                ++iter;
            }
            
            if( ( iter != endIter ) && ( iter->getName() == group->getInterchangeable()->getElementName() ) )
            {
                group->setHasInterchangeable( true );
                isSuccess &= group->getInterchangeable()->fromXElement( message, *iter );
                isIterIncremented = true;
                ++iter;
            }
            
            if( !isBeatsFound )
            {
                message << "importGroup: the time-signature group requires a 'beats' element but it was not found" << std::endl;
                isSuccess = false;
            }
            
            if( !isBeatTypeFound )
            {
                message << "importGroup: the time-signature group requires a 'beat-type' element but it was not found" << std::endl;
                isSuccess = false;
            }

            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }


        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            MidiInstrumentPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                isSuccess = false;
                return;
            }

            if( iter->getName() == group->getMidiChannel()->getElementName() )
            {
                isSuccess &= group->getMidiChannel()->fromXElement( message, *iter );
                group->setHasMidiChannel( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getMidiName()->getElementName() )
            {
                isSuccess &= group->getMidiName()->fromXElement( message, *iter );
                group->setHasMidiName( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getMidiBank()->getElementName() )
            {
                isSuccess &= group->getMidiBank()->fromXElement( message, *iter );
                group->setHasMidiBank( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getMidiProgram()->getElementName() )
            {
                isSuccess &= group->getMidiProgram()->fromXElement( message, *iter );
                group->setHasMidiProgram( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getMidiUnpitched()->getElementName() )
            {
                isSuccess &= group->getMidiUnpitched()->fromXElement( message, *iter );
                group->setHasMidiUnpitched( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getVolume()->getElementName() )
            {
                isSuccess &= group->getVolume()->fromXElement( message, *iter );
                group->setHasVolume( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getPan()->getElementName() )
            {
                isSuccess &= group->getPan()->fromXElement( message, *iter );
                group->setHasPan( true );
                isIterIncremented = true;
                ++iter;
            }

            if( iter->getName() == group->getElevation()->getElementName() )
            {
                isSuccess &= group->getElevation()->fromXElement( message, *iter );
                group->setHasElevation( true );
                isIterIncremented = true;
                ++iter;
            }

            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }

/*
        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            CreditWordsGroupPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                isSuccess = false;
                return;
            }

            bool isCreditWordsFound = false;

            if( iter->getName() == group->getCreditWords()->getElementName() )
            {
                isSuccess &= group->getCreditWords()->fromXElement( message, *iter );
                isCreditWordsFound = true;
                isIterIncremented = true;
                ++iter;
            }
        }*/

/*
        void importGroup(
            std::ostream& message,
            xml::XElementIterator& iter,
            xml::XElementIterator& endIter,
            bool& isSuccess,
            BeatUnitPerPtr& group )
        {
            bool isIterIncremented = false;
            if( iter == endIter )
            {
                isSuccess = false;
                return;
            }

            if( iter->getName() == group->getMidiChannel()->getElementName() )
            {
                isSuccess &= group->getMidiChannel()->fromXElement( message, *iter );
                group->setHasMidiChannel( true );
                isIterIncremented = true;
                ++iter;
            }

            if( isIterIncremented )
            {
                --iter;
                return;
            }
        }
*/


        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            XsString& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.setValue( iter->getValue() );
            return true;
        }


        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            Decimal& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            return true;
        }


        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            Integer& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class Color;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            Color& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class Date;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            Date& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class FontSize;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            FontSize& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class NumberOrNormal;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            NumberOrNormal& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class PositiveIntegerOrEmpty;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            PositiveIntegerOrEmpty& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class YesNoNumber;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            YesNoNumber& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            if( iter->getValue() != toString( assignmentTarget ) )
            {
                isSuccess = false;
                message << className << ": attribute '" << attributeName << "' parsing failed - the value '";
                message << iter->getValue() << "' was unrecognized - setting value to '";
                message << toString( assignmentTarget ) << "'";
            }
            return true;
        }


        class CommaSeparatedText;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            CommaSeparatedText& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            return true;
        }


        class DistanceType;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            DistanceType& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.setValue( iter->getValue() );
            return true;
        }


        class LineWidthType;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            LineWidthType& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.setValue( iter->getValue() );
            return true;
        }
        

        class CommaSeparatedListOfPositiveIntegers;

        bool parseAttribute(
            std::ostream& message,
            xml::XAttributeIterator& iter,
            const std::string& className,
            bool& isSuccess,
            CommaSeparatedListOfPositiveIntegers& assignmentTarget,
            bool& hasAttribute,
            const char* const attributeName )
        {
            MX_UNUSED( message );
            MX_UNUSED( className );
            MX_UNUSED( isSuccess );
            if( iter->getName() != attributeName )
            {
                return false;
            }
            hasAttribute = true;
            assignmentTarget.parse( iter->getValue() );
            return true;
        }
    }
}

