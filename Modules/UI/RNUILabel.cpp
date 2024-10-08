//
//  RNUILabel.cpp
//  Rayne
//
//  Copyright 2018 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNUILabel.h"
#include "RNUIWindow.h"
#include "RNUIServer.h"

namespace RN
{
	namespace UI
	{
		RNDefineMeta(Label, View)

		Label::Label(const TextAttributes &defaultAttributes) : _attributedText(nullptr), _defaultAttributes(defaultAttributes), _additionalLineHeight(0.0f), _shadowColor(Color::ClearColor()), _verticalAlignment(TextVerticalAlignmentTop), _labelDepthMode(DepthMode::GreaterOrEqual), _textMaterial(nullptr), _shadowMaterial(nullptr), _cursorView(nullptr), _cursorBlinkTimer(0.0f)
		{
			
		}
		Label::~Label()
		{
			SafeRelease(_textMaterial);
			SafeRelease(_shadowMaterial);
		}
		
		void Label::SetText(const String *text)
		{
			Lock();
			if(_attributedText && text && text->IsEqual(_attributedText) && !_attributedText->GetAttributesAtIndex(0))
			{
				Unlock();
				return;
			}
			if(!text) text = RNCSTR("");
			
			SafeRelease(_attributedText);
			_attributedText = new AttributedString(text);
			_needsMeshUpdate = true;
			Unlock();
		}
	
		void Label::SetAttributedText(AttributedString *text)
		{
			Lock();
			SafeRelease(_attributedText);
			_attributedText = text;
			SafeRetain(_attributedText);
			_needsMeshUpdate = true;
			Unlock();
		}
	
		void Label::SetDefaultAttributes(const TextAttributes &attributes)
		{
			Lock();
			_defaultAttributes = attributes;
			_needsMeshUpdate = true;
			Unlock();
		}
	
		void Label::SetTextColor(const Color &color)
		{
			Lock();
			if(color == _defaultAttributes.GetColor())
			{
				Unlock();
				return;
			}
			
			_defaultAttributes.SetColor(color);
			_needsMeshUpdate = true;
			Unlock();
		}
	
		void Label::SetVerticalAlignment(TextVerticalAlignment alignment)
		{
			Lock();
			_verticalAlignment = alignment;
			_needsMeshUpdate = true;
			Unlock();
		}
    
        void Label::SetAdditionalLineHeight(float lineHeight)
        {
			Lock();
			_additionalLineHeight = lineHeight;
			_needsMeshUpdate = true;
			Unlock();
        }
		
		void Label::SetShadowColor(Color color)
		{
			Lock();
			_shadowColor = color;
			
			if(_shadowMaterial)
			{
				Color finalColor = _shadowColor;
				finalColor.a *= _combinedOpacityFactor;
				_shadowMaterial->SetDiffuseColor(finalColor);
				_shadowMaterial->SetSkipRendering(finalColor.a < k::EpsilonFloat);
			}
			Unlock();
		}
		
		void Label::SetShadowOffset(Vector2 offset)
		{
			Lock();
			_shadowOffset = offset;
			
			if(_shadowMaterial)
			{
				_shadowMaterial->SetUIOffset(Vector2(_shadowOffset.x, -_shadowOffset.y));
			}
			Unlock();
		}
		
		void Label::SetTextDepthMode(DepthMode depthMode)
		{
			Lock();
			_labelDepthMode = depthMode;
			if(_shadowMaterial)
			{
				_shadowMaterial->SetDepthMode(_labelDepthMode);
			}
			
			if(_textMaterial)
			{
				_textMaterial->SetDepthMode(_labelDepthMode);
			}
			Unlock();
		}
	
		void Label::SetTextMaterial(Material *material)
		{
			RN_ASSERT(material, "A valid material is required!");
			
			Lock();
			SafeRelease(_textMaterial);
			_textMaterial = SafeRetain(material);
			
			material->SetAlphaToCoverage(false);
			material->SetDepthWriteEnabled(false);
			material->SetDepthMode(_inheritRenderSettings? _depthMode : _labelDepthMode);
			material->SetCullMode(CullMode::None);
			material->SetBlendOperation(BlendOperation::Add, BlendOperation::Add);
			material->SetBlendFactorSource(BlendFactor::SourceAlpha, BlendFactor::Zero);
			material->SetBlendFactorDestination(BlendFactor::OneMinusSourceAlpha, BlendFactor::One);
			
			Color finalColor = Color::White();
			finalColor.a *= _combinedOpacityFactor;
			material->SetDiffuseColor(finalColor);
			material->SetSkipRendering(finalColor.a < k::EpsilonFloat || !_attributedText || _attributedText->GetLength() == 0);

			const Rect &scissorRect = GetScissorRect();
			material->SetUIClippingRect(Vector4(scissorRect.GetLeft(), scissorRect.GetRight(), scissorRect.GetTop(), scissorRect.GetBottom()));
			material->SetUIOffset(Vector2(0.0f, 0.0f));
			
			RN::Model *model = GetModel();
			if(model && model->GetLODStage(0)->GetCount() > 1)
			{
				model->GetLODStage(0)->ReplaceMaterial(material, 2);
			}
			Unlock();
		}
		
		void Label::SetTextShadowMaterial(Material *material)
		{
			RN_ASSERT(material, "A valid material is required!");
			
			Lock();
			SafeRelease(_shadowMaterial);
			_shadowMaterial = SafeRetain(material);
			
			material->SetAlphaToCoverage(false);
			material->SetDepthWriteEnabled(false);
			material->SetDepthMode(_inheritRenderSettings? _depthMode : _labelDepthMode);
			material->SetCullMode(CullMode::None);
			material->SetBlendOperation(BlendOperation::Add, BlendOperation::Add);
			material->SetBlendFactorSource(BlendFactor::SourceAlpha, BlendFactor::Zero);
			material->SetBlendFactorDestination(BlendFactor::OneMinusSourceAlpha, BlendFactor::One);

			const Rect &scissorRect = GetScissorRect();
			material->SetUIClippingRect(Vector4(scissorRect.GetLeft(), scissorRect.GetRight(), scissorRect.GetTop(), scissorRect.GetBottom()));
			material->SetUIOffset(Vector2(_shadowOffset.x, -_shadowOffset.y));
			
			Color finalColor = _shadowColor;
			finalColor.a *= _combinedOpacityFactor;
			material->SetDiffuseColor(finalColor);
			material->SetSkipRendering(finalColor.a < k::EpsilonFloat || !_attributedText || _attributedText->GetLength() == 0);
			
			RN::Model *model = GetModel();
			if(model && model->GetLODStage(0)->GetCount() > 1)
			{
				model->GetLODStage(0)->ReplaceMaterial(material, 1);
			}
			Unlock();
		}
	
