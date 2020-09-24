#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"

constexpr auto plugin_version = "1.0.0";

class VodReviewPlugin: public BakkesMod::Plugin::BakkesModPlugin
{

	virtual void onLoad();
	virtual void onUnload();
	void Render(CanvasWrapper canvas);
	void OnDrawPlayerBoost(std::vector<std::string> params);
	void DrawPlayerBoost(CanvasWrapper canvas);
	void OnDrawPlayLine(std::vector<std::string> params);
	void DrawPlayLine(CanvasWrapper canvas);
	void OnClearPlayLine(std::vector<std::string> params);
	void OnShowCrosshair(std::vector<std::string> params);
	void ShowCrosshair(CanvasWrapper canvas);
	std::string menuTitle_ = "VOD Review Plugin";

};

