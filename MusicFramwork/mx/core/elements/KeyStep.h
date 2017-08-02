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

        MX_FORWARD_DECLARE_ELEMENT( KeyStep )

        inline KeyStepPtr makeKeyStep() { return std::make_shared<KeyStep>(); }
		inline KeyStepPtr makeKeyStep( const StepEnum& value ) { return std::make_shared<KeyStep>( value ); }
		inline KeyStepPtr makeKeyStep( StepEnum&& value ) { return std::make_shared<KeyStep>( std::move( value ) ); }

        class KeyStep : public ElementInterface
        {
        public:
            KeyStep();
            KeyStep( const StepEnum& value );

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