		void Label::SetCursor(bool enabled, size_t position)
		{
			if(enabled && !_cursorView)
			{
				_cursorView = new View();
				AddSubview(_cursorView->Autorelease());
			}
			
			if(!enabled && _cursorView)
			{
				_cursorView->RemoveFromSuperview();
				_cursorView = nullptr;
			}
			
			if(_cursorView)
			{
				_currentCursorPosition = position;
				Vector2 characterPosition = GetCharacterPosition(position);
				
				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(position);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				Font *currentFont = currentAttributes->GetFont();
				
				float cursorWidth = 2.0f/30.0f * currentAttributes->GetFontSize();
				
				float fontSizeFactor = currentAttributes->GetFontSize() / currentFont->GetHeight();
				_cursorView->SetFrame(Rect(characterPosition.x - cursorWidth * 0.5f, characterPosition.y - currentFont->GetAscent() * fontSizeFactor, cursorWidth, currentAttributes->GetFontSize()));
				_cursorView->SetBackgroundColor(_defaultAttributes.GetColor());
			}
		}
	
		void Label::SetCursor(bool enabled, Vector2 position)
		{
			SetCursor(enabled, GetCharacterAtPosition(position + RN::Vector2(0.0f, _defaultAttributes.GetFontSize() * 0.5f)));
		}
	
		Vector2 Label::GetCharacterPosition(size_t charIndex)
		{
			Lock();
			if(!_attributedText || _attributedText->GetLength() == 0)
			{
				Vector2 result;
				if(_defaultAttributes.GetAlignment() == TextAlignmentCenter)
				{
					result.x = GetBounds().width * 0.5f;
				}
				else if(_defaultAttributes.GetAlignment() == TextAlignmentRight)
				{
					result.x = GetBounds().width;
				}
				
				float scaleFactor = _defaultAttributes.GetFontSize() / _defaultAttributes.GetFont()->GetHeight();
				result.y = (_defaultAttributes.GetFont()->GetAscent()) * scaleFactor;
				
				if(_verticalAlignment == TextVerticalAlignmentCenter)
				{
					result.y += GetBounds().height * 0.5f;
					result.y -= _defaultAttributes.GetFontSize() * 0.5f;
					result.y += _defaultAttributes.GetFont()->GetDescent() * 0.5f * scaleFactor;
				}
				else if(_verticalAlignment == TextVerticalAlignmentBottom)
				{
					result.y += GetBounds().height;
					result.y -= _defaultAttributes.GetFontSize();
				}
				
				Unlock();
				return result;
			}
			
			Array *spacings = (new Array(_attributedText->GetLength()))->Autorelease();
			
			std::vector<int64> linebreaks;
			std::vector<float> linewidth;
			std::vector<float> lineascent;
			std::vector<float> linedescent;
			std::vector<float> lineoffset;
			
			float currentWidth = 0.0f;
			float lastWordWidth = 0.0f;
			
			float totalHeight = 0.0f;
			
			float maxAscent = 0.0f;
			float lastWordMaxAscent = 0.0f;
			float tempMaxAscent = 0.0f;
			float maxDescent = 0.0f;
			float lastWordMaxDescent = 0.0f;
			float tempMaxDescent = 0.0f;
			float maxLineOffset = 0.0f;
			float lastWordMaxLineOffset = 0.0f;
			float tempMaxLineOffset = 0.0f;
			
			int characterCounter = 0;
			
			int64 lastWhiteSpaceIndex = -1;
			CharacterSet *whiteSpaces = CharacterSet::WithWhitespaces();
			for(int64 i = 0; i < _attributedText->GetLength(); i++)
			{
				int currentCodepoint = _attributedText->GetCharacterAtIndex(i);
				int nextCodepoint = i < _attributedText->GetLength()-1? _attributedText->GetCharacterAtIndex(i+1) : -1;

				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(i);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				
				Font *currentFont = currentAttributes->GetFont();
				
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				float offset = currentFont->GetOffsetForNextCharacter(currentCodepoint, nextCodepoint) * scaleFactor + currentAttributes->GetKerning();
				
				float characterAscent = currentFont->GetAscent() * scaleFactor;
				float characterDescent = -currentFont->GetDescent() * scaleFactor;
				float characterLineOffset = currentFont->GetLineOffset() * scaleFactor;
				maxAscent = std::max(maxAscent, characterAscent);
				tempMaxAscent = std::max(tempMaxAscent, characterAscent);
				maxDescent = std::max(maxDescent, characterDescent);
				tempMaxDescent = std::max(tempMaxDescent, characterDescent);
				maxLineOffset = std::max(maxLineOffset, characterLineOffset);
				tempMaxLineOffset = std::max(tempMaxLineOffset, characterLineOffset);
				
				if(whiteSpaces->CharacterIsMember(currentCodepoint))
				{
					lastWhiteSpaceIndex = i;
					
					lastWordWidth = currentWidth;
					
					lastWordMaxAscent = maxAscent;
					tempMaxAscent = 0.0f;
					lastWordMaxDescent = maxDescent;
					tempMaxDescent = 0.0f;
					lastWordMaxLineOffset = maxLineOffset;
					tempMaxLineOffset = 0.0f;
					
					if(currentCodepoint > 0)
					{
						//TODO: To adjsut this correctly, the previous characters attributes are needed...
						float previousOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, currentCodepoint) * scaleFactor + currentAttributes->GetKerning();
						float correctedOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, -1) * scaleFactor + currentAttributes->GetKerning();
						lastWordWidth -= previousOffset - correctedOffset;
					}
				}
				
				if(currentCodepoint == 10)
				{
					totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
					
					linebreaks.push_back(i);
					linewidth.push_back(currentWidth);
					lineascent.push_back(maxAscent);
					linedescent.push_back(maxDescent);
					lineoffset.push_back(maxLineOffset + _additionalLineHeight);
					maxAscent = 0.0f;
					maxDescent = 0.0f;
					maxLineOffset = 0.0f;
					currentWidth = 0.0f;
					offset = 0.0f;
				}
				
