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

        MX_FORWARD_DECLARE_ELEMENT( PedalStep )

        inline PedalStepPtr makePedalStep() { return std::make_shared<PedalStep>(); }
		inline PedalStepPtr makePedalStep( const StepEnum& value ) { return std::make_shared<PedalStep>( value ); }
		inline PedalStepPtr makePedalStep( StepEnum&& value ) { return std::make_shared<PedalStep>( std::move( value ) ); }

        class PedalStep : public ElementInterface
        {
        public:
            PedalStep();
            PedalStep( const StepEnum& value );

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
