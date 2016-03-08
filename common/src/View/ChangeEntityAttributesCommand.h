/*
 Copyright (C) 2010-2014 Kristian Duske
 
 This file is part of TrenchBroom.
 
 TrenchBroom is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 TrenchBroom is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with TrenchBroom. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TrenchBroom__ChangeEntityAttributesCommand__
#define __TrenchBroom__ChangeEntityAttributesCommand__

#include "Model/EntityAttributeSnapshot.h"
#include "Model/ModelTypes.h"
#include "View/DocumentCommand.h"

namespace TrenchBroom {
    namespace View {
        class MapDocumentCommandFacade;
        
        class ChangeEntityAttributesCommand : public DocumentCommand {
        public:
            static const CommandType Type;
        private:
            typedef enum {
                Action_Set,
                Action_Remove,
                Action_Rename
            } Action;
            
            Action m_action;
            Model::AttributeName m_oldName;
            Model::AttributeName m_newName;
            Model::AttributeValue m_newValue;
            
            Model::EntityAttributeSnapshot::Map m_snapshots;
        public:
            static ChangeEntityAttributesCommand* set(const Model::AttributeName& name, const Model::AttributeValue& value);
            static ChangeEntityAttributesCommand* remove(const Model::AttributeName& name);
            static ChangeEntityAttributesCommand* rename(const Model::AttributeName& oldName, const Model::AttributeName& newName);
        protected:
            void setName(const Model::AttributeName& name);
            void setNewName(const Model::AttributeName& newName);
            void setNewValue(const Model::AttributeValue& newValue);
        private:
            ChangeEntityAttributesCommand(Action action);
            static String makeName(Action action);

            bool doPerformDo(MapDocumentCommandFacade* document);
            bool doPerformUndo(MapDocumentCommandFacade* document);

            bool doIsRepeatable(MapDocumentCommandFacade* document) const;
            
            bool doCollateWith(UndoableCommand* command);
        };
    }
}

#endif /* defined(__TrenchBroom__ChangeEntityAttributesCommand__) */