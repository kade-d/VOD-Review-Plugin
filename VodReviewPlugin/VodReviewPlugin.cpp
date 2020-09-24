#include "pch.h"
#include "VodReviewPlugin.h"
#include "bakkesmod\wrappers\GameEvent\TutorialWrapper.h"
#include "bakkesmod\wrappers\GameObject\BallWrapper.h"
#include "bakkesmod\wrappers\GameObject\CarWrapper.h"
#include <math.h>   

using std::vector;
using std::string;

std::vector<Vector> playLineLocations = {};
bool isDrawingPlayLines = false;

bool isShowingBoost = false;

bool isShowingCrosshair = false;
int crosshairWidth = 2;
int crosshairLength = 8;


BAKKESMOD_PLUGIN(VodReviewPlugin, "Review RL Replays", plugin_version, PLUGINTYPE_REPLAY)

void VodReviewPlugin::onLoad()
{
	cvarManager->log("Plugin loaded!");

	cvarManager->registerNotifier("draw_line", std::bind(&VodReviewPlugin::OnDrawPlayLine, this, std::placeholders::_1), "Starts/stops drawing play line", PERMISSION_REPLAY);
	cvarManager->registerNotifier("clear_line", std::bind(&VodReviewPlugin::OnClearPlayLine, this, std::placeholders::_1), "Clears the play line", PERMISSION_REPLAY);
	cvarManager->registerNotifier("show_player_boost", std::bind(&VodReviewPlugin::OnDrawPlayerBoost, this, std::placeholders::_1), "Shows player boost amounts", PERMISSION_REPLAY);
	cvarManager->registerNotifier("show_crosshair", std::bind(&VodReviewPlugin::OnShowCrosshair, this, std::placeholders::_1), "Shows a crosshair for drawing play lines", PERMISSION_REPLAY);
	cvarManager->executeCommand("bind B \"show_player_boost\"");
	cvarManager->executeCommand("bind X \"show_crosshair\"");
	cvarManager->executeCommand("bind V \"draw_line\"");
	cvarManager->executeCommand("bind C \"clear_line\"");

	gameWrapper->RegisterDrawable(bind(&VodReviewPlugin::Render, this, std::placeholders::_1));

}

void VodReviewPlugin::Render(CanvasWrapper canvas) {
	DrawPlayLine(canvas);
	if (isShowingBoost) {
		DrawPlayerBoost(canvas);
	}
	if (isShowingCrosshair) {
		ShowCrosshair(canvas);
	}

}

void VodReviewPlugin::OnDrawPlayerBoost(vector<string> params) {
	isShowingBoost = !isShowingBoost;
}

void VodReviewPlugin::DrawPlayerBoost(CanvasWrapper canvas) {

	ReplayServerWrapper replayGame = gameWrapper->GetGameEventAsReplay();
	if (!replayGame.IsNull()) {
		ArrayWrapper<CarWrapper> cars = replayGame.GetCars();
		Vector carLocations[8];

		canvas.SetColor(255, 255, 255, 255);
		for (int i = 0; i < cars.Count(); i++) {
			carLocations[i] = cars.Get(i).GetLocation();
			canvas.SetPosition(canvas.Project(carLocations[i]));
			if (!cars.Get(i).GetBoostComponent().IsNull()) {
				int boostAmount = static_cast<int>(cars.Get(i).GetBoostComponent().GetCurrentBoostAmount() * 100);
				canvas.DrawString(std::to_string(boostAmount), 1.5, 1.5);
			}
		}
	}
}

void VodReviewPlugin::OnDrawPlayLine(vector<string> params)
{
	isDrawingPlayLines = !isDrawingPlayLines;
}

void VodReviewPlugin::DrawPlayLine(CanvasWrapper canvas) {

	CameraWrapper camera = gameWrapper->GetCamera();
	ReplayServerWrapper replay = gameWrapper->GetGameEventAsReplay();
	if (!replay.IsNull() && !camera.IsNull()) {
		if (isDrawingPlayLines) {
			Vector cameraPos = camera.GetLocation();
			Vector cameraRot = RotatorToVector(camera.GetRotation());
			Vector planeNormal = Vector{ 0, 0, 1 };
			Vector planePoint = Vector{ 0, 0, 50 };
			Vector diff = cameraPos - planePoint;
			double prod1 = Vector().dot(diff, planeNormal);
			double prod2 = Vector().dot(cameraRot, planeNormal);
			double prod3 = prod1 / prod2;
			Vector groundCoord = cameraPos - cameraRot * prod3;

			playLineLocations.push_back(groundCoord);
		}
		canvas.SetColor(255, 255, 0, 255);
		for (int i = 0; i < playLineLocations.size(); i++) {
			canvas.SetPosition(canvas.Project(playLineLocations[i]));
			canvas.FillBox(Vector2{ 5, 5 });
		}
	}
}

void VodReviewPlugin::OnClearPlayLine(vector<string> params) {
	playLineLocations.clear();
}

void VodReviewPlugin::OnShowCrosshair(vector<string> params) {
	isShowingCrosshair = !isShowingCrosshair;
}

void VodReviewPlugin::ShowCrosshair(CanvasWrapper canvas) {

	canvas.SetColor(255, 255, 255, 255);

	Vector2 center = canvas.GetSize() / 2.0;

	Vector2 top = Vector2{ center.X,  center.Y + crosshairLength };
	Vector2 bottom = Vector2{ center.X,  center.Y - crosshairLength };
	canvas.DrawLine(bottom, top, 2);

	Vector2 right = Vector2{ center.X + crosshairLength,  center.Y };
	Vector2 left = Vector2{ center.X - crosshairLength,  center.Y };


	canvas.DrawLine(left, right, crosshairWidth);
}

void VodReviewPlugin::onUnload() {

}