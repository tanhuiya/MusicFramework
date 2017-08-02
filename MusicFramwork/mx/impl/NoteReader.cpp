// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/NoteReader.h"
#include "../core/elements/Stem.h"
#include "../core/elements/Accidental.h"
#include "../core/elements/ActualNotes.h"
#include "../core/elements/Alter.h"
#include "../core/elements/Beam.h"
#include "../core/elements/CueNoteGroup.h"
#include "../core/elements/DisplayOctave.h"
#include "../core/elements/DisplayStep.h"
#include "../core/elements/DisplayStepOctaveGroup.h"
#include "../core/elements/Duration.h"
#include "../core/elements/EditorialVoiceGroup.h"
#include "../core/elements/FullNoteGroup.h"
#include "../core/elements/FullNoteTypeChoice.h"
#include "../core/elements/GraceNoteGroup.h"
#include "../core/elements/NormalNoteGroup.h"
#include "../core/elements/NormalNotes.h"
#include "../core/elements/NormalType.h"
#include "../core/elements/NormalTypeNormalDotGroup.h"
#include "../core/elements/Note.h"
#include "../core/elements/NoteChoice.h"
#include "../core/elements/Octave.h"
#include "../core/elements/Pitch.h"
#include "../core/elements/Rest.h"
#include "../core/elements/Staff.h"
#include "../core/elements/Step.h"
#include "../core/elements/TimeModification.h"
#include "../core/elements/Type.h"
#include "../core/elements/Unpitched.h"
#include "../core/elements/Voice.h"
#include "../core/elements/Tie.h"
#include "../utility/StringToInt.h"

#include <map>

namespace mx
{
    namespace impl
    {
        NoteReader::NoteReader( const core::Note& mxNote )
        : myNote( mxNote )
        , myNoteChoice( *myNote.getNoteChoice() )
        , myFullNoteGroup( findFullNoteGroup( myNoteChoice ) )
        , myIsNormal( false )
        , myIsGrace( false )
        , myIsCue( false )
        , myIsRest( false )
        , myIsChord( false )
        , myIsMeasureRest( false )
        , myIsUnpitched( false )
        , myIsPitch( false )
        , myIsDisplayStepOctaveSpecified( false )
        , myDurationValue( 0.0L )
        , myStep( core::StepEnum::c )
        , myAlter( 0 )
        , myOctave( 4 )
        , myStaffNumber( 0 )
        , myVoiceNumber( 0 )
        , myDurationType( core::NoteTypeValue::maxima )
        , myIsDurationTypeSpecified( false )
        , myNumDots( 0 )
        , myBeams()
        , myTimeModificationActualNotes( -1 )
        , myTimeModificationNormalNotes( -1 )
        , myTimeModificationNormalType( core::NoteTypeValue::maxima )
        , myTimeModificationNormalTypeDots( 0 )
        , myHasAccidental( false )
        , myAccidental( core::AccidentalValue::natural )
        , myIsAccidentalParenthetical( false )
        , myIsAccidentalCautionary{ false }
        , myIsAccidentalEditorial{ false }
        , myIsAccidentalBracketed{ false }
        , myIsTieStart{ false }
        , myIsTieStop{ false }
        {
            setNormalGraceCueItems();
            setRestPitchUnpitchedItems();
            setChord();
            setStaffNumber();
            setVoiceNumber();
            setDurationType();
            setNumDots();
            setBeams();
            setTimeModification();
            setAccidental();
            setStem();
        }

        const core::FullNoteGroup& NoteReader::findFullNoteGroup( const core::NoteChoice& noteChoice ) const
        {
            switch ( noteChoice.getChoice() )
            {
                case core::NoteChoice::Choice::normal:
                {
                    const auto& noteGuts = *noteChoice.getNormalNoteGroup();
                    return *noteGuts.getFullNoteGroup();
                }
                case core::NoteChoice::Choice::grace:
                {
                    const auto& noteGuts = *noteChoice.getGraceNoteGroup();
                    return *noteGuts.getFullNoteGroup();
                }
                case core::NoteChoice::Choice::cue:
                {
                    const auto& noteGuts = *noteChoice.getCueNoteGroup();
                    return *noteGuts.getFullNoteGroup();
                }
                default:
                    break;
            }

            MX_BUG;
        }


