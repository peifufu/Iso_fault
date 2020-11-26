#include"Jie_Ds.h"
#include"Vec_nD.h"
#include<iostream>
using namespace std;
int main()
{
	class Jie_Ds Jie_test;
	vector<Data> Convexhull, ConvexhullTest,ConvexhullExp;
	//读入井位点信息
	Convexhull = Jie_test.LoadModel("data//fault_test//data1.txt");
	std::ofstream out1("data//fault_test//point_ori.obj");
	for (int j = 0; j < Convexhull.size(); j++)
	{
		Data qq;
		qq = Convexhull[j];
		out1 << "v " << fixed << setprecision(5)<<qq.X << " " << fixed << setprecision(5)<<qq.Y << " " << "0" << endl;
	}
	out1.close();
	//计算凸包(凸包计算完成后，凸包上面的点会沿着顺时针、逆时针两种分布，此处是逆时针)
	ConvexhullTest = Jie_test.Withershins(Convexhull);
	std::ofstream out("data//fault_test//convexhull_ori.obj");
	for (int j = 0; j < ConvexhullTest.size(); j++)
	{
		Data qq;
		qq = ConvexhullTest[j];
		out << "v " << fixed << setprecision(5)<<qq.X << " " << fixed << setprecision(5)<<qq.Y << " " << "0" << endl;
	}
	int count_ori = 1;
	for (int j = 0; j < ConvexhullTest.size() - 1; j++)
	{
		Data qq1, qq2;
		qq1 = ConvexhullTest[j];
		qq2 = ConvexhullTest[j + 1];
		out << "l " << count_ori << " " << count_ori + 1 << endl;
		count_ori++;
	}
	out.close();

	double Sum_x=0,Sum_y=0,Aver_x=0, Aver_y=0;
	for (int j = 0; j < ConvexhullTest.size()-1; j++)
	{
		Sum_x += ConvexhullTest[j].X;
		Sum_y += ConvexhullTest[j].Y;
	}
	Aver_x = Sum_x / (ConvexhullTest.size()-1);
	Aver_y = Sum_y / (ConvexhullTest.size()-1);
	std::ofstream out9("center.obj");
	out9 << "v " << fixed << setprecision(5) << Aver_x << " " << fixed << setprecision(5) << Aver_y << " " << "0" << endl;
	out9.close();



	for (int i = 0; i < ConvexhullTest.size(); i++)
	{
		Data cp;
		cp.X = ConvexhullTest[i].X;
		cp.Y = ConvexhullTest[i].Y;
		double d_v_x = ConvexhullTest[i].X - Aver_x;
		double d_v_y = ConvexhullTest[i].Y - Aver_y;
		double d_n_v_x = d_v_x / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		double d_n_v_y = d_v_y / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		cp.X += 3* d_n_v_x;
		cp.Y += 3* d_n_v_y;
		ConvexhullExp.push_back(cp);
	}

	std::ofstream out12("convexhull_exp.obj");
	for (int j = 0; j < ConvexhullExp.size(); j++)
	{
		Data qq;
		qq = ConvexhullExp[j];
		out12 << "v " << fixed << setprecision(5) << qq.X << " " << fixed << setprecision(5) << qq.Y << " " << "0" << endl;
	}
	int count_exp = 1;
	for (int j = 0; j < ConvexhullExp.size() - 1; j++)
	{;
		out12 << "l " << count_exp << " " << count_exp + 1 << endl;
		count_exp++;
	}
	out12.close();



	
	//cout << "hello world!" << endl;
	cout << "before oridata:" << Jie_test.m_oriData.size() << endl;
	cout << "before border:"<<Jie_test.m_Border.size() << endl;
	cout << "before oriborder:" << Jie_test.m_OriBoder.size() << endl;
	cout << "before grid:"<<Jie_test.m_GridPoint.size() << endl;
	
	Jie_test.SetFault("0,0,1,0,20654708.563&4195954.781;20654889.250&4195911.516;20655024.063&4195883.531;20655176.750&4195847.906;20655428.688&4195809.734;20655609.375&4195786.828;20655868.875&4195763.938;20656059.750&4195753.750;20656240.375&4195753.750;20656413.438&4195763.938;20656538.125&4195769.016;20656728.938&4195791.922;#0,1,0,0,20654390.563&4194183.734;20654580.000&4194296.891;20654700.563&4194373.156;20654853.063&4194483.859;20655130.938&4194659.578;20655400.688&4194817.344;20655581.375&4194914.047;20655744.188&4195003.109;20655917.250&4195081.984;20656115.688&4195165.953;20656278.563&4195239.750;20656436.313&4195303.359;20656594.125&4195369.516;20656746.750&4195420.406;20656909.625&4195489.109;20657059.750&4195550.188;20657217.500&4195611.250;20657347.313&4195667.234;20657544.375&4195758.172;20658092.938&4196117.344;#0,2,1,0,20654968.125&4193601.031;20655075.000&4193669.734;20655209.875&4193740.984;20655349.813&4193837.688;20655484.688&4193921.656;20655614.438&4194000.531;20655734.000&4194084.516;20655874.000&4194186.297;20656013.938&4194300.797;20656115.688&4194379.672;20656240.375&4194478.922;20656385.438&4194603.609;20656530.500&4194738.469;20656599.188&4194807.172;20656706.063&4194896.234;20656774.750&4194952.203;20656863.813&4195038.719;20656965.625&4195127.781;20657082.625&4195244.844;20657217.500&4195369.516;20657396.750&4195529.391;20657795.313&4195795.078;20658065.875&4195937.750;#0,1,1,0,20654417.625&4194210.797;20655003.125&4194641.297;20655357.375&4194847.953;20655726.375&4195037.375;20655868.875&4195117.609;20656047.000&4195211.750;20656227.688&4195303.359;20656380.375&4195382.250;20656515.188&4195448.406;20656650.063&4195494.203;20656807.813&4195555.281;20656970.688&4195606.172;20657151.375&4195667.234;20657273.500&4195707.953;20657403.250&4195753.750;20657490.250&4195795.078;20657554.188&4195827.047;20657603.375&4195856.578;20657679.688&4195898.391;20657755.938&4195947.594;20657849.375&4196001.719;20658100.500&4196130.344;#0,2,0,0,20654983.438&4193588.406;20655158.938&4193662.359;20655441.375&4193825.219;20655688.250&4193988.078;20655896.875&4194140.750;20656296.375&4194423.203;20656575.125&4194660.984;20656798.938&4194845.500;#0,2,0,1,20656843.250&4194892.234;20657016.500&4195054.000;20657123.375&4195137.969;20657240.438&4195239.750;20657388.000&4195374.609;20657510.125&4195499.297;20657639.938&4195611.250;20657701.000&4195662.141;20657783.000&4195718.813;20657849.375&4195772.938;20657918.313&4195817.219;20658065.875&4195920.531;#0,3,1,0,20656843.250&4194892.234;20656924.438&4194685.578;20657005.625&4194439.578;20657026.688&4194389.859;20657095.375&4194232.094;20657194.625&4194064.156;20657268.438&4193934.375;20657393.125&4193753.719;20657459.250&4193674.828;20657512.375&4193617.922;#0,3,0,0,20656794.063&4194843.031;20656870.313&4194678.203;20656941.625&4194471.563;20657008.875&4194288.078;20657123.375&4194079.422;20657212.438&4193944.563;20657286.250&4193837.688;20657375.313&4193730.813;20657505.000&4193605.625;#0,0,0,0,20654718.750&4195901.344;20654899.438&4195858.078;20655034.250&4195830.094;20655186.938&4195794.469;20655438.875&4195756.297;20655619.563&4195733.391;20655879.063&4195710.500;20656069.938&4195700.313;20656250.563&4195700.313;20656423.625&4195710.500;20656548.313&4195715.578;20656739.125&4195738.484;#");
	//Jie_test.SetFault("0,0,1,0,20654708.563&4195954.781;20654889.250&4195911.516;20655024.063&4195883.531;20655176.750&4195847.906;20655428.688&4195809.734;20655609.375&4195786.828;20655868.875&4195763.938;20656059.750&4195753.750;20656240.375&4195753.750;20656413.438&4195763.938;20656538.125&4195769.016;20656728.938&4195791.922;#0,0,0,0,20654718.750&4195901.344;20654899.438&4195858.078;20655034.250&4195830.094;20655186.938&4195794.469;20655438.875&4195756.297;20655619.563&4195733.391;20655879.063&4195710.500;20656069.938&4195700.313;20656250.563&4195700.313;20656423.625&4195710.500;20656548.313&4195715.578;20656739.125&4195738.484;#");
	//Jie_test.SetFault("0,1,0,0,20654390.563&4194183.734;20654580.000&4194296.891;20654700.563&4194373.156;20654853.063&4194483.859;20655130.938&4194659.578;20655400.688&4194817.344;20655581.375&4194914.047;20655744.188&4195003.109;20655917.250&4195081.984;20656115.688&4195165.953;20656278.563&4195239.750;20656436.313&4195303.359;20656594.125&4195369.516;20656746.750&4195420.406;20656909.625&4195489.109;20657059.750&4195550.188;20657217.500&4195611.250;20657347.313&4195667.234;20657544.375&4195758.172;20658092.938&4196117.344;#0,1,1,0,20654417.625&4194210.797;20655003.125&4194641.297;20655357.375&4194847.953;20655726.375&4195037.375;20655868.875&4195117.609;20656047.000&4195211.750;20656227.688&4195303.359;20656380.375&4195382.250;20656515.188&4195448.406;20656650.063&4195494.203;20656807.813&4195555.281;20656970.688&4195606.172;20657151.375&4195667.234;20657273.500&4195707.953;20657403.250&4195753.750;20657490.250&4195795.078;20657554.188&4195827.047;20657603.375&4195856.578;20657679.688&4195898.391;20657755.938&4195947.594;20657849.375&4196001.719;20658100.500&4196130.344;#");
	//Jie_test.SetFault("0,0,1,0,1&3;2&3;3&3;4&3;5&3;6&3;7&3;;#0,0,0,0,1&2;2&2;3&2;4&2;5&2;6&2;7&2;;#0,1,1,0,3&2.5;3&2;3&1;3&0;3&-1;3&-2;3&-3;#0,1,0,0,,4&2.5;4&2;4&1;4&0;4&-1;4&-2;4&-3;#0,2,1,0,6&2.5;6&2;6&1;6&0;#0,2,1,1,6&-1;6&-2;6&-3;#0,2,0,0,7&2;7&1;7&0;#");
	//Jie_test.SetFault("2,0,0,0,-5&0;-4&0;-3&0;-2&0;-1&0;0&0;1&0;2&0;3&0;4&0;5&0;#");
	//Jie_test.SetFault("2,0,0,0,-5&-5;-4&-4;-3&-3;-2&-2;-1&-1;0&0;1&1;2&2;3&3;4&4;5&5;#");
	//Jie_test.SetFault("0,0,1,0,0.8&2;0.8&1.9;0.8&1.8;0.8&1.7;0.8&1.6;0.8&1.5;0.8&1.4;0.8&1.3;0.8&1.2;0.8&1.1;0.8&1.0;0.8&0.9;0.8&0.8;0.8&0.7;0.8&0.6;0.8&0.5;0.8&0.4;0.8&0.3;0.8&0.2;0.8&0.1;0.8&0.0;0.8&-0.1;0.8&-0.2;0.8&-0.3;0.8&-0.4;0.8&-0.5;0.8&-0.6;0.8&-0.7;0.8&-0.8;0.8&-0.9;0.8&-1.0;0.8&-1.1;0.8&-1.2;0.8&-1.3;0.8&-1.4;0.8&-1.5;0.8&-1.6;0.8&-1.7;0.8&-1.8;0.8&-1.9;0.8&-2.0;#0,0,0,0,0.6&2;0.6&1.9;0.6&1.8;0.6&1.7;0.6&1.6;0.6&1.5;0.6&1.4;0.6&1.3;0.6&1.2;0.6&1.1;0.6&1.0;0.6&0.9;0.6&0.8;0.6&0.7;0.6&0.6;0.6&0.5;0.6&0.4;0.6&0.3;0.6&0.2;0.6&0.1;0.6&0.0;0.6&-0.1;0.6&-0.2;0.6&-0.3;0.6&-0.4;0.6&-0.5;0.6&-0.6;0.6&-0.7;0.6&-0.8;0.6&-0.9;0.6&-1.0;0.6&-1.1;0.6&-1.2;0.6&-1.3;0.6&-1.4;0.6&-1.5;0.6&-1.6;0.6&-1.7;0.6&-1.8;0.6&-1.9;0.6&-2.0;#");
	Jie_test.CalcFault();
	cout << "断层组的数量："<<Jie_test.m_faultNunArray.GetCount() << endl;
	cout << "断层线的数量："<<Jie_test.m_faults.size() << endl;
	//cout << Jie_test.m_FaultData.size() << endl;
	std::ofstream out_FaultPU("data//fault//FaultPointUp.obj");
	std::ofstream out_FaultPM("data//fault//FaultPointMid.obj");
	std::ofstream out_FaultPD("data//fault//FaultPointDown.obj");
	for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
	{
		for (int j = 0;j < Jie_test.m_FaultData[i].UData.size(); j++)
			out_FaultPU << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].Y << " " << "0" << endl;
		for (int j = 0; j < Jie_test.m_FaultData[i].TData.size(); j++)
			out_FaultPM << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].Y << " " << "0" << endl;
		for (int j = 0; j < Jie_test.m_FaultData[i].DData.size(); j++)
			out_FaultPD << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].Y << " " << "0" << endl;
	}
	out_FaultPU.close();
	out_FaultPM.close();
	out_FaultPD.close();


	//cout << "下面显示的是各个断层的类型：" << endl;
	//for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
	//	cout << Jie_test.m_FaultData[i].mark << " ";
	//cout << endl;


	for (int i = 0; i < Convexhull.size(); i++)
		Jie_test.AddData(Convexhull[i]);

	Jie_test.Charact();
	//Jie_test.CalcBorder();
	//cout << Jie_test.m_Border.size() << endl;
	Jie_test.m_XNum = 200;
	Jie_test.m_YNum = 200;
	Jie_test.SetGridXY(1000);
	std::ofstream out5("convexhull_border.obj");
	for (int i = 0; i < Jie_test.m_Border.size(); i++)
	{
		out5 << "v " << fixed << setprecision(5) << Jie_test.m_Border[i].X << " " << fixed << setprecision(5) << Jie_test.m_Border[i].Y << " " << "0" << endl;
	}
	int count_border = 1;
	for (int i = 0; i < Jie_test.m_Border.size() - 1; i++)
	{
		out5 << "l " << count_border << " " << count_border + 1 << endl;
		count_border += 1;
	}
	out5.close();

	//插值之前网格点的属性值
	/*for (int i = 1; i < Jie_test.m_GridPoint.size() - 2; i++)
	{
	for (int j = 1; j < Jie_test.m_GridPoint.size() - 2; j++)
	{
		cout << Jie_test.m_GridPoint[i][j].Z <<" ";
	}
	cout << endl;
	}*/
	//如果断层点不为空，要使用有断层的估值方式
	if (Jie_test.m_FaultData.size() > 0)
	{
		//接下来首先要对断层进行一些预处理操作
		Jie_test.GetMiddleData();

		std::ofstream out_FaultPT("data//fault//FaultPointT.obj");
		for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
		{
			for (int j = 0; j < Jie_test.m_FaultData[i].TData.size(); j++)
				out_FaultPT << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].Y << " " << "0" << endl;
		}
		out_FaultPT.close();

		Jie_test.Combination(Jie_test.m_FaultData);

		std::ofstream out_FaultPU("data//fault//FaultPointUp_Combination.obj");
		std::ofstream out_FaultPD("data//fault//FaultPointDown_Combination.obj");
		for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
		{
			for (int j = 0; j < Jie_test.m_FaultData[i].UData.size(); j++)
				out_FaultPU << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].Y << " " << "0" << endl;
			for (int j = 0; j < Jie_test.m_FaultData[i].DData.size(); j++)
				out_FaultPD << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].Y << " " << "0" << endl;
		}
		out_FaultPU.close();
		out_FaultPD.close();

		Jie_test.CloseFalut();

		std::ofstream out_FaultPU1("data//fault//FaultPointUp_CloseFalut.obj");
		std::ofstream out_FaultPD1("data//fault//FaultPointDown_CloseFalut.obj");
		for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
		{
			for (int j = 0; j < Jie_test.m_FaultData[i].UData.size(); j++)
				out_FaultPU1 << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].Y << " " << "0" << endl;
			for (int j = 0; j < Jie_test.m_FaultData[i].DData.size(); j++)
				out_FaultPD1 << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].Y << " " << "0" << endl;
		}
		out_FaultPU1.close();
		out_FaultPD1.close();

		Jie_test.DealWithSingleFault();

		std::ofstream out_FaultPU2("data//fault//FaultPointUp_DealWith.obj");
		std::ofstream out_FaultPD2("data//fault//FaultPointDown_DealWith.obj");
		for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
		{
			for (int j = 0; j < Jie_test.m_FaultData[i].UData.size(); j++)
				out_FaultPU2 << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].UData[j].Y << " " << "0" << endl;
			for (int j = 0; j < Jie_test.m_FaultData[i].DData.size(); j++)
				out_FaultPD2 << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].DData[j].Y << " " << "0" << endl;
		}
		out_FaultPU2.close();
		out_FaultPD2.close();

		

		Jie_test.SetDIF(1);
		//cout << Jie_test.m_valuedis << endl;
		std::ofstream out_FaultPT1("data//fault//FaultPoint_Dif.obj");
		for (int i = 0; i < Jie_test.m_FaultData.size(); i++)
		{
			for (int j = 0; j < Jie_test.m_FaultData[i].TData.size(); j++)
				out_FaultPT1 << "v " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].X << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].Y << " " << fixed << setprecision(5) << Jie_test.m_FaultData[i].TData[j].Z << endl;
		}
		out_FaultPT1.close();

		Jie_test.GetFaultLine(Jie_test.m_FaultData);

		//cout << Jie_test.m_vecFaultLines.size() << endl;
		std::ofstream out_FaultL("data//fault//FaultLine.obj");
		for (int i = 0; i < Jie_test.m_vecFaultLines.size(); i++)
			for (int j = 0; j < Jie_test.m_vecFaultLines[i].FLine.size(); j++)
				out_FaultL << "v " << fixed << setprecision(5) << Jie_test.m_vecFaultLines[i].FLine[j].X << " " << fixed << setprecision(5) << Jie_test.m_vecFaultLines[i].FLine[j].Y << " " << "0" << endl;
		out_FaultL.close();

		//cout << Jie_test.m_vecFaultLines[0].IsClock << endl;
		//cout << Jie_test.m_vecFaultLines[0].IsClose << endl;
		//cout << Jie_test.m_vecFaultLines[0].IsNormal << endl;

		//以上对于断层的预处理已经完成，接下来就是要进行带断层的估计方法
		Jie_test.EvaluateFault();

		//估计完成后，需要计算网格内部的断层线
		Jie_test.SetNewFault();
	}
	//使用无断层的估值方式
	else
	{
		Jie_test.EvaluateNoFault();
	}

	cout << "after oridata:" << Jie_test.m_oriData.size() << endl;
	cout << "after border:" << Jie_test.m_Border.size() << endl;
	cout << "sfter oriborder:" << Jie_test.m_OriBoder.size() << endl;
	cout << "after grid:"<<Jie_test.m_GridPoint.size() << endl;


	//cout << "v " << Jie_test.m_GridPoint[1][1].X << " " << Jie_test.m_GridPoint[1][1].Y << " " << "0" << endl;
	//cout << "v " << Jie_test.m_GridPoint[1][2].X << " " << Jie_test.m_GridPoint[1][2].Y << " " << "0" << endl;
	//std::ofstream out_gaocheng("data//gc.obj");
	std::ofstream out2("data//fault_test//Grid.obj");
	int count_grid = 1;
	// 按照Grid.h中的计算，横纵行列都是从1开始,最后两行/列也是附加的，都是0,0,0;不要了
	// Grid.h中的m_GridPoint，一级下标对应的是网格的列数据，二级下标对应的是行数据
	//cout << "v " << fixed << setprecision(10) << Jie_test.m_GridPoint[1][1].X << " "<< Jie_test.m_GridPoint[1][1].Y<<" "<< "0" <<endl;
	//cout << "v " << fixed << setprecision(10) << Jie_test.m_GridPoint[1][2].X << " " << Jie_test.m_GridPoint[1][2].Y << " " << "0" << endl;
	//cout << "v " << fixed << setprecision(10) << Jie_test.m_GridPoint[2][1].X << " " << Jie_test.m_GridPoint[2][1].Y << " " << "0" << endl;
	for (int i = 1; i < Jie_test.m_GridPoint.size()-2; i++)
	{
		//cout<< fixed << setprecision(10) << Jie_test.m_GridPoint[i][1].X << " " << Jie_test.m_GridPoint[i][1].Y << " " << "0" << endl;
		for (int j = 1; j < Jie_test.m_GridPoint.size()-2; j++)
		{
			//out_gaocheng << fixed << setprecision(10) << Jie_test.m_GridPoint[i][j].Z << endl;
			out2 << "v " << fixed << setprecision(10)<<Jie_test.m_GridPoint[i][j].X << " " << fixed << setprecision(10) << Jie_test.m_GridPoint[i][j].Y << " " << "0" << endl;
			//cout << Jie_test.m_GridPoint[i][j].X << " " << Jie_test.m_GridPoint[i][j].Y << " " << Jie_test.m_GridPoint[i][j].Z << endl;
		}
	}
	//out_gaocheng.close();
	int current = Jie_test.m_GridPoint.size() - 3;
	int flag;
	for (int i = 1; i < Jie_test.m_GridPoint.size() - 2; i++)
	{
		flag = 1;
		int j = (i-1)*current+1;
		while (flag < current)
		{
			out2 << "l " << j << " " << j + 1 << endl;
			j += 1;
			flag++;
		}
	}

	
	for (int i = 1; i < Jie_test.m_GridPoint.size() - 2; i++)
	{
		flag = 1;
		int j = i;
		while (flag < current)
		{
			out2 << "l " << j << " " << j + current << endl;
			j += current;
			flag++;
		}
	}
	out2.close();

	//下面是测试查看下网格内的断层线的输出效果
	std::ofstream out_GL("data//fault_test//fault_GL.obj");
	for (int i = 0; i < Jie_test.m_vecGridFL.size(); i++)
	{
		//cout << Jie_test.m_vecGridFL[i].FLines.size() << " ";
		for (int j = 0; j < Jie_test.m_vecGridFL[i].FLines.size(); j++)
		{
			for (int k = 0; k < Jie_test.m_vecGridFL[i].FLines[j].FLine.size(); k++)
				out_GL << "v " << fixed << setprecision(5) << Jie_test.m_vecGridFL[i].FLines[j].FLine[k].X << " " << fixed << setprecision(5) << Jie_test.m_vecGridFL[i].FLines[j].FLine[k].Y << " " << "0" << endl;
		}
	}
	out_GL.close();

	std::ofstream out_z("data//fault//z.obj");
	//插值之后网格点的属性值
	for (int i = 1; i < Jie_test.m_GridPoint.size() - 2; i++)
	{
		for (int j = 1; j < Jie_test.m_GridPoint.size() - 2; j++)
		{
			//cout << Jie_test.m_GridPoint[i][j].Z<<" " ;
			out_z << Jie_test.m_GridPoint[i][j].Z << endl;
		}
		//cout << endl;
	}
	out_z.close();
	Jie_test.m_Show_MinValue = Jie_test.m_ZMin;//数据点属性值的最小值
	Jie_test.m_Show_MaxValue = Jie_test.m_ZMax;//数据点属性值的最大值
	Jie_test.m_Show_JianGeValue = abs(Jie_test.m_Show_MaxValue - Jie_test.m_Show_MinValue)*0.2;//这里应该是等值线的间隔
	cout << endl;
	cout << endl;
	cout << Jie_test.m_Show_MaxValue << endl;
	cout << Jie_test.m_Show_MinValue << endl;
	cout << Jie_test.m_Show_JianGeValue << endl;
	//Jie_test.CalcSameArray();
	//cout << Jie_test.m_Show_MaxValue << endl;
	//cout << Jie_test.m_Show_MinValue << endl;
	//cout << Jie_test.m_Show_JianGeValue << endl;
	vector<double> vd;
	double m_nDenseNum = 1;
	double dvalue = (Jie_test.m_Show_JianGeValue) / (m_nDenseNum + 1);
	double dmax = (Jie_test.m_Show_MaxValue);
	double dmin = (Jie_test.m_Show_MinValue);

	while (dmin< dmax)
	{
		//cout << "dmin: " << dmin << " ";
		vd.push_back(dmin);
		dmin += dvalue;
	}
	//cout << vd.size() << endl;
	Jie_test.SetTrackValue(vd);
	Jie_test.IsolineTracking();
	

	//Jie_test.CreateIsoLine();
	//cout << Jie_test.m_IsoLine.size() << endl;
	//cout << Jie_test.m_lsoLines.size() << endl;

	cout << "======================================================" << endl;
	cout << "以下是关于等值线的一些信息" << endl;
	cout << "等值线的数量：" << Jie_test.m_IsoRealLine.size() << endl;
	cout << "各条等值线的高程值：" << endl;
	for (int i = 0; i < Jie_test.m_IsoRealLine.size(); i++)
	{
		cout << "value = :" << Jie_test.m_IsoRealLine[i].Value << endl;
	}
	//std::cout << Jie_test.Jie_IsoLine.size() << endl;
	std::ofstream out6("isoline_m_IsoLine.obj");
	for (int i = 0; i < Jie_test.m_IsoLine.size(); i++)
	{
		for (int j = 0; j < Jie_test.m_IsoLine[i].Logic.size(); j++)
		{
			out6 << "v " << Jie_test.m_IsoLine[i].Logic[j].X << " " << Jie_test.m_IsoLine[i].Logic[j].Y << " " << "0" << endl;
		}
	}
	out6.close();
	std::ofstream out7("data//fault_test//isoline.obj");
	std::ofstream out10("startp.obj");
	std::ofstream out11("endp.obj");
	int count_realline = 1;
	for (int i = 0; i < Jie_test.m_IsoRealLine.size(); i++)
	{
		//cout << Jie_test.m_IsoRealLine[i].Value << " ";
		out10 << "v " << Jie_test.m_IsoRealLine[i].Logic[0].X << " " << Jie_test.m_IsoRealLine[i].Logic[0].Y << " " << "0" << endl;
		out11 << "v " << Jie_test.m_IsoRealLine[i].Logic[Jie_test.m_IsoRealLine[i].Logic.size()-1].X << " " << Jie_test.m_IsoRealLine[i].Logic[Jie_test.m_IsoRealLine[i].Logic.size()-1].Y << " " << "0" << endl;
		for (int j = 0; j < Jie_test.m_IsoRealLine[i].Logic.size(); j++)
		{
			out7 << "v " << fixed << setprecision(10) << Jie_test.m_IsoRealLine[i].Logic[j].X << " " << fixed << setprecision(10) << Jie_test.m_IsoRealLine[i].Logic[j].Y << " " << "0" << endl;
		}
		for (int j = 0; j < Jie_test.m_IsoRealLine[i].Logic.size() - 1; j++)
		{
			out7 << "l " << count_realline << " " << count_realline + 1 << endl;
			count_realline++;
		}
		count_realline++;
	}
	out7.close();
	out10.close();
	out11.close();
	Jie_test.IsolineOpt();
















	std::ofstream out3("Equi_point.obj");
	for (int i = 0; i < Jie_test.Jie_IsoLine.size(); i++)
	{
		for (int j = 0; j < Jie_test.Jie_IsoLine[i].Logic.size(); j++)
		{	
			out3 << "v " << fixed << setprecision(10) << Jie_test.Jie_IsoLine[i].Logic[j].X << " " << fixed << setprecision(10) << Jie_test.Jie_IsoLine[i].Logic[j].Y <<" "<<"0"<< endl;
		}
	}
	out3.close();



	//cout << "m_XMin: " << fixed << setprecision(5) << Jie_test.m_XMin << "m_YMin: " << fixed << setprecision(5) << Jie_test.m_YMin << endl;














	cout << endl;
	cout << "=======================================================" << endl;
	cout << "=======================================================" << endl;
	cout << "下面进行的是边界对于等值线的切割操作" << endl;
	vector<TWOVALUE> Jie_border;
	for (int j = 0; j < Convexhull.size(); j++)
	{
		Data qq;
		qq = Convexhull[j];
		TWOVALUE pp;
		pp.X = qq.X;
		pp.Y = qq.Y;
		Jie_border.push_back(pp);
	}
	
	Jie_test.SetOriBoder(Jie_border);
	Jie_test.SetOriISOLine(Jie_test.m_IsoRealLine);

	Jie_test.DleaIso();

	std::ofstream out8("aftercut_isoline.obj");
	for (int i = 0; i < Jie_test.Jie_IsoRealLine.size(); i++)
	{
		for (int j = 0; j < Jie_test.Jie_IsoRealLine[i].Logic.size(); j++)
			out8 << "v " << Jie_test.Jie_IsoRealLine[i].Logic[j].X << " " << Jie_test.Jie_IsoRealLine[i].Logic[j].Y << " " << "0" << endl;
	}
	out8.close();







	getchar();
	return 0;
}