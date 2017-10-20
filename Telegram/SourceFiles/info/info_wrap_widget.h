/*
This file is part of Telegram Desktop,
the official desktop version of Telegram messaging app, see https://telegram.org

Telegram Desktop is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

It is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

In addition, as a special exception, the copyright holders give permission
to link the code of portions of this program with the OpenSSL library.

Full license: https://github.com/telegramdesktop/tdesktop/blob/master/LICENSE
Copyright (c) 2014-2017 John Preston, https://desktop.telegram.org
*/
#pragma once

#include <rpl/variable.h>
#include <rpl/event_stream.h>
#include "window/section_widget.h"

namespace Ui {
class SettingsSlider;
class FadeShadow;
class PlainShadow;
} // namespace Ui

namespace Window {
enum class SlideDirection;
} // namespace Window

namespace Info {
namespace Profile {
class Widget;
} // namespace Profile

namespace Media {
class Widget;
} // namespace Media

class Section;
class Memento;
class MoveMemento;
class ContentMemento;
class ContentWidget;
class TopBar;
class TopBarOverride;

enum class Wrap {
	Layer,
	Narrow,
	Side,
};

class Section final {
public:
	enum class Type {
		Profile,
		Media,
		CommonGroups,
	};
	using MediaType = Storage::SharedMediaType;

	Section(Type type) : _type(type) {
		Expects(type != Type::Media);
	}
	Section(MediaType mediaType)
		: _type(Type::Media)
		, _mediaType(mediaType) {
	}

	Type type() const {
		return _type;
	}
	MediaType mediaType() const {
		Expects(_type == Type::Media);
		return _mediaType;
	}

private:
	Type _type;
	Storage::SharedMediaType _mediaType;

};

struct SelectedItem {
	explicit SelectedItem(FullMsgId msgId) : msgId(msgId) {
	}

	FullMsgId msgId;
	bool canDelete = false;
	bool canForward = false;
};

struct SelectedItems {
	explicit SelectedItems(Storage::SharedMediaType type)
	: type(type) {
	}

	Storage::SharedMediaType type;
	std::vector<SelectedItem> list;

};

class WrapWidget final : public Window::SectionWidget {
public:
	WrapWidget(
		QWidget *parent,
		not_null<Window::Controller*> controller,
		Wrap wrap,
		not_null<Memento*> memento);

	not_null<PeerData*> peer() const;
	PeerData *peerForDialogs() const override {
		return peer();
	}

	Wrap wrap() const;
	void setWrap(Wrap wrap);

	bool hasTopBarShadow() const override;
	QPixmap grabForShowAnimation(
		const Window::SectionSlideParams &params) override;
	void showAnimatedHook(
		const Window::SectionSlideParams &params) override;
	void forceContentRepaint();

	bool showInternal(
		not_null<Window::SectionMemento*> memento,
		const Window::SectionShow &params) override;
	std::unique_ptr<Window::SectionMemento> createMemento() override;

	rpl::producer<int> desiredHeightValue() const override;

	void updateInternalState(not_null<Memento*> memento);
	void saveState(not_null<Memento*> memento);

	// Float player interface.
	bool wheelEventFromFloatPlayer(QEvent *e) override;
	QRect rectForFloatPlayer() const override;

	~WrapWidget();

protected:
	void resizeEvent(QResizeEvent *e) override;

	void doSetInnerFocus() override;
	void showFinishedHook() override;

private:
	using SlideDirection = Window::SlideDirection;
	using SectionSlideParams = Window::SectionSlideParams;
	enum class Tab {
		Profile,
		Media,
		None,
	};
	struct StackItem;

	void showBackFromStack();
	void showNewContent(not_null<ContentMemento*> memento);
	void showNewContent(
		not_null<ContentMemento*> memento,
		const Window::SectionShow &params);
	void setupTop(const Section &section, PeerId peerId);
	void setupTabbedTop(const Section &section);
	void setupTabs(Tab tab);
	void createTabs();
	void createTopBar(
		const Section &section,
		PeerId peerId);

	not_null<RpWidget*> topWidget() const;

	QRect contentGeometry() const;
	rpl::producer<int> desiredHeightForContent() const;
	void finishShowContent();
	rpl::producer<bool> topShadowToggledValue() const;
	void updateContentGeometry();

	void showTab(Tab tab);
	void showContent(object_ptr<ContentWidget> content);
	object_ptr<ContentWidget> createContent(Tab tab);
	object_ptr<Profile::Widget> createProfileWidget();
	object_ptr<Media::Widget> createMediaWidget();
	object_ptr<ContentWidget> createContent(
		not_null<ContentMemento*> memento);

	rpl::producer<SelectedItems> selectedListValue() const;
	void refreshTopBarOverride(SelectedItems &&items);
	void createTopBarOverride(SelectedItems &&items);
	void destroyTopBarOverride();

	rpl::variable<Wrap> _wrap;
	object_ptr<ContentWidget> _content = { nullptr };
	object_ptr<Ui::PlainShadow> _topTabsBackground = { nullptr };
	object_ptr<Ui::SettingsSlider> _topTabs = { nullptr };
	object_ptr<TopBar> _topBar = { nullptr };
	object_ptr<TopBarOverride> _topBarOverride = { nullptr };
	Animation _topBarOverrideAnimation;
	object_ptr<Ui::FadeShadow> _topShadow;
	Tab _tab = Tab::Profile;
	std::unique_ptr<ContentMemento> _anotherTabMemento;
	std::vector<StackItem> _historyStack;

	rpl::event_stream<rpl::producer<int>> _desiredHeights;
	rpl::event_stream<rpl::producer<bool>> _desiredShadowVisibilities;
	rpl::event_stream<rpl::producer<SelectedItems>> _selectedLists;

};

} // namespace Info