				characterCounter += 1;
				
				if(GetBounds().width > 0.0f && currentWidth + offset > GetBounds().width && currentAttributes->GetWrapMode() != TextWrapModeNone)
				{
					Range lastWhitespaceRange(0, 0);
					if(currentAttributes->GetWrapMode() == TextWrapModeWord && lastWhiteSpaceIndex != -1 && (linebreaks.size() == 0 || lastWhiteSpaceIndex > linebreaks.back()))
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linebreaks.push_back(lastWhiteSpaceIndex);
						linewidth.push_back(lastWordWidth);
						lineascent.push_back(lastWordMaxAscent);
						linedescent.push_back(lastWordMaxDescent);
						lineoffset.push_back(lastWordMaxLineOffset + _additionalLineHeight);
						currentWidth -= lastWordWidth;
						maxAscent = tempMaxAscent;
						tempMaxAscent = 0.0f;
						maxDescent = tempMaxDescent;
						tempMaxDescent = 0.0f;
						maxLineOffset = tempMaxLineOffset;
						tempMaxLineOffset = 0.0f;
						
						if(lastWhiteSpaceIndex != i)
						{
							currentWidth -= spacings->GetObjectAtIndex<RN::Number>(lastWhiteSpaceIndex)->GetFloatValue();
							spacings->ReplaceObjectAtIndex(lastWhiteSpaceIndex, RN::Number::WithFloat(0.0f));
						}
						else
						{
							offset = 0.0f;
						}
					}
					else
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linewidth.push_back(currentWidth);
						currentWidth = 0.0f;
						linebreaks.push_back(i);
						
