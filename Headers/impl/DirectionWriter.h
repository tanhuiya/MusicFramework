// MusicXML Class Library
// Copyright (c) by Matthew James Briggs
// Distributed under the MIT License

#pragma once

#include "../api/ApiCommon.h"
#include "../impl/Converter.h"
#include "../impl/Cursor.h"

#include <set>

namespace mx
{
	namespace core
	{
        class PartwiseMeasure;
        using PartwiseMeasurePtr = std::shared_ptr<PartwiseMeasure>;
        class Direction;
        using DirectionPtr = std::shared_ptr<Direction>;
        class DirectionType;
        using DirectionTypePtr = std::shared_ptr<DirectionType>;
	}

    namespace impl
    {
        class DirectionWriter
        {
        public:
            DirectionWriter( const api::DirectionData& inDirectionData, const Cursor& inCursor );
            core::DirectionPtr getDirection();

        private:
            void addDirectionType( const core::DirectionTypePtr& directionType );

        private:
            const api::DirectionData& myDirectionData;
            const Cursor myCursor;
            core::DirectionPtr myOutDirectionPtr;
            const Converter myConverter;
            bool myIsFirstDirectionTypeAdded;
            std::set<api::Placement> myPlacements;
        };
    }
}
