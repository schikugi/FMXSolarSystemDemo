//---------------------------------------------------------------------------

#include <fmx.h>
#include <cmath>
#pragma hdrstop

#include "uSolarSystemForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.fmx"
TSolarSystemForm *SolarSystemForm;

// 地球の公転半径
const float TSolarSystemForm::RADIUS = 8.0;

// 地球の公転間隔（度）
const float TSolarSystemForm::STEP_REVOLUTION_EARTH = 360.0f / 365.0f / 4.0f;
// 地球の自転間隔（度）
const float TSolarSystemForm::STEP_ROTATION_EARTH = -360.0f / 48.0;
// 太陽の自転間隔（度）
const float TSolarSystemForm::STEP_ROTATION_SUN = -360.f / 27.0 / 10.0;

// 地球の公転間隔（1年＝40000ミリ秒で周期）
const int   TSolarSystemForm::INTERVAL_REVOLUTION_EARTH = (120000.0f / 365.0f / 4.0f  + 0.5);
// 地球の自転間隔（30分周期）
const int   TSolarSystemForm::INTERVAL_ROTATION_EARTH = (120000.0f / 365.0f / 48.0f + 0.5f);
// 太陽の自転間隔（27日）
const int   TSolarSystemForm::INTERVAL_ROTATION_SUN = (120000.0f / 27.0 / 10.0 + 0.5f);

//---------------------------------------------------------------------------
__fastcall TSolarSystemForm::TSolarSystemForm(TComponent* Owner)
	: TForm(Owner),
	EarthPositionX(RADIUS),
	EarthPositionY(0.0),
	EarthRevolutionAngle(0.0),
	EarthRotationAngle(0.0),
    SunRotationAngle(0.0),
	Suspended(true),
	Terminated(false)
{
	// 座標の初期化
	this->SphereEarth->Position->X = EarthPositionX;
	this->SphereEarth->Position->Z = EarthPositionY;

	// 地球を公転させるスレッドを登録＆実行
	RevolveEarthThread = std::thread(TSolarSystemForm::RevolveEarth);
	// 地球を自転させるスレッドを登録＆実行
	RotateEarthThread = std::thread(TSolarSystemForm::RotateEarth);
	// 太陽を自転させるスレッドを登録＆実行
	RotateSunThread  = std::thread(TSolarSystemForm::RotateSun);
}
//---------------------------------------------------------------------------

void __fastcall TSolarSystemForm::Timer1Timer(TObject *Sender)
{
	// 各オブジェクトの座標・回転角を設定
	this->SphereEarth->Position->X = EarthPositionX;
	this->SphereEarth->Position->Z = EarthPositionY;
	this->SphereEarth->RotationAngle->Y = EarthRotationAngle;
	this->SphereSun->RotationAngle->Y = SunRotationAngle;


    // 再描画
	this->Invalidate();
}
//---------------------------------------------------------------------------

// 角度計算
void TSolarSystemForm::AddAngle(float& Angle, float diff)
{
	Angle += diff;
	if (Angle >= 360.0)  {
		Angle -= 360.0;
	} else if (Angle <= -360.0)  {
		Angle += 360.0;
	}
}
//---------------------------------------------------------------------------

void TSolarSystemForm::RevolveEarth()
{
	// スレッドが実行中である間ループを回す
	while (!SolarSystemForm->Terminated)  {

		// 一時停止中か？
		if (!SolarSystemForm->Suspended) {

			// 地球の方向角を計算
			AddAngle(SolarSystemForm->EarthRevolutionAngle, STEP_REVOLUTION_EARTH);

			float radian = SolarSystemForm->EarthRevolutionAngle * M_PI / 180.0;
			SolarSystemForm->EarthPositionX = TSolarSystemForm::RADIUS * std::cos(radian);
			SolarSystemForm->EarthPositionY = TSolarSystemForm::RADIUS * std::sin(radian);
		}

		// 次の計算タイミング待ち
		std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_REVOLUTION_EARTH));
	}
}

//---------------------------------------------------------------------------
void TSolarSystemForm::RotateEarth()
{
	// スレッドが実行中である間ループを回す
	while (!SolarSystemForm->Terminated)  {

		// 一時停止中か？
		if (!SolarSystemForm->Suspended) {
			// 回転角計算
			AddAngle(SolarSystemForm->EarthRotationAngle, STEP_ROTATION_EARTH);
		}
		// 次の計算タイミング待ち
		std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_ROTATION_EARTH));
	}
}

//---------------------------------------------------------------------------
void TSolarSystemForm::RotateSun()
{

	// スレッドが実行中である間ループを回す
	while (!SolarSystemForm->Terminated)  {

		// 一時停止中か？
		if (!SolarSystemForm->Suspended) {
			// 回転角計算
			AddAngle(SolarSystemForm->SunRotationAngle, STEP_ROTATION_SUN);
		}

		// 次の計算タイミング待ち
		std::this_thread::sleep_for(std::chrono::milliseconds(INTERVAL_ROTATION_SUN));
	}
}
//---------------------------------------------------------------------------

void __fastcall TSolarSystemForm::FormDestroy(TObject *Sender)
{
	this->Terminated = true;
	this->Suspended = true;

	// スレッドを停止させる
	RevolveEarthThread.detach();
	RotateEarthThread.detach();
	RotateSunThread.detach();
}
//---------------------------------------------------------------------------

void __fastcall TSolarSystemForm::SphereSunDblClick(TObject *Sender)
{
	if (Suspended) {
		this->Timer1->Enabled = true;
		this->Suspended = false;
	} else {
		this->Timer1->Enabled = false;
		this->Suspended = true;
	}
}

//---------------------------------------------------------------------------
void __fastcall TSolarSystemForm::ViewSpaceGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
		  bool &Handled)
{
	switch (EventInfo.GestureID) {
	case sgiScratchout: 	// 取り消し (マウスを左右に動かす)
			OnInit();
			break;
	case sgiLeft:       	// マウスを左方向に動かす
			OnRotateY(10);
			break;
	case sgiRight:      	// マウスを右方向に動かす
			OnRotateY(-10);
			break;
	case sgiUp:         	// マウスを上方向に動かす
			OnRotateX(10);
			break;
	case sgiDown:       	// マウスを下方向に動かす
			OnRotateX(-10);
			break;
	case sgiSemiCircleLeft: // マウスを反時計回りに回す
			OnRotateZ(10);
			break;
	case sgiSemiCircleRight:// マウスを時計回りに回す
			OnRotateZ(-10);
			break;
	default:
		;
	}
}

//---------------------------------------------------------------------------

void __fastcall TSolarSystemForm::OnRotateX(int Direction)
{
	float a = this->SolarSystem->RotationAngle->X;
	this->SolarSystem->RotationAngle->X = a + Direction;
}

//---------------------------------------------------------------------------
void __fastcall TSolarSystemForm::OnRotateY(int Direction)
{
	float a = this->SolarSystem->RotationAngle->Y;
	this->SolarSystem->RotationAngle->Y = a + Direction;

}
//---------------------------------------------------------------------------

void __fastcall TSolarSystemForm::OnRotateZ(int Direction)
{
	float a = this->SolarSystem->RotationAngle->Z;
	this->SolarSystem->RotationAngle->Z = a + Direction;
}

//---------------------------------------------------------------------------
void __fastcall TSolarSystemForm::OnInit()
{
	this->SolarSystem->RotationAngle->X = 0.0;
	this->SolarSystem->RotationAngle->Y = 0.0;
	this->SolarSystem->RotationAngle->Z = 0.0;
}



