//
//  RNResponder.h
//  Rayne
//
//  Copyright 2013 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE_RESPONDER_H__
#define __RAYNE_RESPONDER_H__

#include "RNBase.h"
#include "RNObject.h"

namespace RN
{
	class Responder : public Object
	{
	public:
		virtual Responder *NextResponder() const;
		
		virtual bool CanBecomeFirstResponder() const;
		virtual bool CanResignFirstReponder() const;
		
		virtual bool BecomeFirstResponder();
		virtual bool ResignFirstResponder();
		
		bool IsFirstResponder() const;
		static Responder *FirstResponder();
		
	protected:
		Responder();
		virtual ~Responder();
	};
}

#endif /* __RAYNE_RESPONDER_H__ */
