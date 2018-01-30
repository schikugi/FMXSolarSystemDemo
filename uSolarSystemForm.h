//---------------------------------------------------------------------------

#ifndef uSolarSystemFormH
#define uSolarSystemFormH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <FMX.Controls.hpp>
#include <FMX.Forms.hpp>
#include <FMX.Controls3D.hpp>
#include <FMX.MaterialSources.hpp>
#include <FMX.Objects3D.hpp>
#include <FMX.Types.hpp>
#include <FMX.Viewport3D.hpp>
#include <System.Math.Vectors.hpp>
#include <FMX.Objects.hpp>
#include <FMX.Gestures.hpp>
#include <thread>
#include <atomic>

//---------------------------------------------------------------------------
class TSolarSystemForm : public TForm
{
__published:	// IDE で管理されるコンポーネント
	TViewport3D *ViewSpace;
	TTextureMaterialSource *TextureSun;
	TImage *ImageSpace;
	TSphere *SphereSun;
	TSphere *SphereEarth;
	TTextureMaterialSource *TextureEarth;
	TTimer *Timer1;
	TGestureManager *GestureManager1;
	TDummy *SolarSystem;
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall SphereSunDblClick(TObject *Sender);
	void __fastcall ViewSpaceGesture(TObject *Sender, const TGestureEventInfo &EventInfo,
          bool &Handled);

private:	// ユーザー宣言
	std::thread RevolveEarthThread; // 地球を公転させるスレッド
	std::thread RotateEarthThread;  // 地球を自転させるスレッド
	std::thread RotateSunThread;  	// 太陽を自転させるスレッド

	bool  Terminated;         		// スレッドが終了しているか？
	bool  Suspended;          		// 一時停止中か？
	float EarthRevolutionAngle;   	// 太陽と地球の方向角（度）
	float EarthPositionX;           // 地球の座標
	float EarthPositionY;           // 地球の座標
	float EarthRotationAngle;       // 地球の回転角
	float SunRotationAngle;         // 太陽の回転角

	void __fastcall OnInit();
	void __fastcall OnRotateX(int Direction);
	void __fastcall OnRotateY(int Direction);
	void __fastcall OnRotateZ(int Direction);

public:		// ユーザー宣言
	__fastcall TSolarSystemForm(TComponent* Owner);

private:
    static const float RADIUS;                      // 公転半径

	static const float STEP_REVOLUTION_EARTH;     	// 地球の公転間隔（度）
	static const float STEP_ROTATION_EARTH;       	// 地球の自転間隔（度）
	static const float STEP_ROTATION_SUN;         	// 太陽の自転間隔（度）

	static const int   INTERVAL_REVOLUTION_EARTH; 	// 地球の公転間隔（ミリ秒）
	static const int   INTERVAL_ROTATION_EARTH;   	// 地球の自転間隔（ミリ秒）
	static const int   INTERVAL_ROTATION_SUN;     	// 太陽の自転間隔（ミリ秒）

	static void RevolveEarth(); 					// 地球を公転させる
	static void RotateEarth();  					// 地球を自転させる
	static void RotateSun();  						// 太陽を自転させる
	static void AddAngle(float& Angle, float diff); // 角度計算
};
//---------------------------------------------------------------------------
extern PACKAGE TSolarSystemForm *SolarSystemForm;
//---------------------------------------------------------------------------
#endif
