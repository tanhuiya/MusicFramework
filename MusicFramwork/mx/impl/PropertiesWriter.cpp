// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#include "../impl/PropertiesWriter.h"
#include "../core/elements/Properties.h"
#include "../core/elements/Clef.h"
#include "../core/elements/Directive.h"
#include "../core/elements/Divisions.h"
#include "../core/elements/EditorialGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/Instruments.h"
#include "../core/elements/Key.h"
#include "../core/elements/Level.h"
#include "../core/elements/MeasureStyle.h"
#include "../core/elements/PartSymbol.h"
#include "../core/elements/StaffDetails.h"
#include "../core/elements/Staves.h"
#include "../core/elements/Time.h"
#include "../core/elements/Transpose.h"
#include "../core/elements/PartwiseMeasure.h"
#include "../core/elements/MusicDataChoice.h"
#include "../core/elements/MusicDataGroup.h"
#include "../core/elements/PartwiseMeasure.h"
#include "../impl/Converter.h"
#include "../core/elements/Backup.h"
#include "../core/elements/Forward.h"
#include "../core/elements/MusicDataGroup.h"
#include "../core/elements/MusicDataChoice.h"
#include "../core/elements/Properties.h"
#include "../core/elements/Clef.h"
#include "../core/elements/Directive.h"
#include "../core/elements/Divisions.h"
#include "../core/elements/EditorialGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/Instruments.h"
#include "../core/elements/Key.h"
#include "../core/elements/KeyChoice.h"
#include "../core/elements/TraditionalKey.h"
#include "../core/elements/Cancel.h"
#include "../core/elements/Fifths.h"
#include "../core/elements/Mode.h"
#include "../core/elements/Level.h"
#include "../core/elements/MeasureStyle.h"
#include "../core/elements/PartSymbol.h"
#include "../core/elements/StaffDetails.h"
#include "../core/elements/Staves.h"
#include "../core/elements/Time.h"
#include "../core/elements/Transpose.h"
#include "../core/elements/TimeChoice.h"
#include "../core/elements/TimeSignatureGroup.h"
#include "../core/elements/BeatType.h"
#include "../core/elements/Beats.h"
#include "../core/elements/Clef.h"
#include "../core/elements/ClefOctaveChange.h"
#include "../core/elements/Line.h"
#include "../core/elements/Sign.h"
#include "../impl/ScoreWriter.h"
#include "../core/elements/Print.h"
#include "../core/elements/LayoutGroup.h"
#include "../core/elements/MeasureLayout.h"
#include "../core/elements/MeasureNumbering.h"
#include "../core/elements/PartAbbreviationDisplay.h"
#include "../core/elements/PartNameDisplay.h"
#include "../core/elements/SystemLayout.h"
#include "../core/elements/SystemDistance.h"
#include "../core/elements/SystemDividers.h"
#include "../core/elements/SystemMargins.h"
#include "../core/elements/TopSystemDistance.h"
#include "../core/elements/LeftMargin.h"
#include "../core/elements/RightMargin.h"
#include "../core/elements/Note.h"
#include "../core/elements/Accidental.h"
#include "../core/elements/Beam.h"
#include "../core/elements/CueNoteGroup.h"
#include "../core/elements/Dot.h"
#include "../core/elements/Duration.h"
#include "../core/elements/EditorialVoiceGroup.h"
#include "../core/elements/Footnote.h"
#include "../core/elements/FullNoteGroup.h"
#include "../core/elements/FullNoteTypeChoice.h"
#include "../core/elements/GraceNoteGroup.h"
#include "../core/elements/Grace.h"
#include "../core/elements/Cue.h"
#include "../core/elements/Instrument.h"
#include "../core/elements/Level.h"
#include "../core/elements/Lyric.h"
#include "../core/elements/Notations.h"
#include "../core/elements/NoteChoice.h"
#include "../core/elements/Notehead.h"
#include "../core/elements/NoteheadText.h"
#include "../core/elements/NormalNoteGroup.h"
#include "../core/elements/Pitch.h"
#include "../core/elements/Play.h"
#include "../core/elements/Staff.h"
#include "../core/elements/Stem.h"
#include "../core/elements/Rest.h"
#include "../core/elements/Tie.h"
#include "../core/elements/TimeModification.h"
#include "../core/elements/Type.h"
#include "../core/elements/Unpitched.h"
#include "../core/elements/Voice.h"
#include "../impl/NoteWriter.h"

