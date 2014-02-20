//
//  RNUIUtilities.h
//  Rayne
//
//  Copyright 2014 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#ifndef __RAYNE__UIUTILITIES_H__
#define __RAYNE__UIUTILITIES_H__

#include "RNBase.h"
#include "RNRingbuffer.h"
#include "RNLoggingEngine.h"
#include "RNUIWidget.h"
#include "RNUILabel.h"
#include "RNUIButton.h"
#include "RNUITableView.h"

namespace RN
{
	namespace UI
	{
		class DebugWidget : public Widget
		{
		public:
			RNAPI DebugWidget();
			RNAPI ~DebugWidget() override;
			
		private:
			void HandleMessage(Message *message);
			
			float AverageFPS();
			
			Label *_label;
			Button *_fpsCheckbox;
			stl::ring_buffer<float> _fps;
		};
		
		class ConsoleWidget : public Widget, Log::LoggingEngineDelegate, TableViewDataSource
		{
		public:
			RNAPI ConsoleWidget();
			RNAPI ~ConsoleWidget();
			
			RNAPI void SetLogLevel(Log::Level level);
			
		protected:
			RNAPI void Update() override;
			
		private:
			size_t TableViewNumberOfRows(TableView *tableView) override;
			TableViewCell *TableViewCellForRow(TableView *tableView, size_t row) override;
			
			void CommitQueue();
			
			void Open() override;
			void Close() override;
			bool IsOpen() const override;
			
			void CutOff() override;
			void Write(const Log::Message& message) override;
			
			TableView *_table;
			Log::CallbackLoggingEngine *_engine;
			bool _dirty;
			
			std::vector<std::string> _queue;
			stl::ring_buffer<std::string> _messages;
		};
	}
}

#endif /* __RAYNE__UIUTILITIES_H__ */