        void NoteReader::setNormalGraceCueItems()
        {
            switch ( myNoteChoice.getChoice() )
            {
                case core::NoteChoice::Choice::normal:
                {
                    myIsNormal = true;
                    const auto& noteGuts = *myNoteChoice.getNormalNoteGroup();
                    myDurationValue = noteGuts.getDuration()->getValue().getValue();
                    setTie( noteGuts.getTieSet() );
                    break;
                }
                case core::NoteChoice::Choice::grace:
                {
                    myIsGrace = true;
                    const auto& noteGuts = *myNoteChoice.getGraceNoteGroup();
                    myDurationValue = 0;
                    setTie( noteGuts.getTieSet() );
                    break;
                }
                case core::NoteChoice::Choice::cue:
                {
                    myIsCue = true;
                    const auto& noteGuts = *myNoteChoice.getCueNoteGroup();
                    myDurationValue = noteGuts.getDuration()->getValue().getValue();
                    break;
                }
                default:
                    MX_THROW( "invalid NoteChoice::Choice value" );
            }
        }


        void NoteReader::setRestPitchUnpitchedItems()
        {
            const auto& fullNoteTypeChoice = *myFullNoteGroup.getFullNoteTypeChoice();
            
            switch ( fullNoteTypeChoice.getChoice() )
            {
                case core::FullNoteTypeChoice::Choice::rest:
                {
                    myIsRest = true;
                    const auto& rest = *fullNoteTypeChoice.getRest();
                    
                    if( rest.getAttributes()->hasMeasure && ( rest.getAttributes()->measure == core::YesNo::yes ) )
                    {
                        myIsMeasureRest = true;
                        
                    }
                    
                    const auto& stepOctave = *rest.getDisplayStepOctaveGroup();
                    
                    if( rest.getHasDisplayStepOctaveGroup() )
                    {
                        myIsDisplayStepOctaveSpecified = true;
                        myStep = stepOctave.getDisplayStep()->getValue();
                        myOctave = stepOctave.getDisplayOctave()->getValue().getValue();
                    }

                    myAlter = 0;
                    break;
                }
                case core::FullNoteTypeChoice::Choice::unpitched:
                {
                    myIsUnpitched = true;
                    const auto& unpitched = *fullNoteTypeChoice.getUnpitched();
                    const auto& stepOctave = *unpitched.getDisplayStepOctaveGroup();
                    
                    if( unpitched.getHasDisplayStepOctaveGroup() )
                    {
                        myIsDisplayStepOctaveSpecified = true;
                        myStep = stepOctave.getDisplayStep()->getValue();
                        myOctave = stepOctave.getDisplayOctave()->getValue().getValue();
                    }
                    
                    myAlter = 0;
                    break;
                }
                case core::FullNoteTypeChoice::Choice::pitch:
                {
                    myIsPitch = true;
                    const auto& pitch = *fullNoteTypeChoice.getPitch();
                    myStep = pitch.getStep()->getValue();
                    myOctave = pitch.getOctave()->getValue().getValue();
                    myAlter = static_cast<int>( std::ceil( pitch.getAlter()->getValue().getValue() - 0.5 ) );
                    break;
                }
                    
                default:
                    MX_THROW( "invalid FullNoteTypeChoice::Choice value" );
            }
        }


        void NoteReader::setChord()
        {
            myIsChord = myFullNoteGroup.getHasChord();
        }
        
        
        void NoteReader::setStaffNumber()
        {
            if( myNote.getHasStaff() )
            {
                const auto& staffObject = *myNote.getStaff();
                myStaffNumber = static_cast<int>( staffObject.getValue().getValue() );
            }
        }


        void NoteReader::setVoiceNumber()
        {
            myVoiceNumber = -1;
            
            if( !myNote.getEditorialVoiceGroup()->getHasVoice() )
            {
                return;
            }
            
            utility::stringToInt( myNote.getEditorialVoiceGroup()->getVoice()->getValue().getValue().c_str(), myVoiceNumber );
        }


        void NoteReader::setDurationType()
        {
            if( myNote.getHasType() )
            {
                myIsDurationTypeSpecified = true;
                myDurationType = myNote.getType()->getValue();
            }
        }


