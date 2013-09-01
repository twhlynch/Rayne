//
//  RNUITextEditor.cpp
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUITextEditor.h"

namespace RN
{
	namespace UI
	{
		RNDeclareMeta(TextEditor)
		
		TextEditor::TextEditor(TextEditorInterface *interface) :
			_interface(interface)
		{
			_string = new AttributedString(RNCSTR(""));
			_typingAttributes = new Dictionary();
			_selection = Range(0, 0);
			_typesetter = new Typesetter(_string, Frame());
			_isDirty = true;
			_model = nullptr;
			
			SetBackgroundColor(Color::ClearColor());
		}
		
		TextEditor::~TextEditor()
		{
			_string->Release();
			_typingAttributes->Release();
		}
		
		
		
		void TextEditor::SetFrame(const Rect& frame)
		{
			View::SetFrame(frame);
			_typesetter->SetFrame(frame);
			_isDirty = true;
		}
		
		void TextEditor::SetTypingAttributes(Dictionary *attributes)
		{
			_typingAttributes->Release();
			_typingAttributes = attributes->Retain();
		}
		
		void TextEditor::SetSelection(const Range& selection)
		{
			_selection = selection;
			SetTypingAttributes(_string->GetAttributesAtIndex(_selection.origin));
		}
		
		
		
		void TextEditor::InsertString(String *string)
		{
			if(string)
			{
				_string->ReplaceCharacters(string, _selection, _typingAttributes);
				_typesetter->InvalidateStringInRange(_selection);
				
				_selection.origin ++;
				_selection.length = 0;
			}
			else
			{
				_selection.origin --;
				_selection.length = 1;
				
				_string->ReplaceCharacters(string, _selection, _typingAttributes);
				_typesetter->InvalidateStringInRange(_selection);
				
				_selection.length = 0;
			}
			
			_isDirty = true;
			
			if(_interface)
				_interface->SelectionDidChange(this, _selection);
		}
		
		void TextEditor::ProcessEvent(Event *event)
		{
			switch(event->GetType())
			{
				case Event::Type::KeyDown:
				case Event::Type::KeyRepeat:
				{
					switch(event->GetCode())
					{
						case KeyDelete:
							InsertString(nullptr);
							break;
							
						default:
						{
							char character[2];
							
							character[0] = event->GetCharacter();
							character[1] = '\0';
							
							String *string = new String(character);
							InsertString(string);
							string->Release();
							
							break;
						}
					}
					
					break;
				}
					
				default:
					break;
			}
		}
		
		
		
		void TextEditor::Update()
		{
			View::Update();
			
			if(_isDirty)
			{
				if(_model)
				{
					_model->Release();
					_model = nullptr;
				}
				
				_model   = _typesetter->LineModel()->Retain();
				_isDirty = false;
			}
		}
		
		void TextEditor::Draw(Renderer *renderer)
		{
			View::Draw(renderer);
			
			if(_model)
			{
				RenderingObject object;
				PopulateRenderingObject(object);
				
				uint32 count = _model->GetMeshCount(0);
				for(uint32 i=0; i<count; i++)
				{
					object.mesh     = _model->GetMeshAtIndex(0, i);
					object.material = _model->GetMaterialAtIndex(0, i);
					
					renderer->RenderObject(object);
				}
			}
		}
	}
}