						lineascent.push_back(maxAscent);
						linedescent.push_back(maxDescent);
						lineoffset.push_back(maxLineOffset + _additionalLineHeight);
						maxAscent = 0.0f;
						maxDescent = 0.0f;
						maxLineOffset = 0.0f;
					}
				}
				
				currentWidth += offset;
				spacings->AddObject(RN::Number::WithFloat(offset));
			}
			
			totalHeight += maxAscent;// + maxDescent;
			linewidth.push_back(currentWidth);
			lineascent.push_back(maxAscent);
			linedescent.push_back(maxDescent);
			lineoffset.push_back(maxLineOffset + _additionalLineHeight);
			
			RN::uint32 linebreakIndex = 0;
			
			float characterPositionX = 0.0f;
			float characterPositionY = -lineascent[0] + linedescent[0];
			
			if(_verticalAlignment == TextVerticalAlignmentCenter)
			{
				characterPositionY -= GetBounds().height * 0.5f;
				characterPositionY += totalHeight * 0.5f;
				characterPositionY -= linedescent[0] * 0.5f;
			}
			else if(_verticalAlignment == TextVerticalAlignmentBottom)
			{
				characterPositionY -= GetBounds().height;
				characterPositionY += totalHeight;
			}
			
			const TextAttributes *initialAttributes = _attributedText->GetAttributesAtIndex(0);
			if(!initialAttributes) initialAttributes = &_defaultAttributes;
			
			if(initialAttributes->GetAlignment() == TextAlignmentRight)
				characterPositionX = GetBounds().width - linewidth[linebreakIndex];
			else if(initialAttributes->GetAlignment() == TextAlignmentCenter)
				characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
			
			for(int index = 0; index <= characterCounter; index++)
			{
				if(index > 0) characterPositionX += spacings->GetObjectAtIndex<RN::Number>(index-1)->GetFloatValue();
				
				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(index);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				
				if(index == charIndex)
				{
					break;
				}
				
				if(linebreakIndex < linebreaks.size() && linebreaks[linebreakIndex] == index)
				{
					characterPositionY -= linedescent[linebreakIndex];
					characterPositionY -= lineoffset[linebreakIndex];
					linebreakIndex += 1;
					characterPositionY -= lineascent[linebreakIndex];
					
					characterPositionX = 0.0f;
					if(currentAttributes->GetAlignment() == TextAlignmentRight)
						characterPositionX = GetBounds().width - linewidth[linebreakIndex];
					else if(currentAttributes->GetAlignment() == TextAlignmentCenter)
						characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
				}
			}

			Unlock();
			Vector2 result(characterPositionX, -characterPositionY);
			return result;
		}
	
		size_t Label::GetCharacterAtPosition(const Vector2 &position)
		{
			Lock();
			if(!_attributedText || _attributedText->GetLength() == 0)
			{
				Unlock();
				return 0;
			}
			
			Vector2 realPosition;
			realPosition.x = position.x;
			realPosition.y = /*GetBounds().height +*/ -position.y;
			
			Array *spacings = (new Array(_attributedText->GetLength()))->Autorelease();
			
			std::vector<int64> linebreaks;
			std::vector<float> linewidth;
			std::vector<float> lineascent;
			std::vector<float> linedescent;
			std::vector<float> lineoffset;
			
			float currentWidth = 0.0f;
			float lastWordWidth = 0.0f;
			
			float totalHeight = 0.0f;
			
			float maxAscent = 0.0f;
			float lastWordMaxAscent = 0.0f;
			float tempMaxAscent = 0.0f;
			float maxDescent = 0.0f;
			float lastWordMaxDescent = 0.0f;
			float tempMaxDescent = 0.0f;
			float maxLineOffset = 0.0f;
			float lastWordMaxLineOffset = 0.0f;
			float tempMaxLineOffset = 0.0f;
			
			int characterCounter = 0;
			
			int64 lastWhiteSpaceIndex = -1;
			CharacterSet *whiteSpaces = CharacterSet::WithWhitespaces();
			for(int64 i = 0; i < _attributedText->GetLength(); i++)
			{
				int currentCodepoint = _attributedText->GetCharacterAtIndex(i);
				int nextCodepoint = i < _attributedText->GetLength()-1? _attributedText->GetCharacterAtIndex(i+1) : -1;

				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(i);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				
				Font *currentFont = currentAttributes->GetFont();
				
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				float offset = currentFont->GetOffsetForNextCharacter(currentCodepoint, nextCodepoint) * scaleFactor + currentAttributes->GetKerning();
				
				float characterAscent = currentFont->GetAscent() * scaleFactor;
				float characterDescent = -currentFont->GetDescent() * scaleFactor;
				float characterLineOffset = currentFont->GetLineOffset() * scaleFactor;
				maxAscent = std::max(maxAscent, characterAscent);
				tempMaxAscent = std::max(tempMaxAscent, characterAscent);
				maxDescent = std::max(maxDescent, characterDescent);
				tempMaxDescent = std::max(tempMaxDescent, characterDescent);
				maxLineOffset = std::max(maxLineOffset, characterLineOffset);
				tempMaxLineOffset = std::max(tempMaxLineOffset, characterLineOffset);
				
				if(whiteSpaces->CharacterIsMember(currentCodepoint))
				{
					lastWhiteSpaceIndex = i;
					
					lastWordWidth = currentWidth;
					
					lastWordMaxAscent = maxAscent;
					tempMaxAscent = 0.0f;
					lastWordMaxDescent = maxDescent;
					tempMaxDescent = 0.0f;
					lastWordMaxLineOffset = maxLineOffset;
					tempMaxLineOffset = 0.0f;
					
					if(currentCodepoint > 0)
					{
						//TODO: To adjsut this correctly, the previous characters attributes are needed...
						float previousOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, currentCodepoint) * scaleFactor + currentAttributes->GetKerning();
						float correctedOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, -1) * scaleFactor + currentAttributes->GetKerning();
						lastWordWidth -= previousOffset - correctedOffset;
					}
				}
				
				if(currentCodepoint == 10)
				{
					totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
					
					linebreaks.push_back(i);
					linewidth.push_back(currentWidth);
					lineascent.push_back(maxAscent);
					linedescent.push_back(maxDescent);
					lineoffset.push_back(maxLineOffset + _additionalLineHeight);
					maxAscent = 0.0f;
					maxDescent = 0.0f;
					maxLineOffset = 0.0f;
					currentWidth = 0.0f;
					offset = 0.0f;
				}
				
				characterCounter += 1;
				
				if(GetBounds().width > 0.0f && currentWidth + offset > GetBounds().width && currentAttributes->GetWrapMode() != TextWrapModeNone)
				{
					Range lastWhitespaceRange(0, 0);
					if(currentAttributes->GetWrapMode() == TextWrapModeWord && lastWhiteSpaceIndex != -1 && (linebreaks.size() == 0 || lastWhiteSpaceIndex > linebreaks.back()))
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linebreaks.push_back(lastWhiteSpaceIndex);
						linewidth.push_back(lastWordWidth);
						lineascent.push_back(lastWordMaxAscent);
						linedescent.push_back(lastWordMaxDescent);
						lineoffset.push_back(lastWordMaxLineOffset + _additionalLineHeight);
						currentWidth -= lastWordWidth;
						maxAscent = tempMaxAscent;
						tempMaxAscent = 0.0f;
						maxDescent = tempMaxDescent;
						tempMaxDescent = 0.0f;
						maxLineOffset = tempMaxLineOffset;
						tempMaxLineOffset = 0.0f;
						
						if(lastWhiteSpaceIndex != i)
						{
							currentWidth -= spacings->GetObjectAtIndex<RN::Number>(lastWhiteSpaceIndex)->GetFloatValue();
							spacings->ReplaceObjectAtIndex(lastWhiteSpaceIndex, RN::Number::WithFloat(0.0f));
						}
						else
						{
							offset = 0.0f;
						}
					}
					else
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linewidth.push_back(currentWidth);
						currentWidth = 0.0f;
						linebreaks.push_back(i);
						
						lineascent.push_back(maxAscent);
						linedescent.push_back(maxDescent);
						lineoffset.push_back(maxLineOffset + _additionalLineHeight);
						maxAscent = 0.0f;
						maxDescent = 0.0f;
						maxLineOffset = 0.0f;
					}
				}
				
				currentWidth += offset;
				spacings->AddObject(RN::Number::WithFloat(offset));
			}
			
			totalHeight += maxAscent;// + maxDescent;
			linewidth.push_back(currentWidth);
			lineascent.push_back(maxAscent);
			linedescent.push_back(maxDescent);
			lineoffset.push_back(maxLineOffset + _additionalLineHeight);
			
			RN::uint32 linebreakIndex = 0;
			
			float characterPositionX = 0.0f;
			float characterPositionY = -lineascent[0] + linedescent[0];
			
			if(_verticalAlignment == TextVerticalAlignmentCenter)
			{
				characterPositionY -= GetBounds().height * 0.5f;
				characterPositionY += totalHeight * 0.5f;
				characterPositionY -= linedescent[0] * 0.5f;
			}
			else if(_verticalAlignment == TextVerticalAlignmentBottom)
			{
				characterPositionY -= GetBounds().height;
				characterPositionY += totalHeight;
			}
			
			const TextAttributes *initialAttributes = _attributedText->GetAttributesAtIndex(0);
			if(!initialAttributes) initialAttributes = &_defaultAttributes;
			
			if(initialAttributes->GetAlignment() == TextAlignmentRight)
				characterPositionX = GetBounds().width - linewidth[linebreakIndex];
			else if(initialAttributes->GetAlignment() == TextAlignmentCenter)
				characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
			
			float closestDistance = 0.0;
			size_t closestIndex = -1;
			
			for(int index = 0; index <= characterCounter; index++)
			{
				if(index > 0) characterPositionX += spacings->GetObjectAtIndex<RN::Number>(index-1)->GetFloatValue();
				
				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(index);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				
				Vector2 characterPosition(characterPositionX, characterPositionY);
				float newDistance = characterPosition.GetSquaredDistance(realPosition);
				if(newDistance < closestDistance || closestIndex == -1)
				{
					closestDistance = newDistance;
					closestIndex = index;
				}
				
				if(linebreakIndex < linebreaks.size() && linebreaks[linebreakIndex] == index)
				{
					characterPositionY -= linedescent[linebreakIndex];
					characterPositionY -= lineoffset[linebreakIndex];
					linebreakIndex += 1;
					characterPositionY -= lineascent[linebreakIndex];
					
					characterPositionX = 0.0f;
					if(currentAttributes->GetAlignment() == TextAlignmentRight)
						characterPositionX = GetBounds().width - linewidth[linebreakIndex];
					else if(currentAttributes->GetAlignment() == TextAlignmentCenter)
						characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
				}
			}

			Unlock();
			return closestIndex;
		}
		
		Vector2 Label::GetTextSize()
		{
			Lock();
			if(!_attributedText || _attributedText->GetLength() == 0)
			{
				Unlock();
				return Vector2();
			}
			
			Array *spacings = (new Array(_attributedText->GetLength()))->Autorelease();
			
			std::vector<int> linebreaks;
			std::vector<float> lineascent;
			std::vector<float> linedescent;
			std::vector<float> lineoffset;
			
			float currentWidth = 0.0f;
			float lastWordWidth = 0.0f;
			
			float totalHeight = 0.0f;
			float maxWidth = 0.0f;
			
			float maxAscent = 0.0f;
			float lastWordMaxAscent = 0.0f;
			float tempMaxAscent = 0.0f;
			float maxDescent = 0.0f;
			float lastWordMaxDescent = 0.0f;
			float tempMaxDescent = 0.0f;
			float maxLineOffset = 0.0f;
			float lastWordMaxLineOffset = 0.0f;
			float tempMaxLineOffset = 0.0f;
			
			int lastWhiteSpaceIndex = -1;
			CharacterSet *whiteSpaces = CharacterSet::WithWhitespaces();
			for(int64 i = 0; i < _attributedText->GetLength(); i++)
			{
				int currentCodepoint = _attributedText->GetCharacterAtIndex(i);
				int nextCodepoint = i < _attributedText->GetLength()-1? _attributedText->GetCharacterAtIndex(i+1) : -1;

				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(i);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				
				Font *currentFont = currentAttributes->GetFont();
				
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				float offset = currentFont->GetOffsetForNextCharacter(currentCodepoint, nextCodepoint) * scaleFactor + currentAttributes->GetKerning();
				
				float characterAscent = currentFont->GetAscent() * scaleFactor;
				float characterDescent = -currentFont->GetDescent() * scaleFactor;
				float characterLineOffset = currentFont->GetLineOffset() * scaleFactor;
				maxAscent = std::max(maxAscent, characterAscent);
				tempMaxAscent = std::max(tempMaxAscent, characterAscent);
				maxDescent = std::max(maxDescent, characterDescent);
				tempMaxDescent = std::max(tempMaxDescent, characterDescent);
				maxLineOffset = std::max(maxLineOffset, characterLineOffset);
				tempMaxLineOffset = std::max(tempMaxLineOffset, characterLineOffset);
				
				if(whiteSpaces->CharacterIsMember(currentCodepoint))
				{
					lastWhiteSpaceIndex = i;
					
					lastWordWidth = currentWidth;
					
					lastWordMaxAscent = maxAscent;
					tempMaxAscent = 0.0f;
					lastWordMaxDescent = maxDescent;
					tempMaxDescent = 0.0f;
					lastWordMaxLineOffset = maxLineOffset;
					tempMaxLineOffset = 0.0f;
					
					if(currentCodepoint > 0)
					{
						//TODO: To adjsut this correctly, the previous characters attributes are needed...
						float previousOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, currentCodepoint) * scaleFactor + currentAttributes->GetKerning();
						float correctedOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, -1) * scaleFactor + currentAttributes->GetKerning();
						lastWordWidth -= previousOffset - correctedOffset;
					}
				}
				
				if(currentCodepoint == 10)
				{
					totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
					if(currentWidth > maxWidth) maxWidth = currentWidth;
					
					linebreaks.push_back(i);
					lineascent.push_back(maxAscent);
					linedescent.push_back(maxDescent);
					lineoffset.push_back(maxLineOffset + _additionalLineHeight);
					maxAscent = 0.0f;
					maxDescent = 0.0f;
					maxLineOffset = 0.0f;
					currentWidth = 0.0f;
					offset = 0.0f;
				}
				
				if(GetBounds().width > 0.0f && currentWidth + offset > GetBounds().width && currentAttributes->GetWrapMode() != TextWrapModeNone)
				{
					Range lastWhitespaceRange(0, 0);
					lastWhitespaceRange.length = 0;
					if(currentAttributes->GetWrapMode() == TextWrapModeWord && lastWhiteSpaceIndex != -1 && (linebreaks.size() == 0 || lastWhiteSpaceIndex > linebreaks.back()))
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						if(lastWordWidth > maxWidth) maxWidth = lastWordWidth;
						
						linebreaks.push_back(lastWhiteSpaceIndex);
						lineascent.push_back(lastWordMaxAscent);
						linedescent.push_back(lastWordMaxDescent);
						lineoffset.push_back(lastWordMaxLineOffset + _additionalLineHeight);
						currentWidth -= lastWordWidth;
						maxAscent = tempMaxAscent;
						tempMaxAscent = 0.0f;
						maxDescent = tempMaxDescent;
						tempMaxDescent = 0.0f;
						maxLineOffset = tempMaxLineOffset;
						tempMaxLineOffset = 0.0f;
						
						if(lastWhiteSpaceIndex != i)
						{
							currentWidth -= spacings->GetObjectAtIndex<RN::Number>(lastWhiteSpaceIndex)->GetFloatValue();
							spacings->ReplaceObjectAtIndex(lastWhiteSpaceIndex, RN::Number::WithFloat(0.0f));
						}
						else
						{
							offset = 0.0f;
						}
					}
					else
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						if(currentWidth > maxWidth) maxWidth = currentWidth;
						
						currentWidth = 0.0f;
						linebreaks.push_back(i);
						
						lineascent.push_back(maxAscent);
						linedescent.push_back(maxDescent);
						lineoffset.push_back(maxLineOffset + _additionalLineHeight);
						maxAscent = 0.0f;
						maxDescent = 0.0f;
						maxLineOffset = 0.0f;
					}
				}
				
				currentWidth += offset;
				spacings->AddObject(RN::Number::WithFloat(offset));
			}
			
			totalHeight += maxAscent;// + maxDescent;
			if(currentWidth > maxWidth) maxWidth = currentWidth;
			lineascent.push_back(maxAscent);
			linedescent.push_back(maxDescent);
			lineoffset.push_back(maxLineOffset + _additionalLineHeight);

			Unlock();
			return Vector2(maxWidth, totalHeight);
		}
	
	
		void Label::UpdateModel()
		{
			View::UpdateModel();
			
			Lock();
			if(!_attributedText || _attributedText->GetLength() == 0)
			{
				Model *model = GetModel();
				if(model->GetLODStage(0)->GetCount() > 1)
				{
					Material *textMaterial = model->GetLODStage(0)->GetMaterialAtIndex(2);
					textMaterial->SetSkipRendering(true);
					Material *shadowMaterial = model->GetLODStage(0)->GetMaterialAtIndex(1);
					shadowMaterial->SetSkipRendering(true);
				}
				Unlock();
				return;
			}
			
			bool isUsingSDF = _defaultAttributes.GetFont()->IsSDFFont();
			
			uint32 numberOfVertices = 0;
			uint32 numberOfIndices = 0;
			
			Array *characters = (new Array(_attributedText->GetLength()))->Autorelease();
			Array *spacings = (new Array(_attributedText->GetLength()))->Autorelease();
			
			std::vector<int> linebreaks;
			std::vector<float> linewidth;
			std::vector<float> lineascent;
			std::vector<float> linedescent;
			std::vector<float> lineoffset;
			
			float currentWidth = 0.0f;
			float lastWordWidth = 0.0f;
			
			float totalHeight = 0.0f;
			
			float maxAscent = 0.0f;
			float lastWordMaxAscent = 0.0f;
			float tempMaxAscent = 0.0f;
			float maxDescent = 0.0f;
			float lastWordMaxDescent = 0.0f;
			float tempMaxDescent = 0.0f;
			float maxLineOffset = 0.0f;
			float lastWordMaxLineOffset = 0.0f;
			float tempMaxLineOffset = 0.0f;
			
			int lastWhiteSpaceIndex = -1;
			CharacterSet *whiteSpaces = CharacterSet::WithWhitespaces();
			for(int64 i = 0; i < _attributedText->GetLength(); i++)
			{
				int currentCodepoint = _attributedText->GetCharacterAtIndex(i);
				int nextCodepoint = i < _attributedText->GetLength()-1? _attributedText->GetCharacterAtIndex(i+1) : -1;

				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(i);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				
				Font *currentFont = currentAttributes->GetFont();
				
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				float offset = currentFont->GetOffsetForNextCharacter(currentCodepoint, nextCodepoint) * scaleFactor + currentAttributes->GetKerning();
				
				float characterAscent = currentFont->GetAscent() * scaleFactor;
				float characterDescent = -currentFont->GetDescent() * scaleFactor;
				float characterLineOffset = currentFont->GetLineOffset() * scaleFactor;
				maxAscent = std::max(maxAscent, characterAscent);
				tempMaxAscent = std::max(tempMaxAscent, characterAscent);
				maxDescent = std::max(maxDescent, characterDescent);
				tempMaxDescent = std::max(tempMaxDescent, characterDescent);
				maxLineOffset = std::max(maxLineOffset, characterLineOffset);
				tempMaxLineOffset = std::max(tempMaxLineOffset, characterLineOffset);
				
				if(whiteSpaces->CharacterIsMember(currentCodepoint))
				{
					lastWhiteSpaceIndex = i;
					
					lastWordWidth = currentWidth;
					
					lastWordMaxAscent = maxAscent;
					tempMaxAscent = 0.0f;
					lastWordMaxDescent = maxDescent;
					tempMaxDescent = 0.0f;
					lastWordMaxLineOffset = maxLineOffset;
					tempMaxLineOffset = 0.0f;
					
					if(currentCodepoint > 0)
					{
						//TODO: To adjsut this correctly, the previous characters attributes are needed...
						float previousOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, currentCodepoint) * scaleFactor + currentAttributes->GetKerning();
						float correctedOffset = currentFont->GetOffsetForNextCharacter(currentCodepoint-1, -1) * scaleFactor + currentAttributes->GetKerning();
						lastWordWidth -= previousOffset - correctedOffset;
					}
				}
				
				if(currentCodepoint == 10)
				{
					totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
					
					linebreaks.push_back(i);
					linewidth.push_back(currentWidth);
					lineascent.push_back(maxAscent);
					linedescent.push_back(maxDescent);
					lineoffset.push_back(maxLineOffset + _additionalLineHeight);
					maxAscent = 0.0f;
					maxDescent = 0.0f;
					maxLineOffset = 0.0f;
					currentWidth = 0.0f;
					offset = 0.0f;
				}
				
				Mesh *mesh = currentFont->GetMeshForCharacter(currentCodepoint);
				if(mesh)
				{
					characters->AddObject(mesh);
					
					numberOfVertices += mesh->GetVerticesCount();
					numberOfIndices += mesh->GetIndicesCount();
				}
				else
				{
					characters->AddObject(RN::Null::GetNull());
				}
				
				if(GetBounds().width > 0.0f && currentWidth + offset > GetBounds().width && currentAttributes->GetWrapMode() != TextWrapModeNone)
				{
					Range lastWhitespaceRange(0, 0);
					if(currentAttributes->GetWrapMode() == TextWrapModeWord && lastWhiteSpaceIndex != -1 && (linebreaks.size() == 0 || lastWhiteSpaceIndex > linebreaks.back()))
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linebreaks.push_back(lastWhiteSpaceIndex);
						linewidth.push_back(lastWordWidth);
						lineascent.push_back(lastWordMaxAscent);
						linedescent.push_back(lastWordMaxDescent);
						lineoffset.push_back(lastWordMaxLineOffset + _additionalLineHeight);
						currentWidth -= lastWordWidth;
						maxAscent = tempMaxAscent;
						tempMaxAscent = 0.0f;
						maxDescent = tempMaxDescent;
						tempMaxDescent = 0.0f;
						maxLineOffset = tempMaxLineOffset;
						tempMaxLineOffset = 0.0f;
						
						if(lastWhiteSpaceIndex != i)
						{
							currentWidth -= spacings->GetObjectAtIndex<RN::Number>(lastWhiteSpaceIndex)->GetFloatValue();
							spacings->ReplaceObjectAtIndex(lastWhiteSpaceIndex, RN::Number::WithFloat(0.0f));
						}
						else
						{
							offset = 0.0f;
						}
					}
					else
					{
						totalHeight += maxAscent + maxDescent + maxLineOffset + _additionalLineHeight;
						
						linewidth.push_back(currentWidth);
						currentWidth = 0.0f;
						linebreaks.push_back(i);
						
						lineascent.push_back(maxAscent);
						linedescent.push_back(maxDescent);
						lineoffset.push_back(maxLineOffset + _additionalLineHeight);
						maxAscent = 0.0f;
						maxDescent = 0.0f;
						maxLineOffset = 0.0f;
					}
				}
				
				currentWidth += offset;
				spacings->AddObject(RN::Number::WithFloat(offset));
			}
			
			if(numberOfIndices < 3)
			{
				Model *model = GetModel();
				if(model->GetLODStage(0)->GetCount() > 1)
				{
					Material *textMaterial = model->GetLODStage(0)->GetMaterialAtIndex(2);
					textMaterial->SetSkipRendering(true);
					Material *shadowMaterial = model->GetLODStage(0)->GetMaterialAtIndex(1);
					shadowMaterial->SetSkipRendering(true);
				}
				Unlock();
				return;
			}
			
			totalHeight += maxAscent;// + maxDescent;
			linewidth.push_back(currentWidth);
			lineascent.push_back(maxAscent);
			linedescent.push_back(maxDescent);
			lineoffset.push_back(maxLineOffset + _additionalLineHeight);

			float *vertexPositionBuffer = new float[numberOfVertices * 2];
			float *vertexUVBuffer = new float[numberOfVertices * (isUsingSDF? 2 : 3)];
			float *vertexColorBuffer = new float[numberOfVertices * 4];
			
			RN::uint32 *indexBuffer = new RN::uint32[numberOfIndices];
			
			RN::uint32 vertexOffset = 0;
			RN::uint32 indexIndexOffset = 0;
			RN::uint32 indexOffset = 0;
			
			RN::uint32 linebreakIndex = 0;
			
			float characterPositionX = 0.0f;
			float characterPositionY = -lineascent[0] + linedescent[0];
			
			if(_verticalAlignment == TextVerticalAlignmentCenter)
			{
				characterPositionY -= GetBounds().height * 0.5f;
				characterPositionY += totalHeight * 0.5f;
				characterPositionY -= linedescent[0] * 0.5f;
			}
			else if(_verticalAlignment == TextVerticalAlignmentBottom)
			{
				characterPositionY -= GetBounds().height;
				characterPositionY += totalHeight;
			}
			
			const TextAttributes *initialAttributes = _attributedText->GetAttributesAtIndex(0);
			if(!initialAttributes) initialAttributes = &_defaultAttributes;
			
			if(initialAttributes->GetAlignment() == TextAlignmentRight)
				characterPositionX = GetBounds().width - linewidth[linebreakIndex];
			else if(initialAttributes->GetAlignment() == TextAlignmentCenter)
				characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
			
			characters->Enumerate<RN::Mesh>([&](RN::Mesh *mesh, size_t index, bool &stop){
				if(index > 0) characterPositionX += spacings->GetObjectAtIndex<RN::Number>(index-1)->GetFloatValue();
				
				const TextAttributes *currentAttributes = _attributedText->GetAttributesAtIndex(index);
				if(!currentAttributes) currentAttributes = &_defaultAttributes;
				float scaleFactor = currentAttributes->GetFontSize() / currentAttributes->GetFont()->GetHeight();
				
				if(linebreakIndex < linebreaks.size() && linebreaks[linebreakIndex] == index)
				{
					characterPositionY -= linedescent[linebreakIndex];
					characterPositionY -= lineoffset[linebreakIndex];
					linebreakIndex += 1;
					characterPositionY -= lineascent[linebreakIndex];
					
					characterPositionX = 0.0f;
					if(currentAttributes->GetAlignment() == TextAlignmentRight)
						characterPositionX = GetBounds().width - linewidth[linebreakIndex];
					else if(currentAttributes->GetAlignment() == TextAlignmentCenter)
						characterPositionX = (GetBounds().width - linewidth[linebreakIndex]) * 0.5f;
					
					return;
				}
				
				if(!mesh->IsKindOfClass(RN::Mesh::GetMetaClass()))
				{
					return;
				}
				
				RN::Mesh::Chunk chunk = mesh->GetChunk();
				for(size_t i = 0; i < mesh->GetVerticesCount(); i++)
				{
					RN::Vector2 vertexPosition = *chunk.GetIteratorAtIndex<RN::Vector2>(RN::Mesh::VertexAttribute::Feature::Vertices, i);
					RN::uint32 targetIndex = vertexOffset + i;
					
					vertexPositionBuffer[targetIndex * 2 + 0] = vertexPosition.x * scaleFactor + characterPositionX;
					vertexPositionBuffer[targetIndex * 2 + 1] = vertexPosition.y * scaleFactor + characterPositionY;
					
					if(isUsingSDF)
					{
						RN::Vector2 vertexUV0 = *chunk.GetIteratorAtIndex<RN::Vector2>(RN::Mesh::VertexAttribute::Feature::UVCoords0, i);
						vertexUVBuffer[targetIndex * 2 + 0] = vertexUV0.x;
						vertexUVBuffer[targetIndex * 2 + 1] = vertexUV0.y;
					}
					else
					{
						RN::Vector3 vertexUV1 = *chunk.GetIteratorAtIndex<RN::Vector3>(RN::Mesh::VertexAttribute::Feature::UVCoords0, i);
						vertexUVBuffer[targetIndex * 3 + 0] = vertexUV1.x;
						vertexUVBuffer[targetIndex * 3 + 1] = vertexUV1.y;
						vertexUVBuffer[targetIndex * 3 + 2] = vertexUV1.z;
					}
					
					vertexColorBuffer[targetIndex * 4 + 0] = currentAttributes->GetColor().r;
					vertexColorBuffer[targetIndex * 4 + 1] = currentAttributes->GetColor().g;
					vertexColorBuffer[targetIndex * 4 + 2] = currentAttributes->GetColor().b;
					vertexColorBuffer[targetIndex * 4 + 3] = currentAttributes->GetColor().a;
				}
				
				for(size_t i = 0; i < mesh->GetIndicesCount(); i++)
				{
					indexBuffer[indexIndexOffset + i] = *chunk.GetIteratorAtIndex<RN::uint32>(RN::Mesh::VertexAttribute::Feature::Indices, i) + indexOffset;
				}
				
				vertexOffset += mesh->GetVerticesCount();
				indexOffset += mesh->GetVerticesCount();
				indexIndexOffset += mesh->GetIndicesCount();
			});
			
			std::vector<RN::Mesh::VertexAttribute> meshVertexAttributes;
			meshVertexAttributes.emplace_back(RN::Mesh::VertexAttribute::Feature::Vertices, RN::PrimitiveType::Vector2);
			if(isUsingSDF) meshVertexAttributes.emplace_back(RN::Mesh::VertexAttribute::Feature::UVCoords0, RN::PrimitiveType::Vector2);
			else meshVertexAttributes.emplace_back(RN::Mesh::VertexAttribute::Feature::UVCoords1, RN::PrimitiveType::Vector3);
			meshVertexAttributes.emplace_back(RN::Mesh::VertexAttribute::Feature::Color0, RN::PrimitiveType::Vector4);
			meshVertexAttributes.emplace_back(RN::Mesh::VertexAttribute::Feature::Indices, RN::PrimitiveType::Uint32);
			
			RN::Mesh *textMesh = new RN::Mesh(meshVertexAttributes, numberOfVertices, numberOfIndices);
			textMesh->BeginChanges();
			
			textMesh->SetElementData(RN::Mesh::VertexAttribute::Feature::Vertices, vertexPositionBuffer);
			if(isUsingSDF) textMesh->SetElementData(RN::Mesh::VertexAttribute::Feature::UVCoords0, vertexUVBuffer);
			else textMesh->SetElementData(RN::Mesh::VertexAttribute::Feature::UVCoords1, vertexUVBuffer);
			textMesh->SetElementData(RN::Mesh::VertexAttribute::Feature::Color0, vertexColorBuffer);
			textMesh->SetElementData(RN::Mesh::VertexAttribute::Feature::Indices, indexBuffer);
			
			textMesh->EndChanges();

			delete[] vertexPositionBuffer;
			delete[] vertexUVBuffer;
			delete[] vertexColorBuffer;
			delete[] indexBuffer;
			
			RN::Model *model = GetModel();
			if(model->GetLODStage(0)->GetCount() == 1)
			{
				RN::Material *material = _textMaterial;
				if(!material)
				{
					material = RN::Material::WithShaders(nullptr, nullptr);
					
					RN::Shader::Options *shaderOptions = RN::Shader::Options::WithMesh(textMesh); //This also enables RN_UV1
					shaderOptions->EnableAlpha();
					shaderOptions->AddDefine("RN_UI", "1");
					if(isUsingSDF)
					{
						shaderOptions->AddDefine("RN_UI_SDF", "1");
						material->AddTexture(_defaultAttributes.GetFont()->GetFontTexture());
					}
					
					material->SetVertexShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Vertex, shaderOptions));
					material->SetFragmentShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Fragment, shaderOptions));
					material->SetVertexShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Vertex, shaderOptions, RN::Shader::UsageHint::Multiview), RN::Shader::UsageHint::Multiview);
					material->SetFragmentShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Fragment, shaderOptions, RN::Shader::UsageHint::Multiview), RN::Shader::UsageHint::Multiview);
					
					SetTextMaterial(material);
				}
				
				RN::Material *shadowMaterial = _shadowMaterial;
				if(!shadowMaterial)
				{
					shadowMaterial = RN::Material::WithShaders(nullptr, nullptr);
					
					RN::Shader::Options *shadowShaderOptions = RN::Shader::Options::WithNone();
					shadowShaderOptions->EnableAlpha();
					shadowShaderOptions->AddDefine("RN_UI", "1");
					if(isUsingSDF)
					{
						shadowShaderOptions->AddDefine("RN_UV0", "1");
						shadowShaderOptions->AddDefine("RN_UI_SDF", "1");
						shadowMaterial->AddTexture(_defaultAttributes.GetFont()->GetFontTexture());
					}
					else
					{
						shadowShaderOptions->AddDefine("RN_UV1", "1");
					}
					
					shadowMaterial->SetVertexShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Vertex, shadowShaderOptions));
					shadowMaterial->SetFragmentShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Fragment, shadowShaderOptions));
					shadowMaterial->SetVertexShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Vertex, shadowShaderOptions, RN::Shader::UsageHint::Multiview), RN::Shader::UsageHint::Multiview);
					shadowMaterial->SetFragmentShader(Renderer::GetActiveRenderer()->GetDefaultShader(Shader::Type::Fragment, shadowShaderOptions, RN::Shader::UsageHint::Multiview), RN::Shader::UsageHint::Multiview);
					
					SetTextShadowMaterial(shadowMaterial);
				}
				
				model->GetLODStage(0)->AddMesh(textMesh, shadowMaterial);
				model->GetLODStage(0)->AddMesh(textMesh->Autorelease(), material);
				
				model->Retain();
				SetModel(model);
				model->Release();
			}
			else
			{
				model->GetLODStage(0)->ReplaceMesh(textMesh, 1);
				model->GetLODStage(0)->ReplaceMesh(textMesh->Autorelease(), 2);
			}
			
			Material *textMaterial = model->GetLODStage(0)->GetMaterialAtIndex(2);
			textMaterial->SetSkipRendering(_combinedOpacityFactor  < k::EpsilonFloat);
			Material *shadowMaterial = model->GetLODStage(0)->GetMaterialAtIndex(1);
			shadowMaterial->SetSkipRendering(_shadowColor.a < k::EpsilonFloat);
			
			model->CalculateBoundingVolumes();
			SetBoundingBox(model->GetBoundingBox());
			
			Unlock();
		}
	
		void Label::SetOpacityFromParent(float parentCombinedOpacity)
		{
			View::SetOpacityFromParent(parentCombinedOpacity);
			Lock();
			
			if(_textMaterial)
			{
				Color finalColor = Color::White();
				finalColor.a *= _combinedOpacityFactor;
				_textMaterial->SetDiffuseColor(finalColor);
				_textMaterial->SetSkipRendering(finalColor.a < k::EpsilonFloat || !_attributedText || _attributedText->GetLength() == 0);
			}
			
			if(_shadowMaterial)
			{
				Color finalColor = _shadowColor;
				finalColor.a *= _combinedOpacityFactor;
				_shadowMaterial->SetDiffuseColor(finalColor);
				_shadowMaterial->SetSkipRendering(finalColor.a < k::EpsilonFloat || !_attributedText || _attributedText->GetLength() == 0);
			}
			
			Unlock();
		}
	
		void Label::Update(float delta)
		{
			View::Update(delta);
			
			if(!_cursorView) return;
			_cursorBlinkTimer += delta;
			
			if(_cursorBlinkTimer > 0.5f)
			{
				_cursorView->SetHidden(!_cursorView->GetIsHidden());
				_cursorBlinkTimer -= 0.5f;
			}
		}
	}
}