namespace mx
{
	namespace impl
	{
        PropertiesWriter::PropertiesWriter( const core::PartwiseMeasurePtr& inPartwiseMeasure )
        : myProperties{ nullptr }
        , myPartwiseMeasure{ inPartwiseMeasure }
        {
            allocate();
            MX_ASSERT( myPartwiseMeasure != nullptr );
        }

        
        void PropertiesWriter::clearBuffer()
        {
            allocate();
        }
        
        
        void PropertiesWriter::flushBuffer()
        {
            if( !isPropertiesEmpty() )
            {
                auto mdc = core::makeMusicDataChoice();
                mdc->setChoice( core::MusicDataChoice::Choice::properties );
                mdc->setProperties( myProperties );
                myPartwiseMeasure->getMusicDataGroup()->addMusicDataChoice( mdc );
                allocate();
            }
        }
        
        
        bool PropertiesWriter::isPropertiesEmpty()
        {
            if( !myProperties )
            {
                allocate();
                return false;
            }
            const bool hasContents = myProperties->hasContents();
            return !hasContents;
        }
        
        
        void PropertiesWriter::writeDivisions( int value )
        {
            myProperties->setHasDivisions( true );
            myProperties->getDivisions()->setValue( core::PositiveDecimal{ static_cast<long double>( value ) } );
        }
    
        
        void PropertiesWriter::writeKey( int staffIndex, const api::KeyData& inKeyData )
        {
            // TODO - support non-traditional keys
            // TODO - support placement and other attributes
            auto key = core::makeKey();
            
            if( staffIndex >= 0 )
            {
                key->getAttributes()->hasNumber = true;
                key->getAttributes()->number = core::StaffNumber{ staffIndex + 1 };
            }
            
            key->getKeyChoice()->setChoice( core::KeyChoice::Choice::traditionalKey );
            auto traditionalKey = key->getKeyChoice()->getTraditionalKey();
            traditionalKey->getFifths()->setValue( core::FifthsValue{ inKeyData.fifths } );
            
            if ( inKeyData.cancel != 0 )
            {
                traditionalKey->setHasCancel( true );
                traditionalKey->getCancel()->setValue( core::FifthsValue{ inKeyData.cancel } );
            }
            
            if( inKeyData.mode == api::KeyMode::major || inKeyData.mode == api::KeyMode::minor )
            {
                traditionalKey->setHasMode( true );
                traditionalKey->getMode()->setValue( core::ModeValue{ inKeyData.mode == api::KeyMode::major ? core::ModeEnum::major : core::ModeEnum::minor } );
            }
            myProperties->addKey( key );
        }
        
        
        void PropertiesWriter::writeTime( const api::TimeSignatureData& value )
        {
            auto time = core::makeTime();
            myProperties->addTime( time );
            time->getTimeChoice()->setChoice( core::TimeChoice::Choice::timeSignature );
            auto sigGrp = time->getTimeChoice()->getTimeSignatureGroupSet().front();
            sigGrp->getBeats()->setValue( core::XsString{ std::to_string( value.beats ) } );
            sigGrp->getBeatType()->setValue( core::XsString{ std::to_string( value.beatType ) } );
            
            const auto symbol = value.symbol;
            if( symbol != api::TimeSignatureSymbol::unspecified )
            {
                time->getAttributes()->hasSymbol = true;
                if( symbol == api::TimeSignatureSymbol::common )
                {
                    time->getAttributes()->symbol = core::TimeSymbol::common;
                }
                else if( symbol == api::TimeSignatureSymbol::cut )
                {
                    time->getAttributes()->symbol = core::TimeSymbol::cut;
                }
            }
            
            Converter converter;
            if( value.display != api::Bool::unspecified )
            {
                time->getAttributes()->hasPrintObject = true;
                time->getAttributes()->printObject = converter.convert( value.display );
            }
        }
        
        
        void PropertiesWriter::writeNumStaves( int value )
        {
            myProperties->setHasStaves( true );
            myProperties->getStaves()->setValue( core::NonNegativeInteger{ value } );
        }
        
        
        void PropertiesWriter::writeClef( int staffIndex, const api::ClefData& inClefData )
        {
            auto mxClef = core::makeClef();
            
            if( staffIndex >= 0 )
            {
                mxClef->getAttributes()->hasNumber = true;
                mxClef->getAttributes()->number = core::StaffNumber{ staffIndex + 1 };
            }
            
            Converter converter;
            mxClef->getSign()->setValue( converter.convert( inClefData.symbol ) );
            
            if( inClefData.line >= 0 )
            {
                mxClef->setHasLine( true );
                mxClef->getLine()->setValue( core::StaffLine{ inClefData.line } );
            }
            
            if( inClefData.octaveChange != 0 )
            {
                mxClef->setHasClefOctaveChange( true );
                mxClef->getClefOctaveChange()->setValue( core::Integer{ inClefData.octaveChange } );
            }
            myProperties->addClef( mxClef );
        }
        
        
        void PropertiesWriter::allocate()
        {
            myProperties = core::makeProperties();
        }
	}
}