        void NoteReader::setNumDots()
        {
            myNumDots = static_cast<int>( myNote.getDotSet().size() );
        }
        
        
        void NoteReader::setBeams()
        {
            const auto& mxBeamSet = myNote.getBeamSet();
            std::map<int,core::BeamValue> mapOfBeams;
            int calculatedBeamIndex = 0;
            
            for( const auto& mxBeam : mxBeamSet )
            {
                const int userBeamIndex = ( mxBeam->getAttributes()->number.getValue() );
                int useThisBeamIndex = calculatedBeamIndex;
                
                // if the userBeamIndex seems valid we will use it
                // by doing this we support the possibility that
                // the musicxml file could have beams listed in the
                // 'wrong' order, i.e. 3,4,1,2
                if( mxBeam->getAttributes()->hasNumber )
                {
                    if( mapOfBeams.find( userBeamIndex ) == mapOfBeams.cend() )
                    {
                        useThisBeamIndex = userBeamIndex;
                    }
                }
                
                mapOfBeams[useThisBeamIndex] = mxBeam->getValue();
                ++calculatedBeamIndex;
            }
            
            // if the musicxml document has not provided all of the needed
            // beams then this will not be using the docs illogical numbers
            for( const auto& beamVal : mapOfBeams )
            {
                myBeams.push_back( beamVal.second );
            }
        }
        
        
        void NoteReader::setTimeModification()
        {
            if( ! myNote.getHasTimeModification() )
            {
                myTimeModificationActualNotes = 1;
                myTimeModificationNormalNotes = 1;
            }
            
            const auto& mxTimeMod = *myNote.getTimeModification();
            myTimeModificationActualNotes = mxTimeMod.getActualNotes()->getValue().getValue();
            myTimeModificationNormalNotes = mxTimeMod.getNormalNotes()->getValue().getValue();
            
            if( mxTimeMod.getHasNormalTypeNormalDotGroup() )
            {
                const auto& grp = *mxTimeMod.getNormalTypeNormalDotGroup();
                myTimeModificationNormalType = grp.getNormalType()->getValue();
                myTimeModificationNormalTypeDots = static_cast<int>( grp.getNormalDotSet().size() );
            }
            else
            {
                myTimeModificationNormalType = myDurationType;
                myTimeModificationNormalTypeDots = 0;
            }
        }
        
        
        void NoteReader::setAccidental()
        {
            myIsAccidentalParenthetical = false;
            myHasAccidental = myNote.getHasAccidental();

            if( myHasAccidental )
            {
                myAccidental = myNote.getAccidental()->getValue();
                
                if(    myNote.getAccidental()->getAttributes()->hasParentheses
                    && myNote.getAccidental()->getAttributes()->parentheses == core::YesNo::yes )
                {
                    myIsAccidentalParenthetical = true;
                }
                
                if(    myNote.getAccidental()->getAttributes()->hasCautionary
                    && myNote.getAccidental()->getAttributes()->cautionary == core::YesNo::yes )
                {
                    myIsAccidentalCautionary = true;
                }
                
                if(    myNote.getAccidental()->getAttributes()->hasEditorial
                    && myNote.getAccidental()->getAttributes()->editorial == core::YesNo::yes )
                {
                    myIsAccidentalEditorial = true;
                }
                
                if(    myNote.getAccidental()->getAttributes()->hasBracket
                    && myNote.getAccidental()->getAttributes()->bracket == core::YesNo::yes )
                {
                    myIsAccidentalBracketed = true;
                }
            }
        }
        
        
        void NoteReader::setStem()
        {
            if( myNote.getHasStem() )
            {
                myIsStemSpecified = true;
                myStem = myNote.getStem()->getValue();
            }
            else
            {
                myIsStemSpecified = false;
            }
        }
        
        
        void NoteReader::setTie( const core::TieSet& tieSet )
        {
            for( const auto& tie : tieSet )
            {
                if( tie->getAttributes()->type == core::StartStop::start )
                {
                    myIsTieStart = true;
                }
                else if( tie->getAttributes()->type == core::StartStop::stop )
                {
                    myIsTieStop = true;
                }
            }
        }
    }
}
