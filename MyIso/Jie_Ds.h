#pragma once
#include "afxtempl.h"
#include <Eigen/Dense>
#include<iomanip>
#include"Vec_nD.h"
#include<vector>
#include<string>
#include<fstream>
#include <sstream>
#include<cstdio>
#include<ctime>
#include<iostream>
using namespace std;
using Eigen::MatrixXd;
#define thea pow(10,-3)
#define random(x) rand()%(x)
const int maxn = 1e4 + 20;
struct point
{
	float x, y;
	point() {}
	point(float xx, float yy)
		:x(xx), y(yy) {}
};

struct HwIsoPt
{
	double x; //横坐标
	double y; //纵坐标
	double z; // 属性值
};
struct HwIsoLine // 等值线线结构
{
	double zValue; // 等值线属性值
	HwIsoPt* pts; // 等值线平面坐标
	int ptn; //坐标个数
};
struct TWOVALUE
{
	double X;
	double Y;
};
struct THRVALUE
{
	double X;
	double Y;
	double Z;
};
//等值线数据体
struct IsoLine
{
	TWOVALUE direction0;	//歼灭井位置，从歼灭井到非歼灭井  20140313
	TWOVALUE direction1;	//非歼灭井，从歼灭井到非歼灭井  20140313
	int  Index;				//跟direction0，direction1对应的点在等值线中的位置（用于等值区域符号填充及尖灭方向判断）20130314
	double Value;			//等值线的属性值
	vector<TWOVALUE> Logic; //等值线的逻辑坐标
};
struct Dis //此数据结构代表什么
{
	double dis;//长度
	double per;//百分比
	double angle;//角度
};
struct Fault
{
	int		    m_nfault;					//Fault线的唯一标志	 断层组编号
	BOOL		m_bUpOrDown;				//为true,是上升盘，为false，为下降盘
	bool        m_InverseFlag;              //正逆断层标志。正断层:false;逆断层和垂直断层:true
	bool        m_Mid;                      //是否有中间线
	int         m_InfaultNum;				//断层线组中的编号的线编号

	vector<TWOVALUE> m_pts; // 断层线坐标
};
struct FaultPoint
{
	int sign;					//断层序号
	int	mark;					//正断层还是逆断层  正断层为1 逆断层为0
	vector<TWOVALUE> UData;		//上升盘（upthrown block）
	vector<TWOVALUE> DData;		//下降盘（downthrown side）
	vector<THRVALUE> TData;		//中间线
	vector<double>  Gap;        //落差
								//逆断层 上升盘 上升  + 则下降盘下降 -
								//正断层 上升盘 -  下降盘+


								//根据文章，此处的上升盘应该是上盘···下降盘应该是下盘  、、20130903
								//http://www.doc88.com/p-86415001780.html   陈士亮
};
struct CFpoint
{
	double x;
	double y;
};
struct SingleFalut
{
	//vector<TWOVALUE> FLine;	//断层线数据
	vector<THRVALUE> FLine;		//断层线数据 20140915
};
class Data
{
public:
	Data(double x = 0, double y = 0, double z = 0) { X = x; Y = y; Z = z; mark = 0; };
	double X;
	double Y;
	double Z;
	int mark;//判断正断层还是逆断层  正断层为1 逆断层为0
	void Init(double x = 0, double y = 0, double z = 0) { X = x; Y = y; Z = z; mark = 0; }

};
struct FalutLine
{
	vector<TWOVALUE> FLine;			//断层线数据
	bool IsClose;                 	//是否是闭合断层，TURE为闭合
	bool IsNormal;			  		//是否为正断层，TURE为正断层
	int  IsClock;					//是否为顺时针，1为顺时针，-1为逆时针，对结合后的断层，使用函数判断不是很正确20140722
};
struct GridFL						//GridFaultLine		
{
	vector<SingleFalut> FLines;	//断层线数据
};
struct FLOValue					//网格中单条断层线的属性 20140724	
{
	double Star;				//首点所在网格序号
	double End;					//末点所在网格序号
	SingleFalut FLines;			//断层线数据
};
class Jie_Ds
{
public:
	vector<Data> m_oriData; //原始数据，井位信息；输入数据
							//vector<FaultPoint> m_FaultData; //断层信息20130814
	vector<vector<THRVALUE>> m_GridPoint;//生成的网格
	int m_XNum; //横向网格数量
	int m_YNum; //纵向网格数量
	int m_DIV; //等值线间隔
	double m_XMin; //x方向最小值
	double m_XMax;  //X方向最大值
	double m_YMin; //y方向最小值
	double m_YMax; //Y方向最大值
	double m_ZMin; //属性最小值
	double m_ZMax; //属性最大值
	double exp;//凸包扩充参数
	double m_Show_MinValue;
	double m_Show_MaxValue;
	double m_Show_JianGeValue;
	vector<Data> m_Border;
	vector<Data> m_OriBoder;					//原始的真实边界（可能是凹断层）20140801
	bool m_IsConvex;							//原始的边界是否为凸包20140801
	bool m_IsK;								//是否使用克里金估值 20150323

