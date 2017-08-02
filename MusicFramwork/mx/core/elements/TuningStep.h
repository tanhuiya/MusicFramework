// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../../core/ForwardDeclare.h"
#include "../../core/ElementInterface.h"
#include "../../core/Enums.h"

#include <iosfwd>
#include <memory>
#include <vector>

namespace mx
{
    namespace core
    {

        MX_FORWARD_DECLARE_ELEMENT( TuningStep )

        inline TuningStepPtr makeTuningStep() { return std::make_shared<TuningStep>(); }
		inline TuningStepPtr makeTuningStep( const StepEnum& value ) { return std::make_shared<TuningStep>( value ); }
		inline TuningStepPtr makeTuningStep( StepEnum&& value ) { return std::make_shared<TuningStep>( std::move( value ) ); }

        class TuningStep : public ElementInterface
        {
        public:
            TuningStep();
            TuningStep( const StepEnum& value );

            virtual bool hasAttributes() const;
            virtual bool hasContents() const;
            virtual std::ostream& streamAttributes( std::ostream& os ) const;
            virtual std::ostream& streamName( std::ostream& os ) const;
            virtual std::ostream& streamContents( std::ostream& os, const int indentLevel, bool& isOneLineOnly ) const;
            StepEnum getValue() const;
            void setValue( const StepEnum& value );

            bool fromXElement( std::ostream& message, xml::XElement& xelement );

        private:
            StepEnum myValue;
        };
    }
}