	bool m_bUseFault;
	double m_valuedis;//等值线间隔 20140724
	double m_B;		 //直线型变异函数B值20140728
	vector<Data> m_suV;
	vector<double> m_ni;
	vector<IsoLine>m_IsoLine;
	//存储所有的等值线,每条等值线存储实际值，主要是等值点在断层上不好使用虚拟存储
	vector<IsoLine>m_IsoRealLine;
	vector<IsoLine>opt_IsoRealLine;
	vector<double> m_TrackValue;				//要寻找的等值线值
	CArray<double,double> m_sameArray;
	vector<HwIsoLine> m_lsoLines;
	vector<IsoLine> Jie_IsoLine;
	vector<TWOVALUE>Jie_OriBoder;
	vector<IsoLine>Jie_IsoRealLine;
	vector<double> Jie_RNum;
	vector<Vector2D> originPoint;
	vector<Vector2D> curvePoint;
	vector<point> input_vertice;
	vector<point> control_point;
	vector<point> b_spline;
	vector<Fault> m_faults;
	CArray<int, int> m_faultNunArray;// 记录断层组编号。
	vector<FaultPoint> m_FaultData;
	CArray<CFpoint, CFpoint> m_TepFPointArray;    //临时变量，生成断层线时
	vector<FalutLine> m_vecFaultLines;			//用于追踪，存储所有的断层线信息
	vector<vector<int>> m_NoFault;				//判断每个网格内是否含有断层，有则存储断层线的序号
	vector<vector<BOOL>> m_XFault;				//判断每个X网格线是否含有断层,用于初始等值点的估计。断层两边的等值点暂时不做计算
	vector<vector<BOOL>> m_YFault;				//判断每个Y网格线是否含有断层
	vector<vector<SingleFalut>> m_XFaultPts;	//存储每个X网格线是断层数据20140916
	vector<vector<SingleFalut>> m_YFaultPts;	//存储每个Y网格线是断层数据20140916
	vector<vector<BOOL>> m_GridInFault;			//存储网格点是否在断层内   20140916
	vector<GridFL> m_vecGridFL;					//存储网格内的断层信息
	vector<vector<THRVALUE>> m_GridFault;		//测试每个网格内跟等值点组成的图形对不对
public:
	void ReadFaultStr(CString strFault);
	void SetFault(char* strFaultData);
	void IsFault_ABNull(int group, int &m_flag, bool &InverseFlag);
	void Fault_A(vector<TWOVALUE>& m_FaultA, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag);
	void Fault_B(vector<TWOVALUE>& m_FaultB, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag);
	void Fault_C(vector<TWOVALUE>& m_FaultB, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag);
	void CalcFault();
	bool IsOnePT(TWOVALUE PtA, TWOVALUE PtB);
	int GerEquNum(vector<TWOVALUE> PtA, vector<TWOVALUE>  PtB);
	int  GetCrossNum(FaultPoint LineI, FaultPoint LineJ);
	TWOVALUE GerNearPt(TWOVALUE PtA, TWOVALUE PtB, double Di);
	TWOVALUE AddPt(FaultPoint &Line, int IsU, int IsS);
	void AddPt(FaultPoint &Line, int IsU, int IsS, TWOVALUE AssistPt);
	void AddPt(FaultPoint &LineI, FaultPoint &LineJ, int IU, int IS, int JU, int JS);
	BOOL IsSEinterse(FaultPoint &LineI, FaultPoint &LineJ);
	void Combination(vector<FaultPoint>&Lines);
	void CloseFalut();
	void GetABC(TWOVALUE Star, TWOVALUE End, TWOVALUE P, double &A, double &B, double &C);
	bool L2L_Intersect(THRVALUE p1_s, THRVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e);
	bool IsIntersect(TWOVALUE p1_s, TWOVALUE p1_e, vector<THRVALUE> Lines);
	void DealWithSingleFault();
	void GetMiddleData();
	void GetNearByPoint(const TWOVALUE& m_PrePoint, const TWOVALUE& m_NextPoint, const TWOVALUE& point, Data &m_newPoint);
	double GetPointToLineDis(TWOVALUE PrePoint, TWOVALUE NextPoint, TWOVALUE point);
	double GetDistance(vector<TWOVALUE> Pts, TWOVALUE A);
	void SetDIF(double fDis);
	void TidyFaultLine(FalutLine &LineI);
	bool IsClockwise(const TWOVALUE &p0, const TWOVALUE &p1, const TWOVALUE &p2);
	FalutLine ConnectLine(FalutLine &LineI, FalutLine LineJ, int IP[], int JP[]);
	BOOL Isintersect(FalutLine &LineI, FalutLine LineJ);
	void Combination();
	void GetFaultLine(vector<FaultPoint> data);
	void EvaluateFault();
	void NormalVector(vector<THRVALUE>& a, THRVALUE& b, double Position);
	void ReviseWires(int Num, vector<vector<double>>& faultSu, double Position);
	void FaultWires(int Num, vector<Data>& oriData, vector<THRVALUE>& faultSu, double Position);
	void FaultKriking();
	void ReviseWires(int Num, THRVALUE& Point, double Position);
	void EvaluateFault(THRVALUE& Point);
	void FaultInvDis();
	void GetValueByK(THRVALUE &Point);
	BOOL ISInside(TWOVALUE A, TWOVALUE GridIJ[5]);
	void Sort(vector<double>&Mark);
	void GetOneCross(int IndexI, int IndexJ, TWOVALUE GridIJ[5], FalutLine LineOne, GridFL &AllLine);
	void GetFulatInsidePoint(vector<double>Mark, FalutLine LineOne, GridFL &AllLine, TWOVALUE GridIJ[5]);
	void GetAllCross(int IndexI, int IndexJ, TWOVALUE GridIJ[5], GridFL &AllLine);
	void SetOneFault(int IndexI, int IndexJ, GridFL &ALLLine);
	BOOL IsInside(TWOVALUE A, FalutLine Line);
	BOOL IsInside(TWOVALUE RealyPoint);
	void JudgeFaultGridIn();
	void SetNewFault();
	void GetMinD(vector<double> CosS, vector<double> DisT, int &index);
	vector<Data> Withershins(vector<Data> m_point);
	vector<Data> LoadModel(const char* sFileName);
	void Charact();
	void DataOpt();
	void DataRec();
	void CalcBorder();
	void AddData(Data &t);
	double GetDis(double x1, double y1, double x2, double y2);
	void AddPt(vector<Data> &convexBag);
	double Angle(Data &p0, const Data &p1, const Data &p2);
	double bezier3funcX(double uu, Vector2D *controlP);
	double bezier3funcY(double uu, Vector2D *controlP);
	void createCurve();
	void deCasteljau();
	void deBoor();
	void OptimizeBoder(vector<Data> &convexBag,double e);
	void OptimizeBorderBezier(vector<Data> &convexBag,double e);
	void DividedFourParts(int n, vector<int>& FourPort);
	void BordersPointDis(vector<int>& FourPart, vector<Data>& convexBag, vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
		vector<Dis>& upDis, vector<Dis>&downDis, vector<Dis>&leftDis, vector<Dis>&rightDis);
	void BordersChar(vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
		vector<Dis>& upDis, vector<Dis>&downdis, vector<Dis>&leftDis, vector<Dis>&rightDis);
	void BordersPoints(vector<int>& FourPart, vector<Data>& convexBag, vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
		vector<Dis>& upDis, vector<Dis>&downdis, vector<Dis>&leftDis, vector<Dis>&rightDis,
		vector<Data>& DL, vector<Data>& DR, vector<Data>&DD, vector<Data>& DU,
		vector<TWOVALUE>& sim1, vector<TWOVALUE>& sim2, vector<TWOVALUE>& sim3, vector<TWOVALUE>& sim4, int XM, int YM);
	bool Inv(vector<vector<double>>&M);
	double InsertEst(vector<Data>& suV, TWOVALUE& D, vector<double>& ni);
	void PreMatrix(vector<Data>& suV, vector<double>& ni);
	double DisInv(TWOVALUE D);
	void EvaluateNoFault();
	void SetGridXY(double exp);
	double GetMagnitude(double fNumber);
	float FindStep(float StepMin, bool bUporDown);
	void CalcSameArray();
	void SetTrackValue(vector<double> Track);
	double GetIndex(THRVALUE Star, THRVALUE End, THRVALUE Point);
	bool IsinLineK(THRVALUE Star, THRVALUE End, THRVALUE Point);
	bool IsinLineK(THRVALUE Star, THRVALUE End, TWOVALUE Point);
	vector<FLOValue> GetFalutindex(THRVALUE GridIJ[6], GridFL Lines);
	void GetCrossMe(THRVALUE GridIJ[6], vector<FLOValue> &FLIndex, double &Star, vector<THRVALUE> &FlautArae);
	vector<THRVALUE> GetFalutAera(THRVALUE GridIJ[6], GridFL Lines);
	BOOL GetVirtualValue(THRVALUE GridIJ[6], GridFL Lines, TWOVALUE &Point);
	BOOL IsOnGrid(THRVALUE GridIJ[6], TWOVALUE PT);
	void GetT(THRVALUE A, THRVALUE B, THRVALUE C, double &t);
	void GetPtPs(THRVALUE GridIJ[6], THRVALUE Point, int &Index);
	void TrackRightFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackLeftFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackDownFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackUPFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackRight(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackLeft(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackDown(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackUp(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void GetReallyPoint(TWOVALUE A, TWOVALUE &B);
	void TrackY(TWOVALUE A, double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	void TrackX(TWOVALUE A, double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used);
	//void TrackPoint(double Value, IsoLine &Line, IsoLine &IsoReal, double flag_x[][1000], double flag_y[][1000], TWOVALUE First);
	void TrackPoint(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used, TWOVALUE First);
	void GetMinDis(THRVALUE A, vector<THRVALUE>Pts, THRVALUE &Point);
	void SignBorder(vector<vector<double>>& biaoji1, vector<vector<double>>& biaoji2);
	void Y_FalutEquivalent(int X_index, int Y_index, double Value, vector<TWOVALUE>&VirtualIJ);
	void X_FalutEquivalent(int X_index, int Y_index, double Value, vector<TWOVALUE>&VirtualIJ);
	void EquivalentPoints(double Value, vector<TWOVALUE>&VirtualIJ);
	void EquivalentPoints(double Value, vector<THRVALUE>&VirtualIJ);
	void TrackOneValue(double Value);
	void IsolineTracking();
	void IsolineOpt();
	void CreateIsoLine();
	void SetOriBoder(vector<TWOVALUE> OriBoder);
	void SetOriISOLine(vector<IsoLine> IsoRealLine);
	bool L2L_Intersect(TWOVALUE p1_s, TWOVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e, TWOVALUE &Point);
	bool GetCrossPt(TWOVALUE Star, TWOVALUE End, vector<TWOVALUE> Line, TWOVALUE &A);
	bool ISIntersect(TWOVALUE p1_s, TWOVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e, double &index);
	bool IsinLineK(TWOVALUE Star, TWOVALUE End, TWOVALUE Point);
	bool IsInside(TWOVALUE A, vector<TWOVALUE> Line);
	void DleaIso(IsoLine &OneIso, vector<IsoLine> &NewIso);
	void DleaIso();
	vector<double> Randnum(int k);
};
