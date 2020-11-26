#include "Jie_Ds.h"


void Jie_Ds::ReadFaultStr(CString strFault)
{
	Fault faultObj;
	BOOL bHave = FALSE;
	int pos = 0;
	CString str = _T("");
	str = strFault.Tokenize(",", pos);
	if (str != "")
	{//断层分类（正断层/逆断层）
	//	faultObj.m_InverseFlag = (atoi(str) != 0);
		int flag = atoi(str);
		if (flag == 1)
			faultObj.m_InverseFlag = false;
		else if (flag == 0)
			faultObj.m_InverseFlag = true;
		else
			faultObj.m_Mid = true;
	}
	else
		return;
	str = strFault.Tokenize(",", pos);
	if (str != "")
	{//断层组编号（0、1、2...）
		faultObj.m_nfault = atol(str);

		for (int i = 0; i < m_faultNunArray.GetCount(); i++) // 遍历查看是否有当前组号，没有就记录下。
		{
			if (m_faultNunArray[i] == faultObj.m_nfault)
			{
				bHave = TRUE;
				break;
			}
		}
	}
	else
		return;
	str = strFault.Tokenize(",", pos);
	if (str != "")
	{//线位(上升盘、下降盘)
		faultObj.m_bUpOrDown = atol(str);
	}
	else
		return;
	str = strFault.Tokenize(",", pos);
	if (str != "")
	{//线编号
		faultObj.m_InfaultNum = atol(str);
	}
	else
		return;

	str = strFault.Tokenize(",", pos);
	if (str != "")
	{//x&y;x&y;....
		CStringArray xStrArray;
		CStringArray yStrArray;
		xStrArray.RemoveAll();
		yStrArray.RemoveAll();
		CString xStr, yStr;
		int pos1 = 0;
		xStr = str.Tokenize("&;", pos1);
		if (xStr == "")
		{
			return;
		}
		yStr = str.Tokenize("&;", pos1);
		if (yStr == "")
		{
			return;
		}
		while (xStr != "")
		{
			xStrArray.Add(xStr);
			yStrArray.Add(yStr);
			xStr = str.Tokenize("&;", pos1);
			if (xStr == "")
			{
				break;
			}
			yStr = str.Tokenize("&;", pos1);
			if (yStr == "")
			{
				break;
			}
		}
		if (xStrArray.GetCount()>0)
		{
			int nSize = (int)xStrArray.GetCount();
			for (int i = 0; i<nSize; i++)
			{
				TWOVALUE pt;
				pt.X = (float)atof(xStrArray.GetAt(i));
				pt.Y = (float)atof(yStrArray.GetAt(i));
				faultObj.m_pts.push_back(pt);
				//m_txLine.m_Pts[i].x = fx - Offset_X;
				//m_txLine.m_Pts[i].y = fy - Offset_Y;
			}
		}
		else
			return;
	}
	else
		return;

	if (!bHave)
		m_faultNunArray.Add(faultObj.m_nfault);

	m_faults.push_back(faultObj);
}
void Jie_Ds::SetFault(char* strFaultData)
{
	//strFaultData格式：正逆断层，断层组号，上升盘下降盘，断层线号，x&y;x&y;....x&y;#.....重复
	CString strData = strFaultData;
	CString strOne = "", strTmp = "";
	int n = 0, pos = 0;

	m_faultNunArray.RemoveAll();

	strOne = strData.Tokenize("#", n);
	strOne.Trim();
	while (!strOne.IsEmpty())
	{
		ReadFaultStr(strOne);

		strOne = strData.Tokenize("#", n);
		strOne.Trim();
	}
}
void Jie_Ds::IsFault_ABNull(int group, int &m_flag, bool &InverseFlag)
{
	//返回值为1：上升盘为空 2：下降盘为空 3：均不为空
	InverseFlag = false;
	BOOL m_bUp = FALSE;
	BOOL m_bDown = FALSE;
	BOOL m_bMid = FALSE;
	//对所有的断层线进行遍历
	int faultline_count = m_faults.size();
	for (int i = 0; i<faultline_count; i++)
	{
		Fault m_TmpFault = m_faults[i];
		if (m_TmpFault.m_nfault == group && m_TmpFault.m_Mid!=TRUE && m_TmpFault.m_bUpOrDown == TRUE)
		{
			m_bUp = TRUE;//上升盘不为空
			InverseFlag = m_TmpFault.m_InverseFlag;//正逆断层标志。正断层:false;逆断层和垂直断层:true
		}
		else if (m_TmpFault.m_nfault == group && m_TmpFault.m_Mid != TRUE && m_TmpFault.m_bUpOrDown == FALSE)
		{
			m_bDown = TRUE;//下降盘不为空
			InverseFlag = m_TmpFault.m_InverseFlag;//正逆断层标志。正断层:false;逆断层和垂直断层:true
		}
		else if (m_TmpFault.m_nfault == group && m_TmpFault.m_Mid == TRUE)
		{
			m_bMid = TRUE;//中间线不为空
			InverseFlag = true;
			//InverseFlag = m_TmpFault.m_InverseFlag;//正逆断层标志。正断层:false;逆断层和垂直断层:true
		}
		
	}
	//只有中间断层线
	if (m_bMid == true)
	{
		m_flag = 4;
	}
	else
	{
		if (!m_bUp)
		{
			m_flag = 1;
		}
		else if (!m_bDown)
		{
			m_flag = 2;
		}
		else
			m_flag = 3;
	}
}
//得到上断层线
void Jie_Ds::Fault_A(vector<TWOVALUE>& m_FaultA, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag)
{
	CUIntArray m_templineNO;
	m_templineNO.RemoveAll();

	//组内的线统计
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		//上升盘
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum && m_TmpFault.m_bUpOrDown == TRUE)
		{
			int m_temNO = m_TmpFault.m_InfaultNum;//断层线组中的编号的线编号
												  //统计线号 
			int num = 0;
			for (; num<(int)m_templineNO.GetSize(); num++)
			{
				if (m_templineNO.GetAt(num) == m_temNO)
					break;
			}
			if (num == (int)m_templineNO.GetSize())
				m_templineNO.Add(m_temNO);
		}
	}

	//线号排序
	//冒泡排序
	for (int i = 0; i<(int)m_templineNO.GetSize(); i++)
	{
		for (int k = (int)m_templineNO.GetSize() - 1; k>i; k--)
		{
			if (m_templineNO[k] < m_templineNO[k - 1])
			{
				int n = m_templineNO[k];
				//注意这里下面这两个函数的使用，可以用小例子测试
				m_templineNO.RemoveAt(k);
				m_templineNO.InsertAt(k - 1, n);
			}
		}
	}

	//取数
	m_TepFPointArray.RemoveAll();
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		//上升盘
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum &&m_TmpFault.m_Mid != TRUE && m_TmpFault.m_bUpOrDown == TRUE)
		{
			for (int k = 0; k != (int)m_templineNO.GetSize(); k++)
			{
				if (m_TmpFault.m_InfaultNum == (int)m_templineNO.GetAt(k))
				{
					//if (!m_flag) //有下降盘时
					{
						//抽稀断层点						
						//m_TepFPointArray.RemoveAll();
						CFpoint m_T;
						for (int j = 0; j<m_TmpFault.m_pts.size(); j++)
						{
							//抽稀点
							if (m_TepFPointArray.GetSize() == 0)//第一个点不用判断，直接加入
							{
								CFpoint m_tFP;
								m_tFP.x = m_TmpFault.m_pts[j].X;
								m_tFP.y = m_TmpFault.m_pts[j].Y;
								m_TepFPointArray.Add(m_tFP);
								m_T = m_tFP;
							}
							else
							{
								float m_dis = (float)_hypot(m_TmpFault.m_pts[j].X - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].x, m_TmpFault.m_pts[j].Y - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].y);
								if (m_dis>0.00f) // 原判断值是20.00f，会筛选落下某些数据，导致后面小层平面图断层效果计算时出错。不明白为何加此判断。
								{
									CFpoint m_tFP;
									m_tFP.x = m_TmpFault.m_pts[j].X;
									m_tFP.y = m_TmpFault.m_pts[j].Y;
									m_TepFPointArray.Add(m_tFP);
								}
							}

						}

					}
				}
			}
		}
	}


	//m_FaultA.Resize((int)m_TepFPointArray.GetSize());
	for (int j = 0; j<(int)(m_TepFPointArray.GetSize()); j++)
	{
		TWOVALUE pt;

		pt.X = m_TepFPointArray[j].x;
		pt.Y = m_TepFPointArray[j].y;
		m_FaultA.push_back(pt);
	}
}
//得到下断层线
void Jie_Ds::Fault_B(vector<TWOVALUE>& m_FaultB, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag)
{
	CUIntArray m_templineNO;
	m_templineNO.RemoveAll();

	//int m_ptnGroup = 0;//记录该族中所有下降盘断层点的和
	BOOL m_b = FALSE;	//标记下降盘是否为空
						//组内的线统计
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum &&m_TmpFault.m_Mid!=TRUE && m_TmpFault.m_bUpOrDown == FALSE)
		{
			int m_temNO = m_TmpFault.m_InfaultNum;
			//统计线号 
			int num = 0;
			for (; num<(int)m_templineNO.GetSize(); num++)
			{
				if (m_templineNO.GetAt(num) == m_temNO)
					break;
			}
			if (num == (int)m_templineNO.GetSize())
			{
				m_templineNO.Add(m_temNO);//保存线编号
										  //m_ptnGroup += m_TmpFault;
			}
		}
	}

	//线号排序
	//冒泡排序
	for (int i = 0; i<(int)m_templineNO.GetSize(); i++)
	{
		for (int k = (int)m_templineNO.GetSize() - 1; k>i; k--)
		{
			if (m_templineNO[k] < m_templineNO[k - 1])
			{
				int n = m_templineNO[k];
				m_templineNO.RemoveAt(k);
				m_templineNO.InsertAt(k - 1, n);
			}
		}
	}

	//取数
	m_TepFPointArray.RemoveAll();
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum && m_TmpFault.m_bUpOrDown == FALSE)
		{
			for (int k = 0; k != (int)m_templineNO.GetSize(); k++)
			{
				if (m_TmpFault.m_InfaultNum == (int)m_templineNO.GetAt(k))
				{

					for (int j = 0; j<m_TmpFault.m_pts.size(); j++)
					{
						//抽稀点
						if (m_TepFPointArray.GetSize() == 0)
						{
							CFpoint m_tFP;
							m_tFP.x = m_TmpFault.m_pts[j].X;
							m_tFP.y = m_TmpFault.m_pts[j].Y;
							m_TepFPointArray.Add(m_tFP);
						}
						else
						{
							float m_dis = (m_TmpFault.m_pts[j].X - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].x)*(m_TmpFault.m_pts[j].X - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].x) +
								(m_TmpFault.m_pts[j].Y - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].y)*(m_TmpFault.m_pts[j].Y - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].y);
							if (m_dis>0.00f) // 原判断值是100.00f，会筛选落下某些数据，导致后面小层平面图断层效果计算时出错。不明白为何加此判断。
							{
								CFpoint m_tFP;
								m_tFP.x = m_TmpFault.m_pts[j].X;
								m_tFP.y = m_TmpFault.m_pts[j].Y;
								m_TepFPointArray.Add(m_tFP);
							}
						}

					}

				}//if
			}//for
		}//if	
	}//for

	 //m_FaultB.Resize((int)m_TepFPointArray.GetSize());
	for (int j = 0; j<(int)(m_TepFPointArray.GetSize()); j++)
	{
		TWOVALUE pt;
		pt.X = m_TepFPointArray[j].x;
		pt.Y = m_TepFPointArray[j].y;
		m_FaultB.push_back(pt);
	}
}

//得到中间线
void Jie_Ds::Fault_C(vector<TWOVALUE>& m_FaultC, int m_All_FaultLineNum, int m_CurFaultGroupNum, BOOL m_flag)
{
	CUIntArray m_templineNO;
	m_templineNO.RemoveAll();

	//int m_ptnGroup = 0;//记录该族中所有下降盘断层点的和
	BOOL m_b = FALSE;	//标记下降盘是否为空
						//组内的线统计
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum && m_TmpFault.m_Mid == TRUE)
		{
			int m_temNO = m_TmpFault.m_InfaultNum;
			//统计线号 
			int num = 0;
			for (; num<(int)m_templineNO.GetSize(); num++)
			{
				if (m_templineNO.GetAt(num) == m_temNO)
					break;
			}
			if (num == (int)m_templineNO.GetSize())
			{
				m_templineNO.Add(m_temNO);//保存线编号
										  //m_ptnGroup += m_TmpFault;
			}
		}
	}

	//线号排序
	//冒泡排序
	for (int i = 0; i<(int)m_templineNO.GetSize(); i++)
	{
		for (int k = (int)m_templineNO.GetSize() - 1; k>i; k--)
		{
			if (m_templineNO[k] < m_templineNO[k - 1])
			{
				int n = m_templineNO[k];
				m_templineNO.RemoveAt(k);
				m_templineNO.InsertAt(k - 1, n);
			}
		}
	}

	//取数
	m_TepFPointArray.RemoveAll();
	for (int i = 0; i<m_All_FaultLineNum; i++)
	{
		Fault m_TmpFault = m_faults[i];
		if (m_TmpFault.m_nfault == m_CurFaultGroupNum && m_TmpFault.m_bUpOrDown == FALSE)
		{
			for (int k = 0; k != (int)m_templineNO.GetSize(); k++)
			{
				if (m_TmpFault.m_InfaultNum == (int)m_templineNO.GetAt(k))
				{

					for (int j = 0; j<m_TmpFault.m_pts.size(); j++)
					{
						//抽稀点
						if (m_TepFPointArray.GetSize() == 0)
						{
							CFpoint m_tFP;
							m_tFP.x = m_TmpFault.m_pts[j].X;
							m_tFP.y = m_TmpFault.m_pts[j].Y;
							m_TepFPointArray.Add(m_tFP);
						}
						else
						{
							float m_dis = (m_TmpFault.m_pts[j].X - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].x)*(m_TmpFault.m_pts[j].X - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].x) +
								(m_TmpFault.m_pts[j].Y - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].y)*(m_TmpFault.m_pts[j].Y - m_TepFPointArray[m_TepFPointArray.GetSize() - 1].y);
							if (m_dis>0.00f) // 原判断值是100.00f，会筛选落下某些数据，导致后面小层平面图断层效果计算时出错。不明白为何加此判断。
							{
								CFpoint m_tFP;
								m_tFP.x = m_TmpFault.m_pts[j].X;
								m_tFP.y = m_TmpFault.m_pts[j].Y;
								m_TepFPointArray.Add(m_tFP);
							}
						}

					}

				}//if
			}//for
		}//if	
	}//for

	 //m_FaultB.Resize((int)m_TepFPointArray.GetSize());
	for (int j = 0; j<(int)(m_TepFPointArray.GetSize()); j++)
	{
		TWOVALUE pt;
		pt.X = m_TepFPointArray[j].x;
		pt.Y = m_TepFPointArray[j].y;
		m_FaultC.push_back(pt);
	}
}


void Jie_Ds::CalcFault()
{
	int faultline_count = m_faults.size();//断层线的数量
	int m_FaultGroupNum = m_faultNunArray.GetCount();//断层组的数量

	m_FaultData.resize(m_faultNunArray.GetCount());//角点网格断层数据清空

															 //统计上升盘数据
	for (int j = 0; j<(int)m_faultNunArray.GetSize(); j++)
	{//统计每一组的断层
		vector<TWOVALUE> m_FaultA, m_FaultB, m_FaultC;
		int m_flag = 3;
		bool m_InverseFlag = false;    //正逆断层标志。正断层:false;逆断层和垂直断层:true
		int m_num = m_faultNunArray.GetAt(j);//断层组号
		IsFault_ABNull(m_num, m_flag, m_InverseFlag); //判断该组上升盘、下降盘是否为空




		//统计一组中的上升盘、下降盘对应的断层线的坐标
		Fault_A(m_FaultA, faultline_count, m_faultNunArray.GetAt(j), FALSE);
		Fault_B(m_FaultB, faultline_count, m_faultNunArray.GetAt(j), FALSE);
		Fault_C(m_FaultC, faultline_count, m_faultNunArray.GetAt(j), FALSE);

		//cout << "flag = " << m_flag << endl;
		//初始化控件断层数组
		m_FaultData[j].mark = m_InverseFlag;

		if (m_flag == 2)//只有上升盘
		{
			m_FaultData[j].UData.resize((int)m_FaultA.size());
			//m_IsoGrid.m_FaultData[j].TData.resize((int)m_FaultA.size());
			//这里的断层中间线和上升盘对应的断层线重合
			for (int i = 0; i<(int)(m_FaultA.size()); i++)
			{
				m_FaultData[j].UData[i].X = m_FaultA[i].X;
				m_FaultData[j].UData[i].Y = m_FaultA[i].Y;
				//m_IsoGrid.m_FaultData[j].TData[i].X = m_FaultA[i].X;
				//m_IsoGrid.m_FaultData[j].TData[i].Y = m_FaultA[i].Y;
			}
			for (int index = 0; index < (int)m_FaultA.size(); index++)  // minZZBX之类的这些，可以用到确定border数据中。
			{
				//if (minZZBX > m_IsoGrid.m_FaultData[j].TData[index].X)
				//	minZZBX = m_IsoGrid.m_FaultData[j].TData[index].X;
				//if (maxZZBX < m_IsoGrid.m_FaultData[j].TData[index].X)
				//	maxZZBX = m_IsoGrid.m_FaultData[j].TData[index].X;
				//if (minHZBY > m_IsoGrid.m_FaultData[j].TData[index].Y)
				//	minHZBY = m_IsoGrid.m_FaultData[j].TData[index].Y;
				//if (maxHZBY < m_IsoGrid.m_FaultData[j].TData[index].Y)
				//	maxHZBY = m_IsoGrid.m_FaultData[j].TData[index].Y;
			}
			vector<TWOVALUE> va;
			m_FaultA.swap(va);
			m_FaultA.clear();
		}
		else if (m_flag == 1) //只有下降盘
		{
			m_FaultData[j].DData.resize((int)m_FaultB.size());
			//m_IsoGrid.m_FaultData[j].TData.resize((int)m_FaultB.size());
			for (int i = 0; i<(int)(m_FaultB.size()); i++)
			{
				m_FaultData[j].DData[i].X = m_FaultB[i].X;
				m_FaultData[j].DData[i].Y = m_FaultB[i].Y;
				//m_IsoGrid.m_FaultData[j].TData[i].X = m_FaultB[i].X;
				//m_IsoGrid.m_FaultData[j].TData[i].Y = m_FaultB[i].Y;
			}
			for (int index = 0; index < (int)m_FaultB.size(); index++)
			{
				//if (minZZBX > m_IsoGrid.m_FaultData[j].TData[index].X)
				//	minZZBX = m_IsoGrid.m_FaultData[j].TData[index].X;
				//if (maxZZBX < m_IsoGrid.m_FaultData[j].TData[index].X)
				//	maxZZBX = m_IsoGrid.m_FaultData[j].TData[index].X;
				//if (minHZBY > m_IsoGrid.m_FaultData[j].TData[index].Y)
				//	minHZBY = m_IsoGrid.m_FaultData[j].TData[index].Y;
				//if (maxHZBY < m_IsoGrid.m_FaultData[j].TData[index].Y)
				//	maxHZBY = m_IsoGrid.m_FaultData[j].TData[index].Y;
			}
			vector<TWOVALUE> vb;
			m_FaultB.swap(vb);
			m_FaultB.clear();
		}
		else if (m_flag == 4)//只有中间线
		{
			m_FaultData[j].TData.resize((int)m_FaultC.size());
			//m_IsoGrid.m_FaultData[j].TData.resize((int)m_FaultB.size());
			for (int i = 0; i<(int)(m_FaultC.size()); i++)
			{
				m_FaultData[j].TData[i].X = m_FaultC[i].X;
				m_FaultData[j].TData[i].Y = m_FaultC[i].Y;
				//m_IsoGrid.m_FaultData[j].TData[i].X = m_FaultB[i].X;
				//m_IsoGrid.m_FaultData[j].TData[i].Y = m_FaultB[i].Y;
			}
		}
		else  //既有上升盘也有下降盘
		{
			m_FaultData[j].UData.resize((int)m_FaultA.size());
			for (int i = 0; i<(int)(m_FaultA.size()); i++)
			{
				m_FaultData[j].UData[i].X = m_FaultA[i].X;
				m_FaultData[j].UData[i].Y = m_FaultA[i].Y;
			}
			for (int index = 0; index < (int)m_FaultA.size(); index++)
			{
				//if (minZZBX > m_IsoGrid.m_FaultData[j].UData[index].X)
				//	minZZBX = m_IsoGrid.m_FaultData[j].UData[index].X;
				//if (maxZZBX < m_IsoGrid.m_FaultData[j].UData[index].X)
				//	maxZZBX = m_IsoGrid.m_FaultData[j].UData[index].X;
				//if (minHZBY > m_IsoGrid.m_FaultData[j].UData[index].Y)
				//	minHZBY = m_IsoGrid.m_FaultData[j].UData[index].Y;
				//if (maxHZBY < m_IsoGrid.m_FaultData[j].UData[index].Y)
				//	maxHZBY = m_IsoGrid.m_FaultData[j].UData[index].Y;
			}
			m_FaultData[j].DData.resize((int)m_FaultB.size());
			for (int i = 0; i<(int)(m_FaultB.size()); i++)
			{
				m_FaultData[j].DData[i].X = m_FaultB[i].X;
				m_FaultData[j].DData[i].Y = m_FaultB[i].Y;
			}
			for (int index = 0; index < (int)m_FaultB.size(); index++)
			{
				//if (minZZBX > m_IsoGrid.m_FaultData[j].DData[index].X)
				//	minZZBX = m_IsoGrid.m_FaultData[j].DData[index].X;
				//if (maxZZBX < m_IsoGrid.m_FaultData[j].DData[index].X)
				//	maxZZBX = m_IsoGrid.m_FaultData[j].DData[index].X;
				//if (minHZBY > m_IsoGrid.m_FaultData[j].DData[index].Y)
				//	minHZBY = m_IsoGrid.m_FaultData[j].DData[index].Y;
				//if (maxHZBY < m_IsoGrid.m_FaultData[j].DData[index].Y)
				//	maxHZBY = m_IsoGrid.m_FaultData[j].DData[index].Y;
			}

			vector<TWOVALUE> v1, v2;
			m_FaultA.swap(v1);
			m_FaultB.swap(v2);
			m_FaultA.clear();
			m_FaultB.clear();
		}
	}
}
//两点是否相同 20140728
bool Jie_Ds::IsOnePT(TWOVALUE PtA, TWOVALUE PtB)
{
	if ((PtA.X == PtB.X) && (PtA.Y == PtB.Y))
	{
		return true;
	}
	//if (abs(PtA.X -PtB.X) < 0.00001 && abs(PtA.Y - PtB.Y) < 0.00001)
	//{
	//	AfxMessageBox("IsOnePt");
	//	return true;
	//}
	else
	{
		return false;
	}
}

//相同点个数 20140728
int Jie_Ds::GerEquNum(vector<TWOVALUE> PtA, vector<TWOVALUE>  PtB)
{
	int Num = 0;
	for (int i = 0; i < (int)PtA.size(); i++)
	{
		TWOVALUE A = PtA[i];
		for (int j = 0; j < (int)PtB.size(); j++)
		{
			TWOVALUE B = PtB[j];
			if (IsOnePT(A, B))
			{
				Num++;
			}
		}
	}
	return Num;
}

//得到相交的点个数20140728
int Jie_Ds::GetCrossNum(FaultPoint LineI, FaultPoint LineJ)
{
	vector<TWOVALUE> IUP = LineI.UData;
	vector<TWOVALUE> IDP = LineI.DData;

	vector<TWOVALUE> JUP = LineJ.UData;
	vector<TWOVALUE> JDP = LineJ.DData;

	int Num = 0;
	Num += GerEquNum(IUP, JUP);
	Num += GerEquNum(IUP, JDP);
	Num += GerEquNum(IDP, JUP);
	Num += GerEquNum(IDP, JDP);

	return Num;
}
//得到PTA,PTB之间距离PTA距离Di的点  20140729
TWOVALUE Jie_Ds::GerNearPt(TWOVALUE PtA, TWOVALUE PtB, double Di)
{
	TWOVALUE Pt;
	double dx = PtB.X - PtA.X;
	double dy = PtB.X - PtA.X;
	double d = sqrt(pow(dx, 2) + pow(dy, 2));
	if (abs(dx) + abs(dy) < 0.001)
	{
		Pt.X = (PtB.X + PtA.X) / 2;
		Pt.Y = (PtB.Y + PtA.Y) / 2;
	}
	else
	{
		dx = dx / d * Di;
		dy = dy / d * Di;
		Pt.X = PtA.X + dx;
		Pt.Y = PtA.Y + dy;
	}
	return  Pt;
}

//根据端点相交情况添加辅助点,并返回辅助点20140729
TWOVALUE Jie_Ds::AddPt(FaultPoint &Line, int IsU, int IsS)
{
	vector<TWOVALUE> UP = Line.UData;
	vector<TWOVALUE> DP = Line.DData;

	double GridDis = (m_XMax - m_XMin) / m_XNum + (m_YMax - m_YMin) / m_YNum;
	GridDis = GridDis / 100;

	TWOVALUE BeginPt, NextPt, AssistPt;		//交点（首点或者尾点），交点的相邻点，要添加的辅助点
	double   PtDis;

	if (IsU == 1)		//交点在上断层
	{
		if (IsS == 1)	//交点在首点
		{
			BeginPt = UP[0];
			NextPt = UP[1];
			PtDis = (sqrt(pow(BeginPt.X - NextPt.X, 2) + pow(BeginPt.Y - NextPt.Y, 2))) / 50;
			if (PtDis < GridDis)
			{
				AssistPt = GerNearPt(BeginPt, NextPt, GridDis);
			}
			else
			{
				AssistPt = GerNearPt(BeginPt, NextPt, PtDis);
			}
			UP.insert(UP.begin() + 1, AssistPt);			//在首点后插入辅助点
		}
		else			//交点在尾点
		{
			BeginPt = UP[(int)UP.size() - 1];
			NextPt = UP[(int)UP.size() - 2];
			PtDis = (sqrt(pow(BeginPt.X - NextPt.X, 2) + pow(BeginPt.Y - NextPt.Y, 2))) / 50;
			if (PtDis < GridDis)
			{
				AssistPt = GerNearPt(BeginPt, NextPt, GridDis);
			}
			else
			{
				AssistPt = GerNearPt(BeginPt, NextPt, PtDis);
			}
			UP.insert(UP.begin() + (int)UP.size() - 1, AssistPt);			//在最后一点之前后插入辅助点
		}
	}
	else				//交点在下断层
	{
		if (IsS == 1)	//交点在首点
		{
			BeginPt = DP[0];
			NextPt = DP[1];
			PtDis = (sqrt(pow(BeginPt.X - NextPt.X, 2) + pow(BeginPt.Y - NextPt.Y, 2))) / 50;
			if (PtDis < GridDis)
			{
				AssistPt = GerNearPt(BeginPt, NextPt, GridDis);
			}
			else
			{
				AssistPt = GerNearPt(BeginPt, NextPt, PtDis);
			}
			DP.insert(DP.begin() + 1, AssistPt);			//在首点后插入辅助点
		}
		else			//交点在尾点
		{
			BeginPt = DP[(int)DP.size() - 1];
			NextPt = DP[(int)DP.size() - 2];
			PtDis = (sqrt(pow(BeginPt.X - NextPt.X, 2) + pow(BeginPt.Y - NextPt.Y, 2))) / 50;
			if (PtDis < GridDis)
			{
				AssistPt = GerNearPt(BeginPt, NextPt, GridDis);
			}
			else
			{
				AssistPt = GerNearPt(BeginPt, NextPt, PtDis);
			}
			DP.insert(DP.begin() + (int)DP.size() - 1, AssistPt);			//在最后一点之前后插入辅助点
		}
	}
	Line.UData = UP;
	Line.DData = DP;
	return AssistPt;
}

//根据端点相交情况添加辅助点Pt  20140729
void Jie_Ds::AddPt(FaultPoint &Line, int IsU, int IsS, TWOVALUE AssistPt)
{
	vector<TWOVALUE> UP = Line.UData;
	vector<TWOVALUE> DP = Line.DData;

	if (IsU == 1)		//交点在上断层
	{
		if (IsS == 1)	//交点在首点
		{
			UP.insert(UP.begin() + 1, AssistPt);			//在首点后插入辅助点
		}
		else			//交点在尾点
		{
			UP.insert(UP.begin() + (int)UP.size() - 1, AssistPt);			//在最后一点之前后插入辅助点
		}
	}
	else				//交点在下断层
	{
		if (IsS == 1)	//交点在首点
		{
			DP.insert(DP.begin() + 1, AssistPt);			//在首点后插入辅助点
		}
		else			//交点在尾点
		{
			DP.insert(DP.begin() + (int)DP.size() - 1, AssistPt);			//在最后一点之前后插入辅助点
		}
	}
	Line.UData = UP;
	Line.DData = DP;
}

//根据端点相交情况添加辅助点，使一个交点变为两个交点20140728
void Jie_Ds::AddPt(FaultPoint &LineI, FaultPoint &LineJ, int IU, int IS, int JU, int JS)
{

	TWOVALUE AssistPt = AddPt(LineI, IU, IS);		//针对断层i添加辅助点，并返回添加点
	AddPt(LineJ, JU, JS, AssistPt);				//针对断层j添加辅助点
}
//是否在端点相交，并只有一个交点20140728
BOOL Jie_Ds::IsSEinterse(FaultPoint &LineI, FaultPoint &LineJ)
{
	vector<TWOVALUE> IUP = LineI.UData;
	vector<TWOVALUE> IDP = LineI.DData;

	vector<TWOVALUE> JUP = LineJ.UData;
	vector<TWOVALUE> JDP = LineJ.DData;

	int IU = 1; int IS = 1; int JU = 1; int JS = 1;	//IU = 1表示交点在I断层的上断层
	int Num = 0;		//交点个数
	TWOVALUE IUS, IUE, IDS, IDE, JUS, JUE, JDS, JDE;//上下断层的首尾点
	int iuNum, idNum, juNum, jdNum;
	//两个断层上下断层线上点的个数
	iuNum = (int)IUP.size();
	idNum = (int)IDP.size();
	juNum = (int)JUP.size();
	jdNum = (int)JDP.size();
	//断层点不能太少
	if (iuNum < 2 || idNum < 2 || juNum < 2 || jdNum < 2)
	{
		return FALSE;
	}
	//该方法仅限于有一个交点并且在端点处
	if (GetCrossNum(LineI, LineJ) != 1)			//含有两个以上的交点或无交点
	{
		return FALSE;
	}
	IUS = IUP[0];								//断层I，上断层首点
	IUE = IUP[iuNum - 1];

	IDS = IDP[0];								//断层I，下断层首点
	IDE = IDP[idNum - 1];

	JUS = JUP[0];
	JUE = JUP[juNum - 1];

	JDS = JDP[0];
	JDE = JDP[jdNum - 1];
	//上首 +~~~第一条断层的上断层首点跟第二条断层比较
	if (IsOnePT(IUS, JUS))						//断层I的上断层首点与断层J的上断层首点相同
	{
		IU = 1;		IS = 1;		JU = 1;		JS = 1;
		Num++;
	}
	if (IsOnePT(IUS, JUE))						//断层I的上断层首点与断层J的上断层尾点相同
	{
		IU = 1;		IS = 1;		JU = 1;		JS = 0;
		Num++;
	}
	if (IsOnePT(IUS, JDS))						//断层I的上断层首点与断层J的下断层首点相同
	{
		IU = 1;		IS = 1;		JU = 0;		JS = 1;
		Num++;
	}
	if (IsOnePT(IUS, JDE))						//断层I的上断层首点与断层J的下断层尾点相同
	{
		IU = 1;		IS = 1;		JU = 0;		JS = 0;
		Num++;
	}

	/************************************************/
	//上尾 +~~~第一条断层的上断层未点跟第二条断层比较
	if (IsOnePT(IUE, JUS))						//断层I的上断层首点与断层J的上断层首点相同
	{
		IU = 1;		IS = 0;		JU = 1;		JS = 1;
		Num++;
	}
	if (IsOnePT(IUE, JUE))						//断层I的上断层首点与断层J的上断层尾点相同
	{
		IU = 1;		IS = 0;		JU = 1;		JS = 0;
		Num++;
	}
	if (IsOnePT(IUE, JDS))						//断层I的上断层首点与断层J的下断层首点相同
	{
		IU = 1;		IS = 0;		JU = 0;		JS = 1;
		Num++;
	}
	if (IsOnePT(IUE, JDE))						//断层I的上断层首点与断层J的下断层尾点相同
	{
		IU = 1;		IS = 0;		JU = 0;		JS = 0;
		Num++;
	}

	/***********************************************************/
	//下首 +~~~第一条断层的下断层首点跟第二条断层比较
	if (IsOnePT(IDS, JUS))						//断层I的下断层首点与断层J的上断层首点相同
	{
		IU = 0;		IS = 1;		JU = 1;		JS = 1;
		Num++;
	}
	if (IsOnePT(IDS, JUE))						//断层I的下断层首点与断层J的上断层尾点相同
	{
		IU = 0;		IS = 1;		JU = 1;		JS = 0;
		Num++;
	}
	if (IsOnePT(IDS, JDS))						//断层I的下断层首点与断层J的下断层首点相同
	{
		IU = 0;		IS = 1;		JU = 0;		JS = 1;
		Num++;
	}
	if (IsOnePT(IDS, JDE))						//断层I的下断层首点与断层J的下断层尾点相同
	{
		IU = 0;		IS = 1;		JU = 0;		JS = 0;
		Num++;
	}

	/************************************************/
	//下尾 +~~~第一条断层的下断层未点跟第二条断层比较
	if (IsOnePT(IDE, JUS))						//断层I的下断层尾点与断层J的上断层首点相同
	{
		IU = 0;		IS = 0;		JU = 1;		JS = 1;
		Num++;
	}
	if (IsOnePT(IDE, JUE))						//断层I的下断层尾点与断层J的上断层尾点相同
	{
		IU = 0;		IS = 0;		JU = 1;		JS = 0;
		Num++;
	}
	if (IsOnePT(IDE, JDS))						//断层I的下断层尾点与断层J的下断层首点相同
	{
		IU = 0;		IS = 0;		JU = 0;		JS = 1;
		Num++;
	}
	if (IsOnePT(IDE, JDE))						//断层I的下断层尾点与断层J的下断层尾点相同
	{
		IU = 0;		IS = 0;		JU = 0;		JS = 0;
		Num++;
	}
	if (Num == 1)
	{
		//cout << "hello==================================" << endl;
		//只有一个端点位置的交点，添加辅助点
		AddPt(LineI, LineJ, IU, IS, JU, JS);
		return TRUE;
	}
	return FALSE;
}

void Jie_Ds::Combination(vector<FaultPoint>&Lines)
{
	for (int i = 0; i < (int)Lines.size(); i++)
	{
		FaultPoint LineI = Lines[i];
		for (int j = 0; j < (int)Lines.size(); j++)
		{
			FaultPoint LineJ = Lines[j];
			if (i == j)//同一个断层
			{
				continue;
			}
			//TODO：判断是否在首尾点相交 如果相交，则通过添加辅助点将一个交点扩展成两个交点
			if (IsSEinterse(LineI, LineJ))
			{
				//此时已经完成添加点的操作，更新Lines容器
				Lines[i] = LineI;
				Lines[j] = LineJ;
			}
		}
	}
}
//将不闭合的的有上下断层的断层闭合20131206  20131209
//这里的闭合与不闭合指的应该是断层有没有在地面消隐，自己猜想
void Jie_Ds::CloseFalut()
{
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		int UDateSize = (int)m_FaultData[i].UData.size() - 1;
		int DDateSize = (int)m_FaultData[i].DData.size() - 1;
		//应该为闭合断层
		if (UDateSize > 0 && DDateSize > 0)
		{
			TWOVALUE UpSTar, UpEnd, DownStar, DownEnd;
			UpSTar = m_FaultData[i].UData[0];//上断层第一个数据点信息
			DownStar = m_FaultData[i].DData[0];//下断层第一个数据点信息
			UpEnd = m_FaultData[i].UData[UDateSize];//上断层最后一个数据点信息
			DownEnd = m_FaultData[i].DData[DDateSize];//下断层最后一个数据点信息
			if (UpEnd.X != DownEnd.X || UpEnd.Y != DownEnd.Y)
			{
				//double dx = (UpEnd.X - m_FaultData[i].UData[UDateSize - 1].X) * 0.0001;
				//double dy = (UpEnd.Y - m_FaultData[i].UData[UDateSize - 1].Y) * 0.0001;
				//TWOVALUE TempEnd;
				////TempEnd.X = (UpEnd.X + DownEnd.X) / 2.0 ;
				////TempEnd.Y = (UpEnd.Y + DownEnd.Y) / 2.0 ;
				//TempEnd.X = UpEnd.X + dx ;
				//TempEnd.Y = UpEnd.Y + dy ;  //防止区域追踪时候判断断层顺时针时候出现三点一线情况20131209
				////m_FaultData[i].UData.push_back(TempEnd);
				////m_FaultData[i].DData.push_back(TempEnd);
				m_FaultData[i].DData.push_back(UpEnd);//将上盘的最后一个点加入到下盘中
			}
			if (UpSTar.X != DownStar.X || UpSTar.Y != DownStar.Y)
			{
				//double dx = (UpSTar.X - m_FaultData[i].UData[1].X) * 0.0001;
				//double dy = (UpSTar.Y - m_FaultData[i].UData[1].Y) * 0.0001;
				//TWOVALUE TempSTar;
				////TempSTar.X = (UpSTar.X + DownStar.X) / 2.0 ;
				////TempSTar.Y = (UpSTar.Y + DownStar.Y) / 2.0 ;
				//TempSTar.X = UpSTar.X + dx ;
				//TempSTar.Y = UpSTar.Y + dy ;//防止区域追踪时候判断断层顺时针时候出现三点一线情况20131209

				vector<TWOVALUE> Up, Down;
				//Up = m_FaultData[i].UData;
				Down = m_FaultData[i].DData;
				//Up.insert(Up.begin(),TempSTar);
				//Down.insert(Down.begin(),TempSTar);
				Down.insert(Down.begin(), UpSTar);
				m_FaultData[i].DData = Down;//这几行代码把上盘第一个点加入到下盘中
											//m_FaultData[i].UData = Up;
			}
		}
	}
}

//得到过P点的垂线方程
void Jie_Ds::GetABC(TWOVALUE Star, TWOVALUE End, TWOVALUE P, double &A, double &B, double &C)
{
	double A1, B1;
	A1 = End.Y - Star.Y;
	B1 = Star.X - End.X;
	//A1X+B1Y+C1 = 0;原始数据组成的直线方程
	if (A1 == 0)
	{
		A = 0;
		B = 1;
	}
	else if (B1 == 0)
	{
		A = 1;
		B = 0;
	}
	else
	{
		A = B1;
		B = -A1;
		C = -A * P.X - B * P.Y;
	}

}
//是否有交点
bool Jie_Ds::L2L_Intersect(THRVALUE p1_s, THRVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e)
{
	double Xmax_1, Xmax_2, Xmin_1, Xmin_2, Ymax_1, Ymax_2, Ymin_1, Ymin_2;
	double V1, V2, V3, V4;

	if (p1_s.X > p1_e.X)
	{
		Xmax_1 = p1_s.X;
		Xmin_1 = p1_e.X;
	}
	else
	{
		Xmax_1 = p1_e.X;
		Xmin_1 = p1_s.X;
	}


	if (p1_s.Y > p1_e.Y)
	{
		Ymax_1 = p1_s.Y;
		Ymin_1 = p1_e.Y;
	}
	else
	{
		Ymax_1 = p1_e.Y;
		Ymin_1 = p1_s.Y;
	}


	if (p2_s.X > p2_e.X)
	{
		Xmax_2 = p2_s.X;
		Xmin_2 = p2_e.X;
	}
	else
	{
		Xmax_2 = p2_e.X;
		Xmin_2 = p2_s.X;
	}

	if (p2_s.Y > p2_e.Y)
	{
		Ymax_2 = p2_s.Y;
		Ymin_2 = p2_e.Y;
	}
	else
	{
		Ymax_2 = p2_e.Y;
		Ymin_2 = p2_s.Y;
	}


	if (Xmax_1 < Xmin_2 || Xmin_1 > Xmax_2 || Ymin_1 > Ymax_2 || Ymax_1 < Ymin_2)   //两线段最小矩形不相交，得出两线段不相交
		return false;
	else
	{
		V1 = (p1_e.X - p1_s.X) * (p2_s.Y - p1_s.Y) - (p2_s.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V2 = (p1_e.X - p1_s.X) * (p2_e.Y - p1_s.Y) - (p2_e.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V3 = (p2_e.X - p2_s.X) * (p1_s.Y - p2_s.Y) - (p1_s.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		V4 = (p2_e.X - p2_s.X) * (p1_e.Y - p2_s.Y) - (p1_e.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		//特殊情况点在直线上时候   20131015
		bool Mark = false;
		TWOVALUE TempPoint;
		if (abs(V1)< 0.00000001)
		{
			Mark = true;
		}
		else if (abs(V2)< 0.00000001)
		{
			Mark = true;
		}
		else if (abs(V3)< 0.00000001)
		{
			Mark = true;
		}
		else if (abs(V4)< 0.00000001)
		{
			Mark = true;
		}
		if (Mark)
		{
			double t = 0;
			if (abs(TempPoint.X - p1_s.X)<abs(TempPoint.Y - p1_s.Y))
			{
				t = (TempPoint.Y - p1_s.Y) / (p1_e.Y - p1_s.Y);/////////////////////
			}
			else
			{
				t = (TempPoint.X - p1_s.X) / (p1_e.X - p1_s.X);
			}
			return true;
		}
		if (V3 * V4 <= 0 && V1 * V2 <= 0)
		{
			double dy = p1_e.Y - p1_s.Y;
			double dx = p1_e.X - p1_s.X;
			double t = 0;
			if (abs(p2_e.X - p2_s.X)<0.00000000001)
			{
				t = (p2_e.X - p1_s.X) / dx;/////////////////////
			}
			else
			{
				double k = (p2_s.Y - p2_e.Y) / (p2_s.X - p2_e.X);
				t = ((p2_s.Y - p1_s.Y) - k*p2_s.X + k*p1_s.X) / (dy - k*dx);
			}
			return true;
		}
		else
			return false;
	}
}

bool Jie_Ds::IsIntersect(TWOVALUE p1_s, TWOVALUE p1_e, vector<THRVALUE> Lines)
{
	for (int i = 0; i < (int)Lines.size() - 1; i++)
	{
		THRVALUE Star = Lines[i];
		THRVALUE End = Lines[i + 1];
		if (L2L_Intersect(Star, End, p1_s, p1_e))
		{
			return true;
		}
	}
	return false;
}

//处理单线断层为断层多边形，得到用于估值的断层数组
void Jie_Ds::DealWithSingleFault()
{
	double k1 = 0;//斜率
	double distance = 0.05*(m_XMax - m_XMin) / m_XNum; //点的拆分距离
	//cout << distance << endl;
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		int UDateSize = (int)m_FaultData[i].UData.size();//上断层线
		int DDateSize = (int)m_FaultData[i].DData.size();//下断层线
		int TDateSize = (int)m_FaultData[i].TData.size();//中间断层线
														 //只有上升盘或下降盘，把数据当成中间线数据

														 //只有中间线数据，而没有上下断层数据
		if (TDateSize>0 && DDateSize == 0 && UDateSize == 0)
		{
			if (TDateSize == 2)		//只有两点
			{
				//TODO:进行特殊处理，添加中间的一个点TDateSize···
			}
			//为上升盘和下降盘数组分配空间
			m_FaultData[i].DData.resize(TDateSize);
			m_FaultData[i].UData.resize(TDateSize);
			//上下断层的第一个点和最后一个点默认是和中间线的位置是一样的
			//初始化它们的第一个点
			m_FaultData[i].DData[0].X = m_FaultData[i].TData[0].X + 0;
			m_FaultData[i].DData[0].Y = m_FaultData[i].TData[0].Y + 0;
			m_FaultData[i].UData[0].X = m_FaultData[i].TData[0].X - 0;
			m_FaultData[i].UData[0].Y = m_FaultData[i].TData[0].Y - 0;
			//初始化最后一个点
			m_FaultData[i].DData[TDateSize - 1].X = m_FaultData[i].TData[TDateSize - 1].X + 0;
			m_FaultData[i].DData[TDateSize - 1].Y = m_FaultData[i].TData[TDateSize - 1].Y + 0;
			m_FaultData[i].UData[TDateSize - 1].X = m_FaultData[i].TData[TDateSize - 1].X - 0;
			m_FaultData[i].UData[TDateSize - 1].Y = m_FaultData[i].TData[TDateSize - 1].Y - 0;
			//为数组中其他点赋值
			for (int j = 1; j < (int)m_FaultData[i].TData.size() - 1; j++)
			{
				THRVALUE A = m_FaultData[i].TData[j - 1];
				THRVALUE O = m_FaultData[i].TData[j];
				THRVALUE B = m_FaultData[i].TData[j + 1];
				double dx1 = A.X - O.X;
				double dx2 = B.X - O.X;
				double dy1 = A.Y - O.Y;
				//double dy2 = B.X - O.Y;这一步应该有点错误
				double dy2 = B.Y - O.Y;
				if (abs(dx1*dy2 - dx2*dy1)<0.000001)	//三点一线
				{
					//TODO:特殊处理，应该在中间点的位置上
					//做垂直平分线
					double A1, B1, C1;
					TWOVALUE Star, End, P;
					Star.X = A.X;
					Star.Y = A.Y;
					End.X = B.X;
					End.Y = B.Y;
					P.X = O.X;
					P.Y = O.Y;
					GetABC(Star, End, P, A1, B1, C1);
					TWOVALUE D;
					//垂线的单位方向向量
					double dx = A1 / sqrt(pow(A1, 2) + pow(B1, 2));
					double dy = B1 / sqrt(pow(A1, 2) + pow(B1, 2));
					//cout << A1 << " " << B1 << " " << C1 << endl;
					//cout << dx << " " << dy << endl;
					//cout << dx*distance << " " << dy*distance << endl;
					D.X = O.X + dx*distance;
					D.Y = O.Y - dy*distance;

					TWOVALUE E;
					E.X = 2 * O.X - D.X;
					E.Y = 2 * O.Y - D.Y;
					if (D.X > O.X || (D.X == O.X && D.Y > O.Y))	//按照逆断层来处理
					{
						m_FaultData[i].DData[j] = E;
						m_FaultData[i].UData[j] = D;
					}
					else
					{
						m_FaultData[i].DData[j] = D;
						m_FaultData[i].UData[j] = E;
					}
				}
				else				//使用受力分析或菱形四边形的方法
				{
					//cout << "hello" << endl;
					//dx1 = dx1/sqrt(pow(dx1,2)+pow(dy1,2));
					//dy1 = dy1/sqrt(pow(dx1,2)+pow(dy1,2));
					//dx2 = dx2/sqrt(pow(dx2,2)+pow(dy2,2));
					//dy2 = dy2/sqrt(pow(dx2,2)+pow(dy2,2));
					//A.X = O.X + dx1 * distance;
					//A.Y = O.Y + dy1 * distance;

					//B.X = O.X + dx2 * distance;
					//B.Y = O.Y + dy2 * distance;

					//TWOVALUE D;
					//D.X = 0.5*(A.X + B.X);
					//D.Y = 0.5*(A.Y + B.Y);
					//TWOVALUE E;
					//E.X = 2*O.X - D.X;
					//E.Y = 2*O.Y - D.Y;
					//if (D.X >O.X || (D.X == O.X&&D.Y > O.Y))				//按照逆断层来处理
					//{
					//	m_FaultData[i].DData[j] = E;
					//	m_FaultData[i].UData[j] = D;
					//}
					//else
					//{
					//	m_FaultData[i].DData[j] = D;
					//	m_FaultData[i].UData[j] = E;
					//}
					//20141122重新修订角平分线算法
					double x1, y1, x2, y2;
					//求出相应的方向单位向量
					x1 = dx1 / sqrt(pow(dx1, 2) + pow(dy1, 2));
					y1 = dy1 / sqrt(pow(dx1, 2) + pow(dy1, 2));
					x2 = dx2 / sqrt(pow(dx2, 2) + pow(dy2, 2));
					y2 = dy2 / sqrt(pow(dx2, 2) + pow(dy2, 2));

					A.X = x1 * distance;//乘以点的拆分距离
					A.Y = y1 * distance;

					B.X = x2 * distance;
					B.Y = y2 * distance;

					TWOVALUE D;
					D.X = O.X + (A.X + B.X);
					D.Y = O.Y + (A.Y + B.Y);

					TWOVALUE E;
					E.X = 2 * O.X - D.X;
					E.Y = 2 * O.Y - D.Y;
					if (j == 1)
					{
						if (D.X > O.X || (D.X == O.X && D.Y > O.Y))	//按照逆断层来处理
						{
							m_FaultData[i].DData[j] = E;
							m_FaultData[i].UData[j] = D;
						}
						else
						{
							m_FaultData[i].DData[j] = D;
							m_FaultData[i].UData[j] = E;
						}
					}
					else
					{
						//接下来主要判断点D、E哪一个在中间断层线的下方，哪一个在中间断层线的上方
						if (!IsIntersect(m_FaultData[i].DData[j - 1], D, m_FaultData[i].TData))		//和中间断层线无交点，点D在下方
						{
							m_FaultData[i].DData[j] = D;
							m_FaultData[i].UData[j] = E;
						}
						else   //和中间断层线有交点，点E在下方
						{
							m_FaultData[i].DData[j] = E;
							m_FaultData[i].UData[j] = D;
						}
					}
				}
			}
		}
	}
}

//如果只有上下断层，没有断层中线的时候，构造中线20131119
void Jie_Ds::GetMiddleData()
{
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		FaultPoint OneLine = m_FaultData[i];

		/***防止上下断层点数目不一样20131209***/
		int MinNum = (int)OneLine.UData.size();
		if (MinNum >(int)OneLine.DData.size())
		{
			MinNum = (int)OneLine.DData.size();
		}
		/**************************************/
		if (OneLine.TData.size() <= 1)
		{
			for (int j = 0; j < MinNum; j++)
			{
				THRVALUE Middle;
				Middle.X = (OneLine.DData[j].X + OneLine.UData[j].X) / 2.0;
				Middle.Y = (OneLine.DData[j].Y + OneLine.UData[j].Y) / 2.0;
				Middle.Z = 0;
				m_FaultData[i].TData.push_back(Middle);//TData中保存了断层中线的信息
			}
		}
	}
}

void Jie_Ds::GetNearByPoint(const TWOVALUE& m_PrePoint, const TWOVALUE& m_NextPoint, const TWOVALUE& point, Data &m_newPoint)
{
	double X0 = point.X;      //线段外一点                              
	double Y0 = point.Y;
	double X1 = m_PrePoint.X; //线段上两端点                              
	double Y1 = m_PrePoint.Y;
	double X2 = m_NextPoint.X;
	double Y2 = m_NextPoint.Y;

	double X, Y;//垂足

				//斜率为正无穷的情况
	if (X1 == X2)
	{
		X = X1;
		Y = Y0;
	}
	//斜率为0的情况
	else if (Y1 == Y2)
	{
		X = X0;
		Y = Y1;
	}
	else
	{
		//斜率
		double K = (Y2 - Y1) / (X2 - X1);
		X = (K*Y0 + X0 + K*(K*X1 - Y1)) / (K*K + 1.0f);
		Y = (-1 * K*X1 + Y1 + K*(K*Y0 + X0)) / (K*K + 1.0f);
	}
	m_newPoint.X = X;
	m_newPoint.Y = Y;

}

double Jie_Ds::GetPointToLineDis(TWOVALUE PrePoint, TWOVALUE NextPoint, TWOVALUE point)
{
	Data ND;
	//找到垂足
	GetNearByPoint(PrePoint, NextPoint, point, ND);
	//垂足在线段上
	//if ((((ND.X - PrePoint.X) * (ND.X - NextPoint.X) < 0)||(ND.X==PrePoint.X)||(ND.X==NextPoint.X))||
	//	(((ND.Y - PrePoint.Y) * (ND.Y - NextPoint.Y) < 0) || (ND.Y == PrePoint.Y) || (ND.Y == NextPoint.Y)))
	if (((ND.X - PrePoint.X) * (ND.X - NextPoint.X) <= 0)||((ND.Y - PrePoint.Y) * (ND.Y - NextPoint.Y)<=0))
	//if ((ND.X - PrePoint.X) * (ND.X - NextPoint.X) <= 0)
		return GetDis(point.X, point.Y, ND.X, ND.Y);
	else
		return 999999;
}

//A点到曲线段的距离		20140731
double Jie_Ds::GetDistance(vector<TWOVALUE> Pts, TWOVALUE A)
{
	double Mind = 1000000000;
	for (int i = 0; i < (int)Pts.size() - 1; i++)
	{
		TWOVALUE Star = Pts[i];
		TWOVALUE End = Pts[i + 1];
		double ds = sqrt(pow(Star.X - A.X, 2) + pow(Star.Y - A.Y, 2));
		double de = sqrt(pow(End.X - A.X, 2) + pow(End.Y - A.Y, 2));
		double d = GetPointToLineDis(Star, End, A);
		if (ds < Mind)
		{
			Mind = ds;
		}
		if (de < Mind)
		{
			Mind = de;
		}
		if (d < Mind)
		{
			Mind = d;
		}
	}
	return Mind;
}

//若断层线没有落差，则自己构造断层落差20131115
void Jie_Ds::SetDIF(double fDis)
{
	BOOL Mark = FALSE;
	double MarkD = 0;
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		FaultPoint OneLine = m_FaultData[i];
		for (int j = 0; j < (int)OneLine.TData.size(); j++)
		{
			//断层的落差存储在中线的Z分量之中
			if (abs(OneLine.TData[j].Z) > 0.001)//跳出条件？？
			{
				Mark = TRUE;
				break;
			}
		}
		if (Mark)
		{
			break;
		}
	}
	//if (Mark)
	{
		//设计断层落差，规则就是平面距离越小，落差越小
		double dx = (m_XMax - m_XMin) / m_XNum;
		double dy = (m_YMax - m_YMin) / m_YNum;
		MarkD = dx;
		if (MarkD < dy)
		{
			MarkD = dy;		//d代表一个fdis
		}
	}
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		FaultPoint OneLine = m_FaultData[i];
		//for (int j = 0 ; j < (int)OneLine.UData.size() ; j ++)
		for (int j = 0; j < (int)OneLine.TData.size(); j++)		//以中线为基准20131209
		{
			TWOVALUE UpPt = OneLine.UData[j];
			TWOVALUE DownPt = OneLine.DData[j];
			//TODO:做OneLine.TData[j]到上断层或者下断层的距离20140731
			double d = GetDistance(OneLine.DData, UpPt);	//20140731	
															//double d = sqrt(pow(UpPt.X - DownPt.X , 2) + pow(UpPt.Y - DownPt.Y , 2));
			if (d > 2 * MarkD)
			{
				d = 2 * MarkD;		//防止落差太大 20131209
			}
			//else if (d < 0.02 * fDis )
			//{
			//d = 0.02 * fDis ;		//不能太小20140722
			//}
			double DiF = d / MarkD * fDis / 6;	//落差小点
			m_FaultData[i].TData[j].Z = DiF;  //TData中的Z分量保存落差信息
		}
	}
}

//整理断层线，使其没有重合点，除了首尾点20140721
void Jie_Ds::TidyFaultLine(FalutLine &LineI)
{
	vector<TWOVALUE> Fline = LineI.FLine;
	//这里这个操作其实没必要，因为上一个函数中已经判断过了
	if ((int)Fline.size() < 2)
	{
		return;
	}
	for (int i = 0; i < (int)Fline.size() - 1; i++)
	{
		TWOVALUE P1, P2;
		P1 = Fline[i];
		P2 = Fline[i + 1];
		//两个点的重合条件
		if (abs(P1.X - P2.X) < 0.001 && abs(P1.Y - P2.Y) < 0.001)
		{
			Fline.erase(Fline.begin() + i + 1);
			i = i - 1;
		}
	}
	LineI.FLine = Fline;
}
bool Jie_Ds::IsClockwise(const TWOVALUE &p0, const TWOVALUE &p1, const TWOVALUE &p2)
{
	double TempVal = (p1.X - p0.X) * (p2.Y - p1.Y) - (p2.X - p1.X) * (p1.Y - p0.Y);
	//叉乘:逆时针为正  顺时针为负

	if (TempVal > 0) //逆时针
		return false;
	else            //顺时针
		return true;

}

//连接断层线20140721
//LineI,LineJ为要连接的断层线
//IP,JP为线段相交的交点序号，两点P1,P2;  P1在LineI上的序号为IP[0],在LineJ上的序号为JP[0]
FalutLine Jie_Ds::ConnectLine(FalutLine &LineI, FalutLine LineJ, int IP[], int JP[])
{
	FalutLine NewLine;
	NewLine.IsClose = LineI.IsClose;

	//修改部分
	//NewLine.IsNormal = NewLine.IsNormal;
	NewLine.IsNormal = LineI.IsNormal;

	NewLine.IsClock = LineI.IsClock;
	//保存相同点的索引
	int IS = 0, JSd = 0, IE = 0, JEd = 0;
	IS = IP[0];		IE = IP[1];
	JSd = JP[0];		JEd = JP[1];
	cout << "=====================" << endl;
	cout << "IS;" << IS << " " << "IE;" << IE << endl;
	cout << "JSd;" << JSd << " " << "JEd;" << JEd<< endl;
	cout << "=====================" << endl;
	//一般来说，全部的点都在新断层上，自己构造断层上下盘首尾点可能不在
	//加入第一条线
	if (IE - IS < (int)LineI.FLine.size() / 2)
	{
		//IS ---0,End-1 ~~~IE(不包含IE)
		NewLine.IsClock = LineI.IsClock * (-1);		//修改了顺序20140722
		for (int i = IS; i >= 0; i--)
		{
			TWOVALUE Pt = LineI.FLine[i];
			NewLine.FLine.push_back(Pt);
		}
		//这里的FLine[(int)LineI.FLine.size() - 1]和FLine[0]应该是同一个点，是不是重复添加了
		for (int i = (int)LineI.FLine.size() - 2; i > IE; i--)
			//for (int i = (int)LineI.FLine.size() - 1 ; i > IE ; i --)
		{
			TWOVALUE Pt = LineI.FLine[i];
			NewLine.FLine.push_back(Pt);
		}
	}
	else
	{
		//IS~~~IE(不包含IE)
		for (int i = IS; i < IE; i++)
		{
			TWOVALUE Pt = LineI.FLine[i];
			NewLine.FLine.push_back(Pt);
		}
	}
	//加入另外的一条线
	if (JEd > JSd)
	{
		if (JEd - JSd < (int)LineJ.FLine.size() / 2)
		{
			//JEd ---End,`1~~~JSd
			for (int j = JEd; j < (int)LineJ.FLine.size(); j++)
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
			//这里之所以要加等号，是为了闭合
			//for (int j = 1; j < JSd; j++)
			for (int j = 1; j <= JSd; j++)		//加入最后一个点，跟第一个点对应
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
		}
		else	//JEd---JSd````递减
		{
			for (int j = JEd; j >= JSd; j--)
				//for (int j = JEd ; j >= JSd ; j --)		//加入最后一个点，跟第一个点对应
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
		}
	}
	else
	{
		//cout << "hello" << endl;
		if (JSd - JEd  < (int)LineJ.FLine.size() / 2)
		{
			//JEd ---0,`End-1~~~JSd
			for (int j = JEd; j >= 0; j--)
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
			for (int j = (int)LineJ.FLine.size() - 2; j >= JSd; j--)
				//for (int j = (int)LineJ.FLine.size() - 2 ; j >= JSd ; j --)		//加入最后一个点，跟第一个点对应
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
		}
		else	//JEd---JSd````递增
		{
			//cout << "hello1" << endl;
			for (int j = JEd; j <= JSd; j++)
				//for (int j = JEd ; j >= JSd ; j --)		
			{
				TWOVALUE Pt = LineJ.FLine[j];
				NewLine.FLine.push_back(Pt);
			}
		}
	}
	return NewLine;
}

//断层线是否相交，相交的断层线在断层点上重合20140719
BOOL Jie_Ds::Isintersect(FalutLine &LineI, FalutLine LineJ)
{
	//TODO:判断断层线是否相交
	BOOL IsCross = FALSE;
	int IP[20], JP[20];
	int Num = 0;
	for (int i = 0; i < (int)LineI.FLine.size() - 1; i++)//最后一点跟首点相同，防止出现三个，为了使断层线闭合，断层线上的最后一个点和第一个点是同一个点
	{
		TWOVALUE PLineI = LineI.FLine[i];
		for (int j = 0; j < (int)LineJ.FLine.size() - 1; j++)
		{
			TWOVALUE PLineJ = LineJ.FLine[j];
			//判断两个点重合的条件
			if (abs(PLineI.X - PLineJ.X) < 0.001 && abs(PLineI.Y - PLineJ.Y) < 0.001)
			{
				IP[Num] = i;
				JP[Num] = j;
				Num++;
			}
		}
	}
	if (Num == 2)			//理论上来说只有0或者2个交点
	{
		IsCross = TRUE;
		FalutLine NewLine = ConnectLine(LineI, LineJ, IP, JP);
		LineI = NewLine;
	}
	return IsCross;
}

//对相交的断层线进行组合 20140717
void Jie_Ds::Combination()
{
	for (int i = 0; i < (int)m_vecFaultLines.size(); i++)
	{
		FalutLine LineI = m_vecFaultLines[i];
		for (int j = 0; j < (int)m_vecFaultLines.size(); j++)
		{
			if (i == j)
			{
				continue;
			}
			FalutLine LineJ = m_vecFaultLines[j];

			//TODO:判断断层线是否相交，相交则将两条合并成为一条,并重新开始追踪
			if (Isintersect(LineI, LineJ))
			{
				TidyFaultLine(LineI);

				m_vecFaultLines[i] = LineI;
				m_vecFaultLines.erase(m_vecFaultLines.begin() + j);//因为断层线LineJ已经合并到断层线LineI中，所以可以将断层线LineJ删除即可
				if (j < i)
				{
					i--;
				}
				j = 0;		//重新开始判断···防止出现：01不想交，02相交，21相交的情况
			}
		}
	}
}

//得到用于追踪的断层数组
void Jie_Ds::GetFaultLine(vector<FaultPoint> data)
{
	m_vecFaultLines.clear();
	//检测数据是否合理
	for (int i = 0; i < (int)data.size(); i++)
	{
		FalutLine Line;
		FaultPoint LineOne = data[i];
		vector<TWOVALUE> LineU = LineOne.UData;//上断层线
		vector<TWOVALUE> LineD = LineOne.DData;//下断层线

		int Num = LineD.size();
		//这里之所以不添加下断层的首尾点，是因为之前将不闭合断层进行闭合操作的时候，上断层的首尾分别添加了下断层的首尾点，这两个点没必要重复添加
		//for (int j = Num - 1; j > -1; j --)
		for (int j = Num - 2; j > 0; j--)
		{
			LineU.push_back(LineD[j]);
		}
		LineU.push_back(LineU[0]);//首尾相连
		Line.FLine = LineU;//得到一条断层线
		Line.IsClose = TRUE;//闭合断层
		m_vecFaultLines.push_back(Line);//断层线上的点前一部分为上断层线上的点，后一部分为下断层上的点
	}
	int NumLines = (int)m_vecFaultLines.size();
	//断层线的数量必须大于1
	if (NumLines<1)
	{
		return;
	}
	//删除不合理数据
	for (int i = 0; i < (int)m_vecFaultLines.size(); i++)
	{
		//断层线上点的数目不能太少
		if (m_vecFaultLines[i].FLine.size()<2)
		{
			m_vecFaultLines.erase(m_vecFaultLines.begin() + i);
			i--;
		}
		else
		{
			FalutLine LineI = m_vecFaultLines[i];
			TidyFaultLine(LineI);					//整理断层线，使其没有重合点，除了首尾点20140721

													//给断层顺序赋值20140722
			vector<TWOVALUE> Fline = LineI.FLine;
			int Num = (int)Fline.size();
			//这里还是同样的道理，为了使断层线闭合，Fline[Num-1]其实是和Fline[0]保存的同一个点
			bool Clock = IsClockwise(Fline[Num - 2], Fline[0], Fline[1]);
			if (Clock)
			{
				LineI.IsClock = 1;		//顺时针
			}
			else
			{
				LineI.IsClock = -1;		//逆时针
			}

			m_vecFaultLines[i] = LineI;
		}
	}
	//将相交的断层划分为一个断层20140717
	Combination();
}

void Jie_Ds::NormalVector(vector<THRVALUE>& a, THRVALUE& b, double Position)
{
	//两点距离
	double Dis = sqrt((a[1].X - a[0].X) * (a[1].X - a[0].X) + (a[1].Y - a[0].Y) * (a[1].Y - a[0].Y));
	//X坐标相同 则法向量为x轴方向
	if (a[0].X == a[1].X)
	{
		b.X = 1;
		b.Y = 0;
	}
	//y坐标相同  y轴方向
	else if (a[0].Y == a[1].Y)
	{
		b.X = 0;
		b.Y = 1;
	}
	//都不同 则为根据 k1*k2=-1则 单位向量初始化为
	else
	{
		b.X = -(a[1].Y - a[0].Y) / Dis;
		b.Y = (a[1].X - a[0].X) / Dis;
	}

	//定义一个与线段同方向的单位向量
	vector<THRVALUE> lineP(2);
	lineP[0].X = 0;
	lineP[0].Y = 0;
	lineP[1].X = (a[1].X - a[0].X) / Dis;
	lineP[1].Y = (a[1].Y - a[0].Y) / Dis;
	int judgePoint;
	//点b()在单位向量linp的左侧还是右侧
	judgePoint = (int)(lineP[0].X * (b.Y - lineP[1].Y) + lineP[1].X* (lineP[0].Y - b.Y) + b.X * (lineP[1].Y - lineP[0].Y));

	//与上断层在同一侧 法向量的方向需要指向上断层的方向
	if (judgePoint * Position > 0)
	{
		//不做任何处理
	}
	else
	{
		//否则 条换法向量的方向
		b.X = -b.X;
		b.Y = -b.Y;
	}
}

void Jie_Ds::ReviseWires(int Num, vector<vector<double>>& faultSu, double Position)
{
	vector<THRVALUE> overA(2);  //存储断层上相邻两点
	THRVALUE overB; //存储相邻断层两点 的垂直单位向量方向指向上盘
	int insertCout = 8;
	vector<THRVALUE> newFault(insertCout * (m_FaultData[Num].TData.size() - 1) + 1);
	vector<TWOVALUE> newFaultVec(newFault.size());
	//遍历断层中线的每条线段
	for (int i = 0; i < (int)m_FaultData[Num].TData.size() - 1; i++)
	{
		overA[0].X = m_FaultData[Num].TData[i].X;
		overA[0].Y = m_FaultData[Num].TData[i].Y;
		overA[1].X = m_FaultData[Num].TData[i + 1].X;
		overA[1].Y = m_FaultData[Num].TData[i + 1].Y;

		//计算一个此线段的的法向量方向指向正断层的单位向量
		NormalVector(overA, overB, Position);
		//给插入的断层点赋坐标值和属性值 用线性插值
		for (int j = 1; j <= insertCout; j++)
		{
			newFault[i * insertCout + j].X = m_FaultData[Num].TData[i].X + (m_FaultData[Num].TData[i + 1].X - m_FaultData[Num].TData[i].X) * j / insertCout;
			newFault[i* insertCout + j].Y = m_FaultData[Num].TData[i].Y + (m_FaultData[Num].TData[i + 1].Y - m_FaultData[Num].TData[i].Y) * j / insertCout;
			newFault[i * insertCout + j].Z = m_FaultData[Num].TData[i].Z + (m_FaultData[Num].TData[i + 1].Z - m_FaultData[Num].TData[i].Z) * j / insertCout;
			//把此单位向量赋值给newFaultVec 的 0,1
			newFaultVec[i * insertCout + j].X = overB.X;
			newFaultVec[i * insertCout + j].Y = overB.Y;
		}
	}

	double distance;
	double cosFault;
	double avilityVal;
	double disSum;
	//遍历所有角点网格坐标值 计算修正值
	for (int i = 1; i < (int)faultSu.size(); i++)
	{
		faultSu[i].resize(m_GridPoint.size());
		for (int k = 1; k < (int)faultSu.size(); k++)
		{
			avilityVal = 0;
			disSum = 0;
			for (int j = 1; j < (int)newFault.size(); j++)
			{
				// 待插值点到估计点的距离
				distance = sqrt((newFault[j].X - m_GridPoint[i][k].X) * (newFault[j].X - m_GridPoint[i][k].X)
					+ (newFault[j].Y - m_GridPoint[i][k].Y) * (newFault[j].Y - m_GridPoint[i][k].Y));

				cosFault = (m_GridPoint[i][k].X - newFault[j].X) / distance * newFaultVec[j].X
					+ (m_GridPoint[i][k].Y - newFault[j].Y) / distance * newFaultVec[j].Y;

				avilityVal = (newFault[j].Z / 2 * cosFault) / (distance * distance) + avilityVal;

				disSum = disSum + 1 / (distance * distance);
			}
			faultSu[i][k] = avilityVal / disSum * pow((-1.0), (m_FaultData[Num].mark + 2));
		}
	}
}

void Jie_Ds::FaultWires(int Num, vector<Data>& oriData, vector<THRVALUE>& faultSu, double Position)
{
	vector<THRVALUE> overA(2);  //存储断层上相邻两点
	THRVALUE overB; //存储相邻断层两点 的垂直单位向量方向指向上盘
	int insertCout = 8;
	vector<THRVALUE> newFault(insertCout * (m_FaultData[Num].TData.size() - 1) + 1);
	vector<TWOVALUE> newFaultVec(newFault.size());
	//遍历断层中线的每条线段
	for (int i = 0; i < (int)m_FaultData[Num].TData.size() - 1; i++)
	{
		//取当前断层的中间断层线的两个点
		overA[0].X = m_FaultData[Num].TData[i].X;
		overA[0].Y = m_FaultData[Num].TData[i].Y;
		overA[1].X = m_FaultData[Num].TData[i + 1].X;
		overA[1].Y = m_FaultData[Num].TData[i + 1].Y;

		//计算一个此线段的的法向量方向指向正断层的单位向量，存储在overB中
		NormalVector(overA, overB, Position);
		//给插入的断层点赋坐标值和属性值 用线性插值
		for (int j = 1; j <= insertCout; j++)
		{
			newFault[i * insertCout + j].X = m_FaultData[Num].TData[i].X + (m_FaultData[Num].TData[i + 1].X - m_FaultData[Num].TData[i].X) * j / insertCout;
			newFault[i * insertCout + j].Y = m_FaultData[Num].TData[i].Y + (m_FaultData[Num].TData[i + 1].Y - m_FaultData[Num].TData[i].Y) * j / insertCout;
			newFault[i * insertCout + j].Z = m_FaultData[Num].TData[i].Z + (m_FaultData[Num].TData[i + 1].Z - m_FaultData[Num].TData[i].Z) * j / insertCout;
			//把此单位向量赋值给newFaultVec 的 0,1
			//在此段内每一个点处的指向上盘的单位法向量都是一样的
			newFaultVec[i * insertCout + j].X = overB.X;
			newFaultVec[i * insertCout + j].Y = overB.Y;
		}
	}

	//把井位点坐标复制给 faultsu	
	faultSu.resize(oriData.size());
	for (int i = 0; i < (int)oriData.size(); i++)
	{
		faultSu[i].X = oriData[i].X;
		faultSu[i].Y = oriData[i].Y;
		faultSu[i].Z = oriData[i].Z;
	}

	//按逆断层
	//用加权反距离插值
	double distance;
	double cosFault;
	double avilityVal;
	double disSum;
	//计算所有离散点属性值
	for (int i = 0; i < (int)faultSu.size(); i++)
	{
		avilityVal = 0;
		disSum = 0;
		//用新插入的点做参股点
		for (int j = 1; j < (int)newFault.size(); j++)
		{
			//计算新构造的断层线上的每个点到此井位点的距离
			distance = sqrt((newFault[j].X - faultSu[i].X) * (newFault[j].X - faultSu[i].X)
				+ (newFault[j].Y - faultSu[i].Y) * (newFault[j].Y - faultSu[i].Y));
			//若距离很近，则直接进行Z分量的赋值，这里的Z分量指的应该是落差值
			if (abs(distance) < 0.0001)
			{
				faultSu[i].Z = newFault[j].Z;
				break;
			}
			//井位点和插值后的断层点构成的单位向量和法向量的点积
			//https://wenku.baidu.com/view/dd5572e183d049649b6658ad.html
			cosFault = (faultSu[i].X - newFault[j].X) / distance * newFaultVec[j].X +
				(faultSu[i].Y - newFault[j].Y) / distance * newFaultVec[j].Y;

			avilityVal = (newFault[j].Z / 2 * cosFault) / (distance * distance) + avilityVal;

			//这里只是对于距离平方的倒数值进行叠加
			disSum = disSum + 1 / (distance * distance);
		}

		//此离散点的修正值
		//mark表示是正断层还是逆断层  正断层为1 逆断层为0
		vector<TWOVALUE> UData;
		faultSu[i].Z = avilityVal / disSum * pow((-1.0), m_FaultData[Num].mark + 2);

	}
}


//克里金断层估值
void Jie_Ds::FaultKriking()
{
	//如果没有断层数据，采用无断层的克里金估值
	if ((int)m_FaultData.size() <= 0)	
	{
		EvaluateNoFault();
		return;
	}
	//采用有断层的克里金估值
	double Position;
	vector<Data> suV;
	vector<THRVALUE> faultTemp;
	suV.assign(m_oriData.begin(), m_oriData.end());//赋值操作   原始数据

	int XM = m_XNum;
	int	YM = m_YNum;

	vector<THRVALUE> faultSu(suV.size());
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		//提取断层数组中的单个断层数据
		//判断 上盘在中线的哪一侧
		//这里其实是用上盘中的一个点来进行判断在中线上两个点构成的直线的哪一侧，这个公式化简后即为网上的公式
		//https://blog.csdn.net/qq_27606639/article/details/80905311
		Position = m_FaultData[i].TData[0].X * (m_FaultData[i].UData[1].Y - m_FaultData[i].TData[1].Y)
			+ m_FaultData[i].TData[1].X * (m_FaultData[i].TData[0].Y - m_FaultData[i].UData[1].Y)
			+ m_FaultData[i].UData[1].X * (m_FaultData[i].TData[1].Y - m_FaultData[i].TData[0].Y);
		//断层中线特征属性,原始数据,存储输出数据
		//根据两断层落差估计整体估计值修正值
		FaultWires(i, suV, faultTemp, Position);
		//把每一个断层影响的离散点修正值叠加，为断层影响的修正值
		for (int j = 0; j < (int)suV.size(); j++)
			faultSu[j].Z = faultSu[j].Z + faultTemp[j].Z;

		faultTemp.clear();
	}

	//利用修正值 调整离散点属性值为没有断层前的属性值
	for (int i = 0; i < (int)suV.size(); i++)
		suV[i].Z = suV[i].Z - faultSu[i].Z;
	std::ofstream out("data//fault_test//cor_afterP.obj");
	for (int i = 0; i < suV.size(); i++)
		out << "v " << suV[i].X << " " << suV[i].Y << " " << "0" << endl;
	out.close();
	vector<double> ni;
	//计算插值时的每一项前面的系数
	PreMatrix(suV, ni);

	faultSu.clear();
	faultSu.resize(XM);


	//计算出断层影响的 所有角点网格坐标点的修正值  解释同上
	vector<vector<double>> faultT(m_GridPoint.size());
	vector<vector<double>> faultS(XM + 1);

	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		faultT.resize(m_GridPoint.size());
		//提取断层数组中的单个断层数据
		Position = m_FaultData[i].TData[0].X * (m_FaultData[i].UData[1].Y - m_FaultData[i].TData[1].Y)
			+ m_FaultData[i].TData[1].X * (m_FaultData[i].TData[0].Y - m_FaultData[i].UData[1].Y)
			+ m_FaultData[i].UData[1].X * (m_FaultData[i].TData[1].Y - m_FaultData[i].TData[0].Y);
		//计算此断层影响的角点网格属性值的修正值
		ReviseWires(i, faultT, (int)Position);
		//把断层影响角点网格点的属性值叠加
		for (int k = 1; k <= XM; k++)
		{
			faultS[k].resize(YM + 1);
			for (int j = 1; j <= YM; j++)
			{
				faultS[k][j] = faultS[k][j] + faultT[k][j];
			}
		}
		faultT.clear();
	}
	for (int i = 1; i <= XM; i++)
	{
		for (int j = 1; j <= YM; j++)
		{
			TWOVALUE D;
			D.X = m_GridPoint[i][j].X;
			D.Y = m_GridPoint[i][j].Y;
			//if (!IsInside(D))		//若非断层内部点
			{
				//估计此点的属性值    没有断层的估计值 -/+ 修正值			
				m_GridPoint[i][j].Z = InsertEst(suV, D, ni) - faultS[i][j];
			}
			//else
			{
				//正常估值的属性值
				//m_GridPoint[i][j].Z = InsertEst(suV, D, ni) ;	
			}
			if (m_ZMin > m_GridPoint[i][j].Z)
			{
				m_ZMin = m_GridPoint[i][j].Z;
			}

			if (m_ZMax < m_GridPoint[i][j].Z)
			{
				m_ZMax = m_GridPoint[i][j].Z;
			}

		}
	}
	m_suV = suV;
	m_ni = ni;				//20130913 应用于外推法
}

//得到一个点的估计值
void Jie_Ds::ReviseWires(int Num, THRVALUE& Point, double Position)
{
	vector<THRVALUE> overA(2);  //存储断层上相邻两点
	THRVALUE overB; //存储相邻断层两点的垂直单位向量方向指向上盘
	int insertCout = 8;
	vector<THRVALUE> newFault(insertCout * (m_FaultData[Num].TData.size() - 1) + 1);
	vector<TWOVALUE> newFaultVec(newFault.size());
	//遍历断层中线的每条线段
	for (int i = 0; i < (int)m_FaultData[Num].TData.size() - 1; i++)
	{
		overA[0].X = m_FaultData[Num].TData[i].X;
		overA[0].Y = m_FaultData[Num].TData[i].Y;
		overA[1].X = m_FaultData[Num].TData[i + 1].X;
		overA[1].Y = m_FaultData[Num].TData[i + 1].Y;

		//计算一个此线段的的法向量方向指向正断层的单位向量
		NormalVector(overA, overB, Position);
		//给插入的断层点赋坐标值和属性值 用线性插值
		for (int j = 1; j <= insertCout; j++)
		{
			newFault[i * insertCout + j].X = m_FaultData[Num].TData[i].X + (m_FaultData[Num].TData[i + 1].X - m_FaultData[Num].TData[i].X) * j / insertCout;
			newFault[i* insertCout + j].Y = m_FaultData[Num].TData[i].Y + (m_FaultData[Num].TData[i + 1].Y - m_FaultData[Num].TData[i].Y) * j / insertCout;
			newFault[i * insertCout + j].Z = m_FaultData[Num].TData[i].Z + (m_FaultData[Num].TData[i + 1].Z - m_FaultData[Num].TData[i].Z) * j / insertCout;
			//把此单位向量赋值给newFaultVec 的 0,1
			newFaultVec[i * insertCout + j].X = overB.X;
			newFaultVec[i * insertCout + j].Y = overB.Y;
		}
	}

	double distance;
	double cosFault;
	double avilityVal;
	double disSum;
	//遍历所有角点网格坐标值 计算修正值
	avilityVal = 0;
	disSum = 0;
	for (int j = 1; j < (int)newFault.size(); j++)
	{
		// 待插值点到估计点的距离
		distance = sqrt((newFault[j].X - Point.X) * (newFault[j].X - Point.X)
			+ (newFault[j].Y - Point.Y) * (newFault[j].Y - Point.Y));
		if (distance == 0.0)
		{
			continue;
		}

		cosFault = (Point.X - newFault[j].X) / distance * newFaultVec[j].X
			+ (Point.Y - newFault[j].Y) / distance * newFaultVec[j].Y;

		avilityVal = (newFault[j].Z / 2 * cosFault) / (distance * distance) + avilityVal;
		disSum = disSum + 1 / (distance * distance);
	}
	Point.Z = Point.Z + avilityVal / disSum * pow((-1.0), (m_FaultData[Num].mark + 1));
}

//得到一个点的估值
void Jie_Ds::EvaluateFault(THRVALUE& Point)
{
	double Position;
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		//提取断层数组 中的单个断层数据
		//判断 上盘在中线的哪侧 左侧为 1 右侧为-1
		Position = m_FaultData[i].TData[0].X * (m_FaultData[i].UData[1].Y - m_FaultData[i].TData[1].Y)
			+ m_FaultData[i].TData[1].X * (m_FaultData[i].TData[0].Y - m_FaultData[i].UData[1].Y)
			+ m_FaultData[i].UData[1].X * (m_FaultData[i].TData[1].Y - m_FaultData[i].TData[0].Y);
		//断层中线特征属性,原始数据,存储输出数据
		//根据 两断层落差估计整体估计值修正值
		ReviseWires(i, Point, Position);
	}
}

//距离反比断层估值
void Jie_Ds::FaultInvDis()
{
	if ((int)m_FaultData.size() <= 0)
	{
		EvaluateNoFault();
		return;
	}
	double Position;
	vector<Data> suV;
	vector<THRVALUE> faultTemp;
	suV.assign(m_oriData.begin(), m_oriData.end());

	int XM = m_XNum;
	int	YM = m_YNum;

	vector<THRVALUE> faultSu(suV.size());
	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{

		//提取断层数组 中的单个断层数据
		//判断 上盘在中线的哪侧 左侧为 1 右侧为-1
		Position = m_FaultData[i].TData[0].X * (m_FaultData[i].UData[1].Y - m_FaultData[i].TData[1].Y)
			+ m_FaultData[i].TData[1].X * (m_FaultData[i].TData[0].Y - m_FaultData[i].UData[1].Y)
			+ m_FaultData[i].UData[1].X * (m_FaultData[i].TData[1].Y - m_FaultData[i].TData[0].Y);
		//断层中线特征属性,原始数据,存储输出数据
		//根据 两断层落差估计整体估计值修正值
		FaultWires(i, suV, faultTemp, Position);
		//把两个断层影响的离散点修正值叠加 为断层影响的修正值
		for (int j = 0; j < (int)suV.size(); j++)
			faultSu[j].Z = faultSu[j].Z + faultTemp[j].Z;

		faultTemp.clear();
	}
	//利用修正值 调整离散点属性值为没有断层前的属性值
	for (int i = 0; i < (int)suV.size(); i++)
		suV[i].Z = suV[i].Z - faultSu[i].Z;

	faultSu.clear();
	faultSu.resize(XM);

	//计算出断层影响的 所有角点网格坐标点的修正值  解释同上
	vector<vector<double>> faultT(m_GridPoint.size());
	vector<vector<double>> faultS(XM + 1);

	for (int i = 0; i < (int)m_FaultData.size(); i++)
	{
		faultT.resize(m_GridPoint.size());
		//提取断层数组 中的单个断层数据
		Position = m_FaultData[i].TData[0].X * (m_FaultData[i].UData[1].Y - m_FaultData[i].TData[1].Y)
			+ m_FaultData[i].TData[1].X * (m_FaultData[i].TData[0].Y - m_FaultData[i].UData[1].Y)
			+ m_FaultData[i].UData[1].X * (m_FaultData[i].TData[1].Y - m_FaultData[i].TData[0].Y);
		//计算此断层影响的角点网格属性值 的修正值
		ReviseWires(i, faultT, (int)Position);
		//把 断层影响角点网格点的属性值 叠加
		for (int k = 1; k <= XM; k++)
		{
			faultS[k].resize(YM + 1);
			for (int j = 1; j <= YM; j++)
			{
				faultS[k][j] = faultS[k][j] + faultT[k][j];
			}
		}
		faultT.clear();
	}
	TWOVALUE D;
	m_suV = suV;
	for (int i = 1; i <= m_XNum; i++)
	{
		for (int j = 1; j <= m_YNum; j++)
		{
			D.X = m_GridPoint[i][j].X;
			D.Y = m_GridPoint[i][j].Y;
			//估计此点的属性值    没有断层的估计值 + 修正值
			//m_GridPoint[i][j].Z = InsertEst(Suv, D, ni);//全部井位点的克里金估值
			//m_GridPoint[i][j].Z = Well_Near_K(D);		//邻井范围内的克里金估值
			//m_GridPoint[i][j].Z = DisInv(D);
			m_GridPoint[i][j].Z = DisInv(D)+ faultS[i][j];			//邻井范围内的距离反比估值
			//EvaluateFault(m_GridPoint[i][j]);			//添加断层估值
			if (m_ZMin > m_GridPoint[i][j].Z)
			{
				//cout << "最大值发生了更迭" << endl;
				m_ZMin = m_GridPoint[i][j].Z;
			}

			if (m_ZMax < m_GridPoint[i][j].Z)
			{
				//cout << "最小值发生了更迭" << endl;
				m_ZMax = m_GridPoint[i][j].Z;
			}
		}
	}
}

//含断层点的估值
void Jie_Ds::EvaluateFault()
{
	if ((int)m_oriData.size() < 800)//800口井可以使用克里金方法，速度4s左右可以接受
	{
		m_IsK = true;				//点数较少使用克里金估值
	}
	else
	{
		m_IsK = false;				//点数太多使用幂距离反比估值
	}
	if (m_IsK)
	{
		FaultKriking();		//克里金断层估值
	}
	else
	{
		FaultInvDis();		//距离反比估值
	}
}

//点是否在网格内部
BOOL Jie_Ds::ISInside(TWOVALUE A, TWOVALUE GridIJ[5])
{
	double x = A.X;
	double y = A.Y;
	//通过面积判断点是否在网格（i,j）内
	//double xmax = A.X;
	//double xmin = A.X;
	//double ymax = A.Y;
	//double ymin = A.Y;
	//for (int i = 0 ; i < 4 ; i ++)
	//{
	//	double tx = GridIJ[i].X;
	//	double ty = GridIJ[i].Y;
	//	if (tx > xmax)
	//	{
	//		xmax = tx;
	//	}
	//	else if (tx < xmin)
	//	{
	//		xmin = tx
	//	}
	//}
	double x1 = GridIJ[0].X - A.X;
	double x2 = GridIJ[1].X - A.X;
	double x3 = GridIJ[2].X - A.X;
	double x4 = GridIJ[3].X - A.X;

	double y1 = GridIJ[0].Y - A.Y;
	double y2 = GridIJ[1].Y - A.Y;
	double y3 = GridIJ[2].Y - A.Y;
	double y4 = GridIJ[3].Y - A.Y;


	double s1 = abs((y4 - y1)*x2 - (x4 - x1)*y2 + y1*x4 - y4*x1);
	double s2 = abs((y4 - y3)*x2 - (x4 - x3)*y2 + y3*x4 - y4*x3);
	double s = s1 + s2;

	x = 0;
	y = 0;

	double ss1 = abs((y2 - y1)*x - (x2 - x1)*y + y1*x2 - y2*x1);
	double ss2 = abs((y3 - y2)*x - (x3 - x2)*y + y2*x3 - y3*x2);
	double ss3 = abs((y3 - y4)*x - (x3 - x4)*y + y4*x3 - y3*x4);
	double ss4 = abs((y1 - y4)*x - (x1 - x4)*y + y4*x1 - y1*x4);
	double ss = ss1 + ss2 + ss3 + ss4;

	double d = abs(s - ss);
	if (d<0.00000000001)
	{
		return TRUE;
	}
	return FALSE;
}

//对交点进行排序
void Jie_Ds::Sort(vector<double>&Mark)
{
	vector<double>NewMark;
	for (int i = 0; i < (int)Mark.size() - 1; i++)
	{
		for (int j = i + 1; j < (int)Mark.size(); j++)
		{
			if (Mark[i]>Mark[j])
			{
				double midd = Mark[j];
				Mark[j] = Mark[i];
				Mark[i] = midd;
			}
		}
	}
}


//通过克里金得到点Point所在的属性值 20150323
void Jie_Ds::GetValueByK(THRVALUE &Point)
{
	if (m_IsK)
	{
		TWOVALUE D;
		D.X = Point.X;
		D.Y = Point.Y;
		Point.Z = InsertEst(m_suV, D, m_ni) /*+ Point.Z*/;//试用克里金插值
		EvaluateFault(Point);							//添加断层估值
	}
	else
	{
		TWOVALUE D;
		D.X = Point.X;
		D.Y = Point.Y;
		//Point.Z = InsertEst(m_suV, D, m_ni) + Point.Z;//20140813,
		Point.Z = DisInv(D);                             //使用邻域的距离反比方法来估值
		EvaluateFault(Point);							//添加断层估值
	}
}

//得到网格内真实的坐标
void Jie_Ds::GetFulatInsidePoint(vector<double>Mark, FalutLine LineOne, GridFL &AllLine, TWOVALUE GridIJ[5])
{
	for (int i = 0; i < (int)Mark.size() - 1; i = i + 2)
	{
		SingleFalut Line;
		if ((int)Mark[i] == (int)Mark[i + 1])//两交点之间没有断层点，这种情况出现在断层线上的两个点直接穿过两条网格边
		{
			int Star = (int)Mark[i];
			int End = (int)Mark[i] + 1;
			TWOVALUE SP = LineOne.FLine[Star];
			TWOVALUE EP = LineOne.FLine[End];
			double dx = EP.X - SP.X;
			double dy = EP.Y - SP.Y;
			double t2 = Mark[i + 1] - Star;
			double t1 = Mark[i] - Star;
			/*--------------------------------------------*/
			//20140915
			//TWOVALUE t1p;
			//t1p.X = SP.X + t1*dx;
			//t1p.Y = SP.Y + t1*dy;

			//TWOVALUE t2p;
			//t2p.X = SP.X + t2*dx;
			//t2p.Y = SP.Y + t2*dy;
			THRVALUE t1p;
			t1p.X = SP.X + t1*dx;
			t1p.Y = SP.Y + t1*dy;
			t1p.Z = 0.0;
			GetValueByK(t1p);
			THRVALUE t2p;
			t2p.X = SP.X + t2*dx;
			t2p.Y = SP.Y + t2*dy;
			t2p.Z = 0.0;
			GetValueByK(t2p);
			/*--------------------------------------------*/
			Line.FLine.push_back(t1p);
			Line.FLine.push_back(t2p);
		}
		else							//两交点之间存在断层点
		{
			int Star = (int)Mark[i];
			int End = (int)Mark[i] + 1;
			TWOVALUE SP = LineOne.FLine[Star];
			TWOVALUE EP = LineOne.FLine[End];
			double dx = EP.X - SP.X;
			double dy = EP.Y - SP.Y;
			double t1 = Mark[i] - Star;
			//double t2 = Mark[i+1]-Star;
			/*--------------------------------------------*/
			//20140915
			//TWOVALUE t1p;
			//t1p.X = SP.X + t1*dx;
			//t1p.Y = SP.Y + t1*dy;
			THRVALUE t1p;
			t1p.X = SP.X + t1*dx;
			t1p.Y = SP.Y + t1*dy;
			t1p.Z = 0.0;
			GetValueByK(t1p);
			/*--------------------------------------------*/
			Line.FLine.push_back(t1p);
			for (int h = End; h <= (int)Mark[i + 1]; h++)
			{
				TWOVALUE A = LineOne.FLine[h];
				if (!ISInside(A, GridIJ))
				{
					AllLine.FLines.push_back(Line);//预防首末点在断层上的情况20130816,
					break;
				}
				/*--------------------------------------------*/
				//20140915
				THRVALUE B;
				B.X = A.X;
				B.Y = A.Y;
				B.Z = 0.0;
				GetValueByK(B);
				//Line.FLine.push_back(A);
				Line.FLine.push_back(B);
				/*--------------------------------------------*/
			}
			if ((int)Mark[i + 1] == (int)LineOne.FLine.size() - 1)//防止末点
			{
				AllLine.FLines.push_back(Line);
				return;
			}
			Star = (int)Mark[i + 1];
			End = (int)Mark[i + 1] + 1;
			SP = LineOne.FLine[Star];
			EP = LineOne.FLine[End];
			dx = EP.X - SP.X;
			dy = EP.Y - SP.Y;
			t1 = Mark[i + 1] - Star;
			t1p.X = SP.X + t1*dx;
			t1p.Y = SP.Y + t1*dy;

			/*--------------------------------------------*/
			//20140915
			t1p.Z = 0.0;
			GetValueByK(t1p);
			/*--------------------------------------------*/
			Line.FLine.push_back(t1p);
		}
		AllLine.FLines.push_back(Line);//将该断层线段加入
	}
}


//得到网格内部一条断层线的信息
void Jie_Ds::GetOneCross(int IndexI, int IndexJ, TWOVALUE GridIJ[5], FalutLine LineOne, GridFL &AllLine)
{
	vector<double>mark;
	int NumLineOne = LineOne.FLine.size();
	int Star = 0;
	if (ISInside(LineOne.FLine[0], GridIJ))	//如果首点在网格内，则加入，这里点在网格边上也算作内部
	{
		//cout << "hello" << endl;
		//cout << "v " << LineOne.FLine[1].X << " " << LineOne.FLine[1].Y << " " << "0" << endl;
		//mark.push_back(0.0);             //20130917
		Star = 1;
		//因为首点在网格内，那么一定可以确定索引为0-1,0(size()-1)和size()-2构成的线段可能都会和网格有交点，所以加入第二个点
		LineOne.FLine.push_back(LineOne.FLine[1]);//将第二个点作为最后一个点，重合0-1部分·~使得首尾点所在的线段为一组而不是两组线段
	}
	for (int i = Star/*0*/; i < (int)LineOne.FLine.size() - 1; i++)  //20130918
	{
		TWOVALUE StarA = LineOne.FLine[i];
		TWOVALUE EndA = LineOne.FLine[i + 1];
		double Index = i;
		for (int j = 0; j < 4; j++)
		{
			TWOVALUE StarB = GridIJ[j];
			TWOVALUE EndB = GridIJ[j + 1];
			if (ISIntersect(StarA, EndA, StarB, EndB, Index))
			{
				/*--------------------------------------*/
				//20140916,在此函数中添加网格线上的断层点
				THRVALUE MidPt;
				double t = Index - i;
				MidPt.X = StarA.X + (EndA.X - StarA.X) * t;
				MidPt.Y = StarA.Y + (EndA.Y - StarA.Y) * t;
				MidPt.Z = 0.0;
				GetValueByK(MidPt);
				/*--------------------------------------*/
				mark.push_back(Index);
				Index = i;
				//存储断层是否切分网格边···用于初始等值点选择··
				//初始等值点选择不能选择被断层通过的边··断层两边数据不是线性的
				if (j == 0)
				{
					m_XFault[IndexI][IndexJ] = TRUE;
					//20140916  网格线上的断层点
					m_XFaultPts[IndexI][IndexJ].FLine.push_back(MidPt);
				}
				else if (j == 1)
				{
					m_YFault[IndexI + 1][IndexJ] = TRUE;
					//20140916
					m_YFaultPts[IndexI + 1][IndexJ].FLine.push_back(MidPt);
				}
				else if (j == 2)
				{
					m_XFault[IndexI][IndexJ + 1] = TRUE;
					//20140916
					m_XFaultPts[IndexI][IndexJ + 1].FLine.push_back(MidPt);
				}
				else
				{
					m_YFault[IndexI][IndexJ] = TRUE;
					//20140916
					m_YFaultPts[IndexI][IndexJ].FLine.push_back(MidPt);
				}
			}
		}
		/////*if (ISInside(StarA,GridIJ))
		////{
		////	mark.push_back(i);
		////}*/
	}
	if (ISInside(LineOne.FLine[NumLineOne - 1], GridIJ))	//若末点在网格内，则加入
	{
		//mark.push_back(NumLineOne-1);					//20130918
	}
	if (mark.size() <1)//防止断层线跟网格点相交，因为断层线和网格相交的话，应该至少有两个交点
	{
		return;
	}
	Sort(mark);//mark之内存储的是断层线上的虚拟比例值
	GetFulatInsidePoint(mark, LineOne, AllLine, GridIJ);//得到网格内的断层点以及断层跟网格的交点信息
														//if (mark.size()>1)
														//{
														//	m_NoFault[IndexI][IndexJ]
														//}
}

//得到网格内部的交点信息
void Jie_Ds::GetAllCross(int IndexI, int IndexJ, TWOVALUE GridIJ[5], GridFL &AllLine)
{
	for (int i = 0; i < (int)m_vecFaultLines.size(); i++)
	{
		FalutLine LineOne = m_vecFaultLines[i];
		GetOneCross(IndexI, IndexJ, GridIJ, LineOne, AllLine);	//判断网格IndexI,IndexJ内部是否跟第i条断层线相交
	}
}

//操作一个断层完成上述
void Jie_Ds::SetOneFault(int IndexI, int IndexJ, GridFL &ALLLine)
{
	TWOVALUE GridIJ[5];//顺时针存储，首尾相连,所以数组中有五个值
	GridIJ[0].X = m_GridPoint[IndexI][IndexJ].X;
	GridIJ[1].X = m_GridPoint[IndexI + 1][IndexJ].X;
	GridIJ[2].X = m_GridPoint[IndexI + 1][IndexJ + 1].X;
	GridIJ[3].X = m_GridPoint[IndexI][IndexJ + 1].X;
	GridIJ[4].X = m_GridPoint[IndexI][IndexJ].X;

	GridIJ[0].Y = m_GridPoint[IndexI][IndexJ].Y;
	GridIJ[1].Y = m_GridPoint[IndexI + 1][IndexJ].Y;
	GridIJ[2].Y = m_GridPoint[IndexI + 1][IndexJ + 1].Y;
	GridIJ[3].Y = m_GridPoint[IndexI][IndexJ + 1].Y;
	GridIJ[4].Y = m_GridPoint[IndexI][IndexJ].Y;
	GetAllCross(IndexI, IndexJ, GridIJ, ALLLine);		//得到所有断层线
}

//点A是否在断层Line内部
BOOL Jie_Ds::IsInside(TWOVALUE A, FalutLine Line)
{
	double Xmax = Line.FLine[0].X;
	double Xmin = Line.FLine[0].X;
	double Ymax = Line.FLine[0].Y;
	double Ymin = Line.FLine[0].Y;
	for (int i = 1; i < (int)Line.FLine.size(); i++)
	{
		if (Line.FLine[i].X > Xmax)
		{
			Xmax = Line.FLine[i].X;
		}
		else if (Line.FLine[i].X < Xmin)
		{
			Xmin = Line.FLine[i].X;
		}

		if (Line.FLine[i].Y > Ymax)
		{
			Ymax = Line.FLine[i].Y;
		}
		else if (Line.FLine[i].Y < Ymin)
		{
			Ymin = Line.FLine[i].Y;
		}
	}
	if (A.X > Xmax || A.X <Xmin || A.Y>Ymax || A.Y < Ymin)
	{
		return FALSE;
	}
	//射线判断是否在内部
	TWOVALUE B = A;
	//B.X = m_XMin - 200.0;     //AB射线
	B.X = Xmin - 200.0;     //AB射线 20140805,
	B.Y = Ymin - 200.0;     //AB射线 20140805,
	int Sum = 0;
	for (int i = 0; i < (int)Line.FLine.size() - 1; i++)
	{
		double t = 0;
		TWOVALUE LineA = Line.FLine[i];
		TWOVALUE LineB = Line.FLine[i + 1];
		if (ISIntersect(LineA, LineB, A, B, t))
		{
			//if (t<0.00000001)//防止跟首点相交，交点在端点上，只收入一个在尾点上的
			//{
			//	continue;
			//}
			Sum = Sum + 1;
		}
	}
	if (Sum % 2 == 1)//交点为奇数个则在内部
	{
		return TRUE;
	}
	return FALSE;

}
//点A是否在断层内部
BOOL Jie_Ds::IsInside(TWOVALUE RealyPoint)
{
	for (int i = 0; i < (int)m_vecFaultLines.size(); i++)
	{
		FalutLine Line = m_vecFaultLines[i];
		if (Line.IsClose)
		{
			if (IsInside(RealyPoint, Line))
			{
				return TRUE;
			}
		}
	}
	return FALSE;

}
//判断断层线两边的网格点是否在断层内部20140916
void Jie_Ds::JudgeFaultGridIn()
{
	TWOVALUE A, B;
	for (int j = 1; j <= m_YNum; j++)			//横边上的网格点
	{
		for (int i = 1; i < m_XNum; i++)
		{
			if (m_XFault[i][j])//此横向网格边有断层线通过
			{
				//这里要清楚一点，m_GridPoint第一级是列下标，第二级是行下标
				A.X = m_GridPoint[i][j].X;
				A.Y = m_GridPoint[i][j].Y;

				B.X = m_GridPoint[i + 1][j].X;
				B.Y = m_GridPoint[i + 1][j].Y;

				if (IsInside(A))
				{
					m_GridInFault[i][j] = TRUE;		//i,j网格点在断层内
				}
				else if (IsInside(B))
				{
					m_GridInFault[i + 1][j] = TRUE;	//i + 1,j网格点在断层内
				}
			}
		}
	}

	for (int i = 1; i <= m_XNum; i++)			//
	{
		for (int j = 1; j < m_YNum; j++)
		{
			if (m_YFault[i][j])
			{
				A.X = m_GridPoint[i][j].X;
				A.Y = m_GridPoint[i][j].Y;

				B.X = m_GridPoint[i][j + 1].X;
				B.Y = m_GridPoint[i][j + 1].Y;
				if (IsInside(A))
				{
					m_GridInFault[i][j] = TRUE;		//i,j网格点在断层内
				}
				else if (IsInside(B))
				{
					m_GridInFault[i][j + 1] = TRUE;	//i,j + 1网格点在断层内
				}
			}
		}
	}
	
	for (int i = 1; i <= m_XNum; i++)			
	{
		for (int j = 1; j <= m_YNum; j++)
		{
				A.X = m_GridPoint[i][j].X;
				A.Y = m_GridPoint[i][j].Y;
				if (IsInside(A))
				{
					m_GridInFault[i][j] = TRUE;		//i,j网格点在断层内
					if (m_XFault[i][j] != true)
						m_XFault[i][j] = true;
					if (m_YFault[i][j] != true)
						m_YFault[i][j] = true;
				}
		}
	}
	std::ofstream out("data//fault_test//pinfault.obj");
	for (int i = 0; i < m_XNum; i++)
	{
		for (int j = 0; j < m_YNum; j++)
		{
			if (m_GridInFault[i][j] == TRUE)
				out << "v " << fixed << setprecision(5)<< m_GridPoint[i][j].X << " " << fixed << setprecision(5)<<m_GridPoint[i][j].Y << " " << "0" << endl;
		}
	}

}
//将断层与网格的交点加入网格断层线数组中
void Jie_Ds::SetNewFault()
{
	m_NoFault.resize(m_XNum + 3);				//网格内是否含有断层的标记 //判断每个网格内是否含有断层，有则存储断层线的序号
	m_XFault.resize(m_XNum + 3);				//X网格边是否含有断层的标记20130913
	m_YFault.resize(m_XNum + 3);				//Y网格边是否含有断层的标记
	m_XFaultPts.resize(m_XNum + 3);			//存储每个X网格线是断层数据20140916
	m_YFaultPts.resize(m_XNum + 3);			//存储每个Y网格线是断层数据20140916
	m_GridInFault.resize(m_XNum + 3);			//存储每个网格点是否在断层内20140916
	for (int i = 0; i < m_XNum + 3; i++)
	{
		m_NoFault[i].resize(m_YNum + 3);

		m_XFault[i].resize(m_YNum + 3);		//X网格边是否含有断层的标记
		m_YFault[i].resize(m_YNum + 3);		//Y网格边是否含有断层的标记
		m_XFaultPts[i].resize(m_XNum + 3);	//存储每个X网格线是断层数据20140916
		m_YFaultPts[i].resize(m_XNum + 3);	//存储每个Y网格线是断层数据20140916
		m_GridInFault[i].resize(m_XNum + 3);	//存储每个网格点是否在断层内20140916
		for (int j = 0; j < m_YNum + 3; j++)
		{
			m_NoFault[i][j] = -1;		//表示不含断层
			m_XFault[i][j] = FALSE;	//表示不含断层
			m_YFault[i][j] = FALSE;	//表示不含断层			20130913
			m_GridInFault[i][j] = FALSE;	//表示网格点不在断层内	20140916
		}
	}

	for (int i = 1; i < m_XNum; i++)
	{
		for (int j = 1; j < m_YNum; j++)
		{
			GridFL AllLine;
			SetOneFault(i, j, AllLine);		//判断网格i,j内是否有断层线
			if ((int)AllLine.FLines.size()>0)
			{
				m_vecGridFL.push_back(AllLine);//这里应注意，每一个网格内可能存储多条断层线
				m_NoFault[i][j] = m_vecGridFL.size() - 1;
			}
		}
	}
	//cout << m_vecGridFL.size() << endl;
	/*std::ofstream out("data//fault//test.obj");
	int k = 2;
	cout << m_vecGridFL[k].FLines.size() << endl;
	for (int i = 0; i < m_vecGridFL[k].FLines.size(); i++)
		for (int j = 0; j < m_vecGridFL[k].FLines[i].FLine.size(); j++)
			out << "v " << m_vecGridFL[k].FLines[i].FLine[j].X << " " << m_vecGridFL[k].FLines[i].FLine[j].Y << " " << "0" << endl;
	out.close();*/
	//20140916  判断与断层相交的网格线上的网格点是否在断层内
	JudgeFaultGridIn();
}

void Jie_Ds::GetMinD(vector<double> CosS, vector<double> DisT, int &index)
{
	double MinCos = CosS[0];
	index = 0;
	for (int i = 0; i < (int)CosS.size(); i++)
	{
		if (CosS[i] < MinCos)
		{
			MinCos = CosS[i];
			index = i;
		}
		else if (CosS[i] == MinCos && DisT[i] >= DisT[index])
		{
			index = i;
		}
	}
}
vector<Data> Jie_Ds::Withershins(vector<Data> m_point)
{

	Data Temppoint;
	int Numindex = (int)m_point.size();
	int Num = (int)m_point.size();
	Temppoint = m_point[0];
	vector<Data> Point;
	int StarIndex = 0;
	//选择最小的x,最大的y作为初始点,存储在m_point[0]中
	for (int i = 1; i< (int)m_point.size(); i++)
	{
		if (Temppoint.X == m_point[i].X && Temppoint.Y < m_point[i].Y)
		{
			Temppoint = m_point[i];
			StarIndex = i;
		}
		else if (Temppoint.X > m_point[i].X)
		{
			Temppoint = m_point[i];
			StarIndex = i;
		}
	}
	//计算所有点和该点组成的向量和初始向量（pt = ptStartA-ptEndA，且使得该向量的x值为正值）之间的的夹角，取一个最大的夹角(最小的cos x = (a*b)/(|a|*|b|))
	//这里的初始向量指的应该是取一个y轴正方向的向量即可，比如说（0,1）
	Point.push_back(Temppoint);
	m_point[StarIndex].mark = 1;	//被使用过
	if ((int)m_point.size() == 1)
	{
		return Point;		//处理单个井位估值 20131108
	}
	vector<double> CosS, NiCosS, Dis, NiDis;
	vector<int> IndexS, NiIndexS;
	//找到第二个点，将其放到m_point[1];
	for (int i = 0; i< Num; i++)
	{
		if (m_point[i].mark == 1)
		{
			continue;	//被使用过
		}
		double x1 = 0.0;
		double y1 = 1.0;
		double x2 = m_point[i].X - Point[0].X;
		double y2 = m_point[i].Y - Point[0].Y;
		double d = -x2;		//叉乘结果，这里主要根据符号来判断(x2,y2)所构成的向量在(x1,y1)所构成向量的顺时针还是逆时针方向
							//若小于0，则在顺时针方向，若大于0，则在逆时针方向，若等于0，则共线

		double d1 = sqrt(pow(x1, 2) + pow(y1, 2));
		double d2 = sqrt(pow(x2, 2) + pow(y2, 2));

		if (d1 == 0 || d2 == 0)
		{
			continue;
		}

		double cosa = (x1*x2 + y1*y2) / (d1*d2);//这里应该是此向量和竖直向上的向量夹角的余弦值
		if (d >= 0)
		{
			NiCosS.push_back(cosa);
			NiIndexS.push_back(i);
			NiDis.push_back(d2);
		}
		else
		{
			CosS.push_back(cosa);
			IndexS.push_back(i);
			Dis.push_back(d2);	//存储距离
		}
	}
	if ((int)NiCosS.size() >= 1)
	{
		int Good;
		GetMinD(NiCosS, NiDis, Good);
		Good = NiIndexS[Good];
		Temppoint = m_point[Good];
		m_point[Good].mark = 1;
	}
	else
	{
		int Good;
		GetMinD(CosS, Dis, Good);
		Good = IndexS[Good];
		Temppoint = m_point[Good];
		m_point[Good].mark = 1;
	}
	Point.push_back(Temppoint);
	m_point[StarIndex].mark = 0;	//删除首点被使用痕迹
	for (int j = 2; j < Num + 1; j++)
	{
		NiIndexS.clear(); IndexS.clear();
		NiDis.clear();	  Dis.clear();
		NiCosS.clear();	  CosS.clear();
		for (int i = 0; i < Num; i++)
		{
			if (m_point[i].mark == 1)
			{
				continue;
			}
			double x2 = m_point[i].X - Point[j - 1].X;
			double y2 = m_point[i].Y - Point[j - 1].Y;
			double x1 = Point[j - 1].X - Point[j - 2].X;
			double y1 = Point[j - 1].Y - Point[j - 2].Y;
			double d = x1 * y2 - y1 * x2;		//叉乘结果
			double d1 = sqrt(pow(x1, 2) + pow(y1, 2));
			double d2 = sqrt(pow(x2, 2) + pow(y2, 2));
			if (d1 == 0 || d2 == 0)
			{
				continue;
			}
			double cosa = (-x1*x2 - y1*y2) / (d1*d2);
			if (d > 0)
			{
				NiCosS.push_back(cosa);
				NiIndexS.push_back(i);
				NiDis.push_back(d2);
			}
		}
		if ((int)NiCosS.size() >= 1)
		{
			int Good;
			GetMinD(NiCosS, NiDis, Good);
			Good = NiIndexS[Good];
			Temppoint = m_point[Good];
			m_point[Good].mark = 1;
		}
		else
		{
			return Point;
		}
		Point.push_back(Temppoint);
		if (Point[j].X == Point[0].X && Point[j].Y == Point[0].Y)
		{
			return Point;
		}
	}
	return Point;
}
vector<Data> Jie_Ds::LoadModel(const char* sFileName)
{
	vector<Data> Convexhull;
	std::ifstream in(sFileName);
	std::ifstream in2(sFileName);
	if (!in)
	{
		std::cout << "error of load mesh" << std::endl;
	}
	string str;
	Vector3D point2;

	in >> str;
	if (str == "v")
	{
		Data Points;
		in >> point2.x >> point2.y>>point2.z;
		//cout << point2.x << " " << point2.y << " "<<point2.z<<endl;
		Points.X = point2.x;
		Points.Y = point2.y;
		Points.Z = point2.z;
		Convexhull.push_back(Points);
	}
	while (getline(in, str))
		//while (!in.eof())
	{
		in >> str;
		if (str == "v")
		{
			Data Points;
			in >> point2.x >> point2.y>>point2.z;
			//cout << point2.x << " " << point2.y << " "<<point2.z<<endl;
			Points.X = point2.x;
			Points.Y = point2.y;
			Points.Z = point2.z;
			Convexhull.push_back(Points);
		}

	}
	return Convexhull;
}
void Jie_Ds::Charact()	
{
	double XMin = m_oriData[0].X;//m_oriData中存储原始数据，井位信息
	double XMax = m_oriData[0].X;
	double YMin = m_oriData[0].Y;
	double YMax = m_oriData[0].Y;
	double ZMin = m_oriData[0].Z;
	double ZMax = m_oriData[0].Z;
	for (size_t i = 1; i<m_oriData.size(); i++)
	{
		if (XMin > m_oriData[i].X)
			XMin = m_oriData[i].X;

		if (XMax < m_oriData[i].X)
			XMax = m_oriData[i].X;

		if (YMin > m_oriData[i].Y)
			YMin = m_oriData[i].Y;

		if (YMax < m_oriData[i].Y)
			YMax = m_oriData[i].Y;

		if (ZMin > m_oriData[i].Z)
			ZMin = m_oriData[i].Z;

		if (ZMax < m_oriData[i].Z)
			ZMax = m_oriData[i].Z;
	}

	/*************************************************/
	//是否要判断m_boder的数据？20131112  之前已经有这么一步的赋值操作：m_OriBoder = m_Border;
	for (size_t i = 0; i<m_Border.size(); i++)
	{
		if (XMin > m_Border[i].X)
			XMin = m_Border[i].X;

		if (XMax < m_Border[i].X)
			XMax = m_Border[i].X;

		if (YMin > m_Border[i].Y)
			YMin = m_Border[i].Y;

		if (YMax < m_Border[i].Y)
			YMax = m_Border[i].Y;
	}
	/*************************************************/

	m_XMin = XMin;
	m_XMax = XMax;
	m_YMin = YMin;
	m_YMax = YMax;
	m_ZMin = ZMin;
	m_ZMax = ZMax;
}
void Jie_Ds::DataOpt()
{
	vector<Data> datatemp;
	for (int i = 0; i < m_oriData.size(); i++)
	{
		Data p;
		p.X = m_oriData[i].X-m_XMin;
		p.Y = m_oriData[i].Y-m_YMin;
		p.Z = m_oriData[i].Z;
		datatemp.push_back(p);
	}
	m_oriData.clear();
	m_oriData = datatemp;
}
void Jie_Ds::DataRec()
{
	vector<Data> datatemp;
	for (int i = 0; i < m_oriData.size(); i++)
	{
		Data p;
		p.X = m_oriData[i].X + m_XMin;
		p.Y = m_oriData[i].Y + m_YMin;
		p.Z = m_oriData[i].Z;
		datatemp.push_back(p);
	}
	m_oriData.clear();
	m_oriData = datatemp;
}

void Jie_Ds::CalcBorder()
{
	if (m_Border.size() < 3) // 没有设置边界，计算一个边界
	{
		Data dt;
		dt.X = m_XMin; // 左上
		dt.Y = m_YMax;
		m_Border.push_back(dt);

		dt.X = m_XMax; // 右上
		dt.Y = m_YMax;
		m_Border.push_back(dt);

		dt.X = m_XMax; // 右下
		dt.Y = m_YMin;
		m_Border.push_back(dt);

		dt.X = m_XMin; // 左下
		dt.Y = m_YMin;
		m_Border.push_back(dt);
	}


	//m_Border.clear();//20131111
	//m_Border.resize(0);

	//for (int i = 0; i< m_Border.size(); i++)
	//{
	//	m_Border.push_back(m_Border[i]); // 导入边界
	//}

}
double Jie_Ds::GetDis(double x1, double y1, double x2, double y2)
{
	return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*((y1 - y2)));
}
void Jie_Ds::AddPt(vector<Data> &convexBag)
{
	int Num = (int)convexBag.size();
	double MaxD = 0;
	int Index = 0;
	for (int i = 0; i < Num - 1; i++)
	{
		Data Star = convexBag[i];
		Data End = convexBag[i + 1];
		double d = pow(Star.X - End.X, 2) + pow(Star.Y - End.Y, 2);
		if (d>MaxD)
		{
			MaxD = d;
			Index = i;
		}
	}
	Data Middle;
	Middle.X = 0.5*(convexBag[Index].X + convexBag[Index + 1].X);
	Middle.Y = 0.5*(convexBag[Index].Y + convexBag[Index + 1].Y);
	convexBag.insert(convexBag.begin() + Index + 1, Middle);
}
void Jie_Ds::deCasteljau()
{
	vector<point> bezierpoint;
	int n = control_point.size();

	for (double t = 0.0; t <= 1.0; t += 0.001 / n)//保留输入点
	{
		for (int i = 1; i < n; i++)
		{
			for (int j = 0; j < n - i; j++)
			{
				if (i == 1)//i=1由已知控制顶点计算
				{
					control_point[j].x = (1 - t) * input_vertice[j].x + t * input_vertice[j + 1].x;
					control_point[j].y = (1 - t) * input_vertice[j].y + t * input_vertice[j + 1].y;
					continue;
				}
				else//i!=1由上一次迭代结果计算
				{
					control_point[j].x = (1 - t) * control_point[j].x + t * control_point[j + 1].x;
					control_point[j].y = (1 - t) * control_point[j].y + t * control_point[j + 1].y;
				}
			}
		}
		b_spline.push_back(control_point[0]);
	}
}
void Jie_Ds::deBoor()
{
	float t[maxn];
	int k = 3;
	int n = control_point.size() - 1;
	//准均匀B样条
	for (int i = 0; i < n + k; i++)
	{
		if (i <= k - 1) t[i] = 0;
		if (i >= k && i < n + 1) t[i] = t[i - 1] + 1.0 / (n - k + 2);
		if (i >= n + 1) t[i] = 1;
	}
	for (int j = k - 1; j <= n; j++)
	{
		for (double u = t[j]; u <= t[j + 1]; u += 0.02 / n)
		{
			for (int r = 1; r <= k - 1; r++)
			{
				for (int i = j; i >= j - k + r + 1; i--)
				{
					float x1 = u - t[i];
					float x2 = t[i + k - r] - t[i];
					float y1 = t[i + k - r] - u;
					float k1, k2;
					if (x1 == 0.0 && x2 == 0.0) k1 = 0;
					else k1 = x1 / x2;
					if (y1 == 0.0 && x2 == 0.0) k2 = 0;
					else k2 = y1 / x2;

					if (r == 1)//控制点
					{
						control_point[i].x = input_vertice[i].x * k1 + input_vertice[i - 1].x * k2;
						control_point[i].y = input_vertice[i].y * k1 + input_vertice[i - 1].y * k2;
						continue;
					}
					else
					{
						control_point[i].x = control_point[i].x * k1 + control_point[i - 1].x * k2;
						control_point[i].y = control_point[i].y * k1 + control_point[i - 1].y * k2;
					}
				}
			}
			b_spline.push_back(control_point[j]);//递推的最后一层的点，即为求得的点
		}
	}


}
double Jie_Ds::bezier3funcX(double uu, Vector2D *controlP) {
	double part0 = controlP[0].x * uu * uu * uu;
	double part1 = 3 * controlP[1].x * uu * uu * (1 - uu);
	double part2 = 3 * controlP[2].x * uu * (1 - uu) * (1 - uu);
	double part3 = controlP[3].x * (1 - uu) * (1 - uu) * (1 - uu);
	return part0 + part1 + part2 + part3;
}
double Jie_Ds::bezier3funcY(double uu, Vector2D *controlP) {
	double part0 = controlP[0].y * uu * uu * uu;
	double part1 = 3 * controlP[1].y * uu * uu * (1 - uu);
	double part2 = 3 * controlP[2].y * uu * (1 - uu) * (1 - uu);
	double part3 = controlP[3].y * (1 - uu) * (1 - uu) * (1 - uu);
	return part0 + part1 + part2 + part3;
}
void Jie_Ds::createCurve()
{
	double scale = 0.3;
	int count = originPoint.size();
	//CvPoint midpoints[count];
	vector<Vector2D> midpoints;
	//生成中点       
	for (int i = 0; i < count; i++) {
		int nexti = (i + 1) % count;
		Vector2D p;
		p.x = (originPoint[i].x + originPoint[nexti].x) / 2.0;
		p.y = (originPoint[i].y + originPoint[nexti].y) / 2.0;
		midpoints.push_back(p);
	}

	//平移中点  
	Vector2D extrapoints[maxn];
	for (int i = 0; i < count; i++) {
		int nexti = (i + 1) % count;
		int backi = (i + count - 1) % count;
		Vector2D midinmid;
		midinmid.x = (midpoints[i].x + midpoints[backi].x) / 2.0;
		midinmid.y = (midpoints[i].y + midpoints[backi].y) / 2.0;
		double offsetx = originPoint[i].x - midinmid.x;
		double offsety = originPoint[i].y - midinmid.y;
		int extraindex = 2 * i;
		extrapoints[extraindex].x = midpoints[backi].x + offsetx;
		extrapoints[extraindex].y = midpoints[backi].y + offsety;
		//朝 originPoint[i]方向收缩   
		double addx = (extrapoints[extraindex].x - originPoint[i].x) * scale;
		double addy = (extrapoints[extraindex].y - originPoint[i].y) * scale;
		extrapoints[extraindex].x = originPoint[i].x + addx;
		extrapoints[extraindex].y = originPoint[i].y + addy;

		int extranexti = (extraindex + 1) % (2 * count);
		extrapoints[extranexti].x = midpoints[i].x + offsetx;
		extrapoints[extranexti].y = midpoints[i].y + offsety;
		//朝 originPoint[i]方向收缩   
		addx = (extrapoints[extranexti].x - originPoint[i].x) * scale;
		addy = (extrapoints[extranexti].y - originPoint[i].y) * scale;
		extrapoints[extranexti].x = originPoint[i].x + addx;
		extrapoints[extranexti].y = originPoint[i].y + addy;

	}

	Vector2D controlPoint[4];
	//生成4控制点，产生贝塞尔曲线  
	for (int i = 0; i < count; i++) {
		controlPoint[0] = originPoint[i];
		int extraindex = 2 * i;
		controlPoint[1] = extrapoints[extraindex + 1];
		int extranexti = (extraindex + 2) % (2 * count);
		controlPoint[2] = extrapoints[extranexti];
		int nexti = (i + 1) % count;
		controlPoint[3] = originPoint[nexti];
		double u = 1;
		while (u >= 0) {
			double px = bezier3funcX(u, controlPoint);
			double py = bezier3funcY(u, controlPoint);
			//u的步长决定曲线的疏密  
			u -= 0.01;
			Vector2D tempP = Vector2D(px, py);
			//存入曲线点   
			curvePoint.push_back(tempP);
		}
	}
}
void Jie_Ds::OptimizeBorderBezier(vector<Data> &convexBag,double e)
{
	std::ofstream outC("center_Bezier.obj");
	vector<Data> ConvexhullExp;
	double Sum_x = 0, Sum_y = 0, Aver_x = 0, Aver_y = 0;
	for (int j = 0; j < convexBag.size()-1; j++)
	{
		Sum_x += convexBag[j].X;
		Sum_y += convexBag[j].Y;
	}
	Aver_x = Sum_x / (convexBag.size() - 1);
	Aver_y = Sum_y / (convexBag.size() - 1);
	outC << "v " << fixed << setprecision(5) << Aver_x << " " << fixed << setprecision(5) << Aver_y << " " << "0" << endl;
	outC.close();
	for (int i = 0; i < convexBag.size(); i++)
	{
		Data cp;
		cp.X = convexBag[i].X;
		cp.Y = convexBag[i].Y;
		double d_v_x = convexBag[i].X - Aver_x;
		double d_v_y = convexBag[i].Y - Aver_y;
		double d_n_v_x = d_v_x / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		double d_n_v_y = d_v_y / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		cp.X += e* d_n_v_x;
		cp.Y += e* d_n_v_y;
		ConvexhullExp.push_back(cp);
	}

	for (int i = 0; i < ConvexhullExp.size()-1; i++)
	{
		Vector2D p;
		p.x = ConvexhullExp[i].X;
		p.y = ConvexhullExp[i].Y;
		originPoint.push_back(p);
	}
	createCurve();
	convexBag.clear();
	for (int i = 0; i < curvePoint.size(); i++)
	{
		Data d;
		d.X = curvePoint[i].x;
		d.Y = curvePoint[i].y;
		convexBag.push_back(d);
	}
	std::ofstream out101("data//fault_test//convex_exp.obj");
	for (int j = 0; j < ConvexhullExp.size(); j++)
	{
		Data qq;
		qq = ConvexhullExp[j];
		out101 << "v " << fixed << setprecision(5) << qq.X << " " << fixed << setprecision(5) << qq.Y << " " << "0" << endl;
	}
	int count_exp_ = 1;
	for (int j = 0; j < ConvexhullExp.size() - 1; j++)
	{
		out101 << "l " << count_exp_ << " " << count_exp_ + 1 << endl;
		count_exp_++;
	}
	out101.close();
	std::ofstream out2000("data//fault_test//convex_bezier.obj");
	int count_exp = 1;
	for (int j = 0; j < convexBag.size(); j++)
	{
		Data qq;
		qq = convexBag[j];
		out2000 << "v " << fixed << setprecision(5)<< qq.X << " " << fixed << setprecision(5)<<qq.Y << " " << "0" << endl;
	}
	count_exp_ = 1;
	for (int j = 0; j < convexBag.size() - 1; j++)
	{
		out2000 << "l " << count_exp << " " << count_exp + 1 << endl;
		count_exp++;
	}
	out2000 << "l " << convexBag.size() << " " << "1" << endl;
	out2000.close();


}
void Jie_Ds::OptimizeBoder(vector<Data> &convexBag,double e)
{
	vector<Data> ConvexhullExp;
	double Sum_x = 0, Sum_y = 0, Aver_x = 0, Aver_y = 0;
	for (int j = 0; j < convexBag.size()-1; j++)
	{
		Sum_x += convexBag[j].X;
		Sum_y += convexBag[j].Y;
	}
	Aver_x = Sum_x / (convexBag.size()-1);
	Aver_y = Sum_y / (convexBag.size()-1);
	for (int i = 0; i < convexBag.size(); i++)
	{
		Data cp;
		cp.X = convexBag[i].X;
		cp.Y = convexBag[i].Y;
		double d_v_x = convexBag[i].X - Aver_x;
		double d_v_y = convexBag[i].Y - Aver_y;
		double d_n_v_x = d_v_x / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		double d_n_v_y = d_v_y / sqrt((d_v_x*d_v_x) + (d_v_y*d_v_y));
		cp.X += e* d_n_v_x;
		cp.Y += e* d_n_v_y;
		ConvexhullExp.push_back(cp);
	}



	for (int i = 0; i < ConvexhullExp.size(); i++)
	{
		point p;
		p.x = ConvexhullExp[i].X;
		p.y = ConvexhullExp[i].Y;
		input_vertice.push_back(p);
	}
	control_point = input_vertice;
	deBoor();
	//deCasteljau();
	convexBag.clear();
	for (int i = 0; i < b_spline.size(); i++)
	{
		Data d;
		d.X = b_spline[i].x;
		d.Y = b_spline[i].y;
		convexBag.push_back(d);
	}
	std::ofstream out100("convex_current_exp.obj");
	for (int j = 0; j < ConvexhullExp.size(); j++)
	{
		Data qq;
		qq = ConvexhullExp[j];
		out100 << "v " << fixed << setprecision(5) << qq.X << " " << fixed << setprecision(5) << qq.Y << " " << "0" << endl;
	}
	int count_exp = 1;
	for (int j = 0; j < ConvexhullExp.size() - 1; j++)
	{
		out100 << "l " << count_exp << " " << count_exp + 1 << endl;
		count_exp++;
	}
	out100.close();



	std::ofstream out200("convex_bezier.obj");
	count_exp = 1;
	for (int j = 0; j < convexBag.size(); j++)
	{
		Data qq;
		qq = convexBag[j];
		out200 << "v " << qq.X << " " << qq.Y << " " << "0" << endl;
	}
	count_exp = 1;
	for (int j = 0; j < convexBag.size() - 1; j++)
	{
		out200 << "l " << count_exp << " " << count_exp + 1 << endl;
		count_exp++;
	}
	out200 << "l " << convexBag.size() << " " << "1" << endl;
	out200.close();

	//if ((int)convexBag.size() == 1)
	//{
	//	//制造四边形
	//	m_XMin = m_XMin - 200;
	//	m_XMax = m_XMax + 200;
	//	m_YMin = m_YMin - 200;
	//	m_YMax = m_YMax + 200;

	//	convexBag.clear();

	//	Data p1, p2, p3, p4;
	//	p1.X = m_XMin;
	//	p1.Y = m_YMin;

	//	p2.X = m_XMin;
	//	p2.Y = m_YMax;

	//	p3.X = m_XMax;
	//	p3.Y = m_YMax;

	//	p4.X = m_XMax;
	//	p4.Y = m_YMin;

	//	convexBag.push_back(p1);
	//	convexBag.push_back(p2);
	//	convexBag.push_back(p3);
	//	convexBag.push_back(p4);//左上角为（1,1）点，实际上的（0,0）点
	//	convexBag.push_back(p1);
	//}
	//else if ((int)convexBag.size() == 2)
	//{
	//	//制造对应的矩形
	//	convexBag.clear();

	//	Data p1, p2, p3, p4;
	//	if (m_XMin == m_XMax)
	//	{
	//		m_XMin = m_XMin - 200;
	//	}
	//	if (m_YMin == m_YMax)
	//	{
	//		m_YMin = m_YMin - 200;
	//	}
	//	p1.X = m_XMin;
	//	p1.Y = m_YMin;

	//	p2.X = m_XMin;
	//	p2.Y = m_YMax;

	//	p3.X = m_XMax;
	//	p3.Y = m_YMax;

	//	p4.X = m_XMax;
	//	p4.Y = m_YMin;

	//	convexBag.push_back(p1);
	//	convexBag.push_back(p2);
	//	convexBag.push_back(p3);
	//	convexBag.push_back(p4);		 //左上角为（1,1）点，实际上的（0,0）点
	//	convexBag.push_back(p1);        //这里为啥左上角有区别

	//}
	//else if ((int)convexBag.size() == 4)
	//{
	//	//添加一个点
	//	AddPt(convexBag);
	//}

	//double dx = m_XMax - m_XMin;
	//double dy = m_YMax - m_YMin;
	//if (dy - 8 * dx > 0)
	//{
	//	//修改图形为矩形？
	//}
	//else if (dx - 8 * dy > 0)
	//{
	//	//修改图形为矩形？
	//}
}
double Jie_Ds::Angle(Data &p0, const Data &p1, const Data &p2)
{
	//将第二个点沿着x轴方向向右平移一个单位赋值给第一个点
	p0.X = p1.X + 1;
	p0.Y = p1.Y;


	/*std::ofstream out("current_triangle_point.obj");
	out << "v " << p0.X << " " << p0.Y << " " << "0" << endl;
	out << "v " << p1.X << " " << p1.Y << " " << "0" << endl;
	out << "v " << p2.X << " " << p2.Y << " " << "0" << endl;

	out.close();*/

	double A, B, X, cross, angle;
	//计算 距离
	A = sqrt((p1.X - p0.X) * (p1.X - p0.X) + (p1.Y - p0.Y) *(p1.Y - p0.Y));
	B = sqrt((p1.X - p2.X) * (p1.X - p2.X) + (p1.Y - p2.Y) *(p1.Y - p2.Y));

	if (A == 0 || B == 0)
		return 0;

	//余弦值
	X = ((p0.X - p1.X) * (p2.X - p1.X) + (p0.Y - p1.Y) * (p2.Y - p1.Y)) / (A*B);

	//叉乘判断逆时针是否大于180度
	//a = (x1, y1) b = (x2, y2)
	//a×b = x1y2 - x2y1,若结果为正，则向量b在a的逆时针方向,否则，b在a的顺时针方向,若结果为0，则a与b共线
	cross = (p0.X - p1.X) * (p2.Y - p1.Y) - (p2.X - p1.X) * (p0.Y - p1.Y);

	if (X == 1 || X >1 || X < -1)
		return 0;

	double temp;
	double p = 3.14159265358979323846;
	if (X == -1)
	{
		angle = p;
	}
	else
	{
		if (cross < 0)//顺时针
		{
			//cout << "顺时针" << endl;
			//angle = atan2((p2.Y - p1.Y), (p2.X - p1.X));
			temp = (atan(-X / sqrt(-X * X + 1)) + 2 * atan(1.0));
			angle = p*2  - temp;
		}
		else//逆时针
		{
			//cout << "逆时针" << endl;
			//angle = atan2((p1.Y - p2.Y), (p2.X - p1.X));
			angle = (atan(-X / sqrt(-X * X + 1)) + 2 * atan(1.0));
		}
	}
	//if (angle > p)
	//	angle = angle - p;
	return angle;
}
void Jie_Ds::DividedFourParts(int n, vector<int>& FourPort)
{
	if (n>3)
	{
		FourPort[0] = n;
		FourPort[1] = (int)(n / 4);
		FourPort[2] = 2 * (int)(n / 4);
		FourPort[3] = n - (int)(n / 4);
	}
	else
	{
		FourPort[0] = 3;
		FourPort[1] = 1;
		FourPort[2] = 2;
		FourPort[3] = 2;
	}

}
void Jie_Ds::BordersPointDis(vector<int>& FourPart, vector<Data>& convexBag, vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
	vector<Dis>& upDis, vector<Dis>&downDis, vector<Dis>&leftDis, vector<Dis>&rightDis)
{
	//这个容量分配有什么依据
	downline.resize(FourPart[0] - FourPart[3] + 2);//
	leftLine.resize(FourPart[1] + 2);//
	upLine.resize(FourPart[2] - FourPart[1] + 2);//
	rightLine.resize(FourPart[3] - FourPart[2] + 2);//

	downDis.resize(downline.size());
	leftDis.resize(leftLine.size());
	upDis.resize(upLine.size());
	rightDis.resize(rightLine.size());

	bool JudgeMax = true;
	vector<int> side(4);
	//赋初值
	side[0] = FourPart[3];
	side[1] = 0;
	side[2] = FourPart[1];
	side[3] = FourPart[2];

	int count_s0 = 0, count_s1 = 0, count_s2 = 0, count_s3 = 0;
	int j = 1, i;
	while (JudgeMax)
	{
		i = 0;

		//给下边计数器小于最大值
		if (side[0] <= FourPart[0])
		{
			count_s0++;
			//给点赋值
			downline[j].X = convexBag[side[0]].X;
			downline[j].Y = convexBag[side[0]].Y;


			//当为首点时计算的距离 默认为到(0,0)的距离 则
			//为首点时 给距离赋值 0
			if (j == 1)
				downDis[j].dis = 0;
			else
			//计算此点距此边首点的距离
			downDis[j].dis = sqrt((downline[j].X - downline[j - 1].X) * (downline[j].X - downline[j - 1].X)
				+ (downline[j].Y - downline[j - 1].Y) * (downline[j].Y - downline[j - 1].Y)) + downDis[j - 1].dis;

			

			//边序号自增
			side[0] = side[0] + 1;
			//记录判断的数自增
			i = i + 1;
			/*if (side[0] == FourPart[0])
			{
				int j_ = j + 1;
				downline[j_].X = convexBag[0].X;
				downline[j_].Y = convexBag[0].Y;
				downDis[j_].dis = sqrt((downline[j_].X - downline[j_ - 1].X) * (downline[j_].X - downline[j_ - 1].X)
					+ (downline[j_].Y - downline[j_ - 1].Y) * (downline[j_].Y - downline[j_ - 1].Y)) + downDis[j_ - 1].dis;
			}*/

		}

		//左边赋值  同上
		if (side[1] <= FourPart[1])
		{
			count_s1++;
			leftLine[j].X = convexBag[side[1]].X;
			leftLine[j].Y = convexBag[side[1]].Y;

			if (j == 1)
				leftDis[j].dis = 0;
			else
			leftDis[j].dis = sqrt((leftLine[j].X - leftLine[j - 1].X) * (leftLine[j].X - leftLine[j - 1].X)
				+ (leftLine[j].Y - leftLine[j - 1].Y) * (leftLine[j].Y - leftLine[j - 1].Y)) + leftDis[j - 1].dis;

			

			side[1] = side[1] + 1;
			i = i + 1;
		}
		//上边赋值 同上
		if (side[2] <= FourPart[2])
		{
			count_s2++;
			upLine[j].X = convexBag[side[2]].X;
			upLine[j].Y = convexBag[side[2]].Y;
			if (j == 1)
				upDis[j].dis = 0;
			else
			upDis[j].dis = sqrt((upLine[j].X - upLine[j - 1].X) * (upLine[j].X - upLine[j - 1].X)
				+ (upLine[j].Y - upLine[j - 1].Y) * (upLine[j].Y - upLine[j - 1].Y)) + upDis[j - 1].dis;

			

			side[2] = side[2] + 1;
			i = i + 1;
		}

		//右边赋值 同上
		if (side[3] <= FourPart[3])
		{
			count_s3++;
			rightLine[j].X = convexBag[side[3]].X;
			rightLine[j].Y = convexBag[side[3]].Y;

			if (j == 1)
				rightDis[j].dis = 0;
			else
			rightDis[j].dis = sqrt((rightLine[j].X - rightLine[j - 1].X) * (rightLine[j].X - rightLine[j - 1].X)
				+ (rightLine[j].Y - rightLine[j - 1].Y) * (rightLine[j].Y - rightLine[j - 1].Y)) + rightDis[j - 1].dis;

			

			side[3] = side[3] + 1;
			i = i + 1;
		}

		//边的记录器 自增
		j = j + 1;
		//如果没有运算 i=0 则说明已经遍历完了 所有点上的点 跳出循环
		if (i == 0)
			JudgeMax = false;
	}
	std::ofstream outd("DP.obj");
	for (int i = 1; i < downline.size(); i++)
		outd << "v " << downline[i].X << " " << downline[i].Y << " " << "0" << endl;
	outd.close();
	std::ofstream outu("UP.obj");
	for (int i = 1; i < upLine.size(); i++)
		outu << "v " << upLine[i].X << " " << upLine[i].Y << " " << "0" << endl;
	outu.close();
	std::ofstream outl("LP.obj");
	for (int i = 1; i < leftLine.size(); i++)
		outl << "v " << leftLine[i].X << " " << leftLine[i].Y << " " << "0" << endl;
	outl.close();
	std::ofstream outr("RP.obj");
	for (int i = 1; i < rightLine.size(); i++)
		outr << "v " << rightLine[i].X << " " << rightLine[i].Y << " " << "0" << endl;
	outr.close();
	//cout << "j = :" << j << endl;
	//cout << "count_s0 = :" << count_s0 << endl;
	//cout << "count_s1 = :" << count_s1 << endl;
	//cout << "count_s2 = :" << count_s2 << endl;
	//cout << "count_s3 = :" << count_s3 << endl;
}
//计算upLine、downline、leftLine、rightLine中Dis分量占总的长度的半分比
//计算upDis、downdis、leftDis、rightDis中angle分量，和前一个点构成的线段与x轴正方向的夹角
void Jie_Ds::BordersChar(vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
	vector<Dis>& upDis, vector<Dis>&downdis, vector<Dis>&leftDis, vector<Dis>&rightDis)
{
	bool JudgeMax = true;
	Data triPoint[3];
	int i, j = 2;
	std::ofstream out22("2.obj");
	while (JudgeMax)
	{
		//一个判断次循环是否有计算的 计数器 i>0说明次循环有运算，否则i=0 跳出循环
		//赋初值
		i = 0;
		//个数没有超出 下边 则进行计算
		if (j < (int)downline.size())
		{
			//计算此点占此边总长的百分数
			//这里downline.size()比downdis.size()大1，downsize下标从1开始，downsize[1]=0;
			downdis[j].per = downdis[j].dis / downdis[downline.size() - 1].dis;
			//赋值记录此点与前一点的线段
			triPoint[1] = downline[j - 1];
			triPoint[2] = downline[j];

			//计算此线段与x轴正方向的角度,这里的角度其实是从x轴正方向开始逆时针旋转，直到旋转到向量时所旋转的角度
			downdis[j].angle = Angle(triPoint[0], triPoint[1], triPoint[2]);
			//cout << downdis[j].angle << endl;
			//自增 >0说明有运算
			i = i + 1;
			
			out22 << "v " << downline[downline.size() - 1].X << " " << downline[downline.size() - 1].Y << " " << "0" << endl;
		}
		
		if (j < (int)leftLine.size())
		{
			//计算此点占此边总长的百分数
			leftDis[j].per = leftDis[j].dis / leftDis[leftLine.size() - 1].dis;
			//赋值记录此点与前一点的线段
			triPoint[1] = leftLine[j - 1];
			triPoint[2] = leftLine[j];

			//计算此线段与x轴正方向的角度
			leftDis[j].angle = Angle(triPoint[0], triPoint[1], triPoint[2]);
			//自增 >0说明有运算
			i = i + 1;
			
		}

		if (j < (int)upLine.size())
		{
			//计算此点占此边总长的百分数
			upDis[j].per = upDis[j].dis / upDis[upLine.size() - 1].dis;
			//赋值记录此点与前一点的线段
			triPoint[1] = upLine[j - 1];
			triPoint[2] = upLine[j];

			//计算此线段与x轴正方向的角度
			upDis[j].angle = Angle(triPoint[0], triPoint[1], triPoint[2]);
			//自增 >0说明有运算
			i = i + 1;
			
		}

		if (j < (int)rightLine.size())
		{
			//计算此点占此边总长的百分数
			rightDis[j].per = rightDis[j].dis / rightDis[rightLine.size() - 1].dis;
			//赋值记录此点与前一点的线段
			triPoint[1] = rightLine[j - 1];
			triPoint[2] = rightLine[j];

			//计算此线段与x轴正方向的角度
			rightDis[j].angle = Angle(triPoint[0], triPoint[1], triPoint[2]);
			//自增 >0说明有运算
			i = i + 1;
			
		}

		//边界计数器自增
		j = j + 1;
		//i=0则说明此次循环没有运算 则跳出循环
		if (i == 0)
			JudgeMax = false;
		out22.close();

	}
}
void Jie_Ds::BordersPoints(vector<int>& FourPart, vector<Data>& convexBag, vector<Data>& upLine, vector<Data>&downline, vector<Data>&leftLine, vector<Data>& rightLine,
	vector<Dis>& upDis, vector<Dis>&downdis, vector<Dis>&leftDis, vector<Dis>&rightDis,
	vector<Data>& DL, vector<Data>& DR, vector<Data>&DD, vector<Data>& DU,
	vector<TWOVALUE>& sim1, vector<TWOVALUE>& sim2, vector<TWOVALUE>& sim3, vector<TWOVALUE>& sim4, int XM, int YM)
{
	int i, j, m;
	double s, t, Tran;

	//*****************分割算法
	//确定四点坐标
	//存储映射的原点坐标
	sim1[1].X = convexBag[FourPart[0]].X;//这里应该存储的是凸包边界上最后一个点的位置

	//对应第一条边的x坐标差值
	sim1[2].X = convexBag[FourPart[3]].X - convexBag[FourPart[0]].X;

	//对应第二条边的x坐标差值
	sim1[3].X = convexBag[FourPart[1]].X - convexBag[FourPart[0]].X;

	sim1[4].X = convexBag[FourPart[0]].X - convexBag[FourPart[1]].X + convexBag[FourPart[2]].X - convexBag[FourPart[3]].X;

	//解释同上 对应y坐标值
	sim1[1].Y = convexBag[FourPart[0]].Y;
	sim1[2].Y = convexBag[FourPart[3]].Y - convexBag[FourPart[0]].Y;
	sim1[3].Y = convexBag[FourPart[1]].Y - convexBag[FourPart[0]].Y;
	sim1[4].Y = convexBag[FourPart[0]].Y - convexBag[FourPart[1]].Y + convexBag[FourPart[2]].Y - convexBag[FourPart[3]].Y;

	/*cout << "=======================" << endl;
	cout << "=======================" << endl;
	for(int p=1;p<=4;p++)
	cout << "v " << sim1[p].X << " " << sim1[p].Y << " " << "0" << endl;*/



	sim3.resize(YM + 1);
	sim2.resize(XM + 1);
	sim4.resize(XM + 1);
	DL.resize(YM + 1);
	DR.resize(YM + 1);
	DD.resize(XM + 1);
	DU.resize(XM + 1);
	//注意，其实这里的(XM-1),(YM-1)才是两个方向上网格的实际个数,XM,YM是两个方向上网格点的个数
	for (j = 1; j <= YM; j++)
	{
		//此分割点距边起点的距离占此边长度的百分数
		//t = double(j) / YM;
		//网格个数的比值
		t = double(j - 1) / (YM - 1);

							  //计算
							 //则此点到此边起点的距离
		sim3[j].X = sim1[3].X * t;
		sim3[j].Y = sim1[3].Y * t;

		//遍历此点的特征数组(存储点到边的距离,及此距离为此边的百分数, 和此点与前一点的线段和x轴正方向的角度(注意这个角度的定义))
		for (m = 2; m<(int)leftDis.size(); m++)
		{
			//当分割点 百分数与 凸包数组的百分数相同时
			if (t == leftDis[m - 1].per) //当凸包点为分割点时
			{
				//把此凸包点赋值给左边分割点数组
				DL[j].X = leftLine[m - 1].X;
				DL[j].Y = leftLine[m - 1].Y;
				//查找到此分割点遍历结束
				break;
			}

			//当分割点遍历到此边的尾点时
			else if (t == 1)
			{
				//把此边的最后一个点赋值给 分割点数组
				DL[j].X = convexBag[FourPart[1]].X;
				DL[j].Y = convexBag[FourPart[1]].Y;
				//查找到了跳出循环
				break;
			}

			//当此分割点距首点的距离占的百分数 为在条凸包边上时 则  t在某个线段两端点的百分数之间
			else if (leftDis[m - 1].per < t && t < leftDis[m].per)
			{
				//此分割点所在凸包边上的位置到此凸包边一端点距离占此凸包边长度的百分数
				Tran = leftDis[leftDis.size() - 1].dis* t - leftDis[m - 1].dis;
				//根据此边的角度与此点到一段点的距离 由直线参数方程 可计算此点的(x,y)坐标
				DL[j].X = leftLine[m - 1].X + Tran * cos(leftDis[m].angle);
				DL[j].Y = leftLine[m - 1].Y + Tran * sin(leftDis[m].angle);
				break;
			}
		}

		if ((int)rightDis.size()>1)
		{
			//同上，遍历右边界的分割点 并给它赋值
			for (m = 2; m< (int)rightDis.size(); m++)
			{
				if ((1 - t) == rightDis[m - 1].per)  //当为某个节点时
				{
					DR[j].X = rightLine[m - 1].X;
					DR[j].Y = rightLine[m - 1].Y;
					break;
				}
				else if ((1 - t) == 1)
				{
					DR[j].X = convexBag[FourPart[3]].X;
					DR[j].Y = convexBag[FourPart[3]].Y;
					break;
				}
				else if (rightDis[m - 1].per < (1 - t) && (1 - t) < rightDis[m].per)
				{
					Tran = rightDis[rightDis.size() - 1].dis * (1 - t) - rightDis[m - 1].dis;
					DR[j].X = rightLine[m - 1].X + Tran * cos(rightDis[m].angle);
					DR[j].Y = rightLine[m - 1].Y + Tran * sin(rightDis[m].angle);
					break;
				}
			}
		}
		//当特征数组=3时, 只有一个点
		else
		{
			//此点为第4个点
			DR[j].X = convexBag[FourPart[3]].X;
			DR[j].Y = convexBag[FourPart[3]].Y;
		}
	}

	// 左右边界上分割点遍历完毕

	//遍历上下边界的分割点
	for (i = 1; i <= XM; i++)
	{
		//计算此点占总边长的百分数
		s = double(i - 1) / (XM - 1);
		sim2[i].X = sim1[2].X * s;
		sim2[i].Y = sim1[2].Y * s;
		sim4[i].X = sim1[4].X * s;
		sim4[i].Y = sim1[4].Y * s;

		//给上边界分割点赋值
		for (m = 2; m< (int)upDis.size(); m++)
		{
			//当此分割点的百分数与凸包点的百分数相同时 凸包点为分割点
			if (s == upDis[m - 1].per) //当为某个节点时
			{
				//给分割点赋值
				DU[i].X = upLine[m - 1].X;
				DU[i].Y = upLine[m - 1].Y;
				//成立时跳出循环
				break;
			}

			//为尾点时
			else if (s == 1)
			{
				DU[i].X = convexBag[FourPart[2]].X;
				DU[i].Y = convexBag[FourPart[2]].Y;
				//跳出循环
				break;

			}

			//当为某个凸包边时
			else if (upDis[m - 1].per < s && s < upDis[m].per)
			{
				Tran = upDis[upDis.size() - 1].dis * s - upDis[m - 1].dis;
				DU[i].X = upLine[m - 1].X + Tran * cos(upDis[m].angle);
				DU[i].Y = upLine[m - 1].Y + Tran * sin(upDis[m].angle);
				break;
			}
		}

		//与上同理 可计算下边界分割点的坐标值
		for (m = 2; m < (int)downdis.size(); m++)
		{
			if ((1 - s) == downdis[m - 1].per)  //当为某个节点时
			{
				DD[i].X = downline[m - 1].X;
				DD[i].Y = downline[m - 1].Y;
				break;
			}

			else if ((1 - s) == 1)
			{
				DD[i].X = convexBag[FourPart[0]].X;
				DD[i].Y = convexBag[FourPart[0]].Y;
				break;
			}
			else if (downdis[m - 1].per <= 1 - s && 1 - s <= downdis[m].per)
			{
				Tran = downdis[downdis.size() - 1].dis * (1 - s) - downdis[m - 1].dis;
				DD[i].X = downline[m - 1].X + Tran * cos(downdis[m].angle);
				DD[i].Y = downline[m - 1].Y + Tran * sin(downdis[m].angle);
				break;
			}

		}

	}
	std::ofstream outDL("FGL.obj");
	for (int i = 1; i < DL.size(); i++)
		outDL << "v " << DL[i].X << " " << DL[i].Y << " " << "0" << endl;
	outDL.close();
	std::ofstream outDR("FGR.obj");
	for (int i = 1; i < DR.size(); i++)
		outDR << "v " << DR[i].X << " " << DR[i].Y << " " << "0" << endl;
	outDR.close();
	std::ofstream outDU("FGU.obj");
	for (int i = 1; i < DU.size(); i++)
		outDU << "v " << DU[i].X << " " << DU[i].Y << " " << "0" << endl;
	outDU.close();
	std::ofstream outDD("FGD.obj");
	for (int i = 1; i < DD.size(); i++)
		outDD << "v " << DD[i].X << " " << DD[i].Y << " " << "0" << endl;
	outDD.close();


}
void Jie_Ds::AddData(Data &t)
{
	bool m_b = false;
	for (int i = 0; i<(int)m_oriData.size(); i++)
	{
		if (m_oriData[i].X == t.X && m_oriData[i].Y == t.Y)
			m_b = true;
	}
	if (!m_b)
		m_oriData.push_back(t);
}
bool Jie_Ds::Inv(vector<vector<double>>&M)
{
	int i, j, k, n;
	double Temp;
	n = (int)M.size();
	vector<int> iw(n), jw(n);
	//在m右边增加一个单位阵，构成一个m的增广矩阵mm
	//double **mm = new double * [n];
	//vector<vector<double>> mm(n);
	//for(i = 0 ;i< n;i++)
	//{
	//	mm[i].resize(2 * n);
	//	for(int j = 0;j < n;j ++) 
	//	{
	//		mm[i][j] = M[i][j];
	//	}
	//}
	double **mm = new double *[n];		//数据比较大或操作比较多，vector效率比指针低很多20150325
	for (i = 0; i< n; i++)
	{
		mm[i] = new double[2 * n];
		for (int j = 0; j < n; j++)
		{
			mm[i][j] = M[i][j];
		}
	}

	for (i = 0; i< n; i++)
	{
		for (j = n; j<2 * n; j++)
		{
			if (i == j - n)
				mm[i][j] = 1;
			else
				mm[i][j] = 0;
		}
	}
	//通过初等行变换(即高斯消去法)使原矩阵变为单位阵，则右边的单位阵即是原矩阵的逆阵
	for (k = 0; k < n - 1; k++)
	{
		/*----------------------------------------*/
		//从第 k 行、第 k 列开始的右下角子阵中选取绝对值最大的元素，并记住次元素在的行号和列号，
		//在通过行交换和列交换将它交换到主元素位置上.这一步称为全选主元  20140925
		iw[k] = k;
		jw[k] = k;
		if (abs(mm[k][k]) < 0.00000001)
		{
			for (int i = k + 1; i < n; i++)
			{
				if (abs(mm[i][k]) > 0.000001)
				{
					iw[k] = i;
					for (j = 0; j < n; j++)
					{
						Temp = mm[k][j];
						mm[k][j] = mm[iw[k]][j];
						mm[iw[k]][j] = Temp;
						//swap(mm[k][j], mm[iw[k]][j]);
					}
					break;
				}
			}
		}
		if (abs(mm[k][k]) < 0.00000001)
		{
			return false;
		}
		/*----------------------------------------*/
		for (i = k + 1; i < n; i++)
		{
			Temp = mm[i][k] / mm[k][k];
			for (j = 0; j < 2 * n; j++)
			{
				mm[i][j] = mm[i][j] - mm[k][j] * Temp;
			}
			mm[i][k] = 0.0;		//防止计算误差20140929
		}
	}

	if (abs(mm[n - 1][n - 1]) < 0.00000001)
	{
		return false;
	}
	for (k = n - 1; k > 0; k--)
	{
		for (i = k - 1; i >= 0; i--)
		{
			Temp = mm[i][k] / mm[k][k];
			for (j = 2 * n - 1; j >= 0; j--)
				mm[i][j] = mm[i][j] - mm[k][j] * Temp;
		}
	}
	double s;
	for (i = 0; i<n; i++)
	{
		s = mm[i][i];
		for (j = 0; j < 2 * n; j++)
			mm[i][j] = mm[i][j] / s;
	}

	//输出变换后的右边的矩阵
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < n; j++)
			M[i][j] = mm[i][j + n];
	}

	//根据在全选主元过程中所记录的行、列交换的信息进行恢复，恢复的原则如下：在全选主元过程中，
	//先交换的行（列）后进行恢复；原来的行（列）交换用列（行）交换来恢复。
	for (k = n - 2; k >= 0; k--)
	{
		if (jw[k] != k)
		{
			for (i = 0; i < n; i++)
			{
				Temp = M[k][i];
				M[k][i] = M[jw[k]][i];
				M[jw[k]][i] = Temp;
				//swap(MInv[k][i], MInv[jw[k]][i]);
			}
		}

		if (iw[k] != k)
		{
			for (i = 0; i < n; i++)
			{
				Temp = M[i][k];
				M[i][k] = M[i][iw[k]];
				M[i][iw[k]] = Temp;
				//swap(MInv[i][k], MInv[i][iw[k]]);
			}
		}
	}
	return true;
}
void Jie_Ds::PreMatrix(vector<Data>& suV, vector<double>& ni)
{
	double b0 = 1;			//先假设斜率为一个值
	double b1 = 0;			//设截距为一个值
	double RR = m_B;		//设最大滞后距
	//cout << m_B << endl;
	int n = (int)suV.size();//井位点个数
	//vector<vector<double>> Arr(n + 1);
	MatrixXd m(n + 1, n + 1);
	MatrixXd m_inver(n + 1, n + 1);
	MatrixXd m_tran(n + 1, n + 1);
	MatrixXd m_temp(n + 1, n + 1);
	MatrixXd m_temp_inver(n + 1, n + 1);

	//********给矩阵赋值
	//Arr[0].resize(n + 1);
	for (int i = 1; i <= n; i++)
	{
		//Arr[i].resize(n + 1);
		//Arr[i][i] = b1 + b0 * RR - b1;//对角线数值为最大滞后距离
		m(i,i) = b1 + b0 * RR - b1;
		//Arr[i][n] = 1;
		m(i, n) = 1;
		//Arr[0][i] = 1;
		m(0, i) = 1;
		for (int j = 0; j <= n - 1; j++)
		{
			//计算当前点和剩余所有点的距离
			double d = sqrt((suV[i - 1].X - suV[j].X) * (suV[i - 1].X - suV[j].X) + (suV[i - 1].Y - suV[j].Y) * (suV[i - 1].Y - suV[j].Y));
			//这里RR其实已经设置的值足够大了，所以下面这个if语句不会进去
			if (d > RR)
			{
				d = RR;			//20140422最大滞后距离,这里的滞后距离指的应该是井位点两两距离的最大值
			}
			//Arr[i][j] = abs(b0 * (d)-RR);//当前距离和这个最大滞后距离差值的绝对值
			m(i,j)= abs(b0 * (d)-RR);
		}
	}
	//Arr[0][n] = 0;
	//Arr[0][0] = 1;
	m(0, n) = 0;
	m(0, 0) = 1;
	m_temp = m;
	double temp;
	for (int i = 0; i <= n; i++)
	{
		temp = m_temp(0,i);
		m_temp(0, i) = m_temp(n, i);
		m_temp(n, i) = temp;
	}
	//cout << m_temp;
//	cout << endl;
	//cout << "原矩阵：" << endl;
	//cout << m << endl;
	//cout << endl;
	//Inv(Arr);//求出矩阵的逆矩阵
	m_tran = m.transpose();
	m_inver = m.inverse();
	m_temp_inver = m_temp.inverse();
	//cout << "转置矩阵：" << endl;
	//cout << m_tran << endl;
	//cout << m_tran.inverse() << endl;
	//cout << endl;
	//cout << "逆矩阵：" << endl;
	//cout << m_inver << endl;
	//cout << endl;
	//求解方程根
	ni.resize(n + 1);
	Data d;
	d.X = 0; d.Y = 0; d.Z = 0;
	suV.push_back(d);
	//cout<<suV[n].Z << endl;
	for (int i = 0; i <= n; i++)
	{
		for (int j = 0; j <= n; j++)
		{
			//ni[i] = ni[i] + suV[j].Z * Arr[j][i];
			//之前先对m进行一个转置操作之后就清晰一些了
			//任何一个高程值都是ni中元素的线性组合，其中ni[0]前面的系数为1
			//剩余ni[i]前面的系数与欧几里得距离和最大滞后距离的比值有关
			ni[i] = ni[i] + suV[j].Z * m_inver(j, i);
		}
	}
	//cout << endl;
	//for (int i = 0; i <= n; i++)
	//	cout << ni[i] << " ";
	//cout << endl;
	//cout << ni[0] <<" "<<ni[1]<<" "<<ni[2]<<" "<<ni[3]<< endl;
}

double Jie_Ds::InsertEst(vector<Data>& suV, TWOVALUE& D, vector<double>& ni)
{
	double dg = 0;
	double ZZ = m_B;//直线型变异函数B值
	double b0 = 1;
	double b1 = 0;
	vector<double> cc(suV.size());
	cc[0] = 1;
	//cout << ni[0] << endl;
	dg = cc[0] * ni[0];
	double distanceVal = 0;
	//一定要注意这里的suV在上一个函数中多加了一个元素！！！
	for (int i = 1; i < (int)suV.size(); i++)
	{
		// distanceVal = 0;
		//当前网格点和井位点的距离
		distanceVal = sqrt((suV[i - 1].X - D.X) * (suV[i - 1].X - D.X) + (suV[i - 1].Y - D.Y) * (suV[i - 1].Y - D.Y));
		if (distanceVal > ZZ)
		{
			distanceVal = ZZ;	//20140422最大滞后距离
		}
		cc[i] = b0 * (ZZ - distanceVal);
		dg = cc[i] * ni[i] + dg;
	}
	//cout << dg << " " << endl;
	return dg;
}
double Jie_Ds::DisInv(TWOVALUE D)
{
	double Z = 0;
	double Sum = 0;
	double Di = 0;
	Data TempWell;
	for (int i = 0; i < (int)m_suV.size(); i++)
	{
		TempWell = m_suV[i];
		double d = (pow(D.X - TempWell.X, 2) + pow(D.Y - TempWell.Y, 2));
		//默认是是2次距离反比
		//d = pow(d,1.5);		//加大距离反比法的次数，以便离得近的点影响力更大20131217
		//如果距离太近，直接把此点的高程值赋值给它
		if (d < 0.00001)
		{
			return TempWell.Z;
		}
		/************************/
		//TWOVALUE temp;
		//temp.X = TempWell.X;
		//temp.Y = TempWell.Y;
		//if (IsChangeW(D,temp,m_vecFaultLines))
		//{
		//	d = d * 1000;								//隔着断层则缩小权值20140904；
		//}
		/**************************/
		//d = pow(d,3) ;
		d = 1.0 / d;
		Sum += d;
		Di = Di + d * TempWell.Z;
	}
	Z = Di / Sum;
	return Z;
}
void Jie_Ds::EvaluateNoFault()
{
	if ((int)m_oriData.size() < 800)//20150323,800口井可以使用克里金方法，速度4s左右可以接受
	{
		m_IsK = true;				//点数较少使用克里金估值
	}
	else
	{
		m_IsK = false;				//点数太多使用幂距离反比估值
	}

	if ((int)m_oriData.size() <= 1)
	{
		return;		//处理只有一个井位点的情况20131107
	}
	if (m_IsK)			//是否使用克里金算法估值（准确但是速度慢）201308163
	{
		vector<Data> Suv = m_oriData;
		vector<double> ni;
		PreMatrix(Suv, ni);
		TWOVALUE D;
		for (int i = 1; i <= m_XNum; i++)
		{
			for (int j = 1; j <= m_YNum; j++)
			{
				D.X = m_GridPoint[i][j].X;
				D.Y = m_GridPoint[i][j].Y;
				//估计此点的属性值    没有断层的估计值 + 修正值
				m_GridPoint[i][j].Z = InsertEst(Suv, D, ni);//全部井位点的克里金估值

															//m_GridPoint[i][j].Z = Well_Near_K(D);		//邻井范围内的克里金估值20131212
															//m_GridPoint[i][j].Z = DisInv(D);			//邻井范围内的距离反比估值20131212
				if (m_ZMin > m_GridPoint[i][j].Z)
					m_ZMin = m_GridPoint[i][j].Z;

				if (m_ZMax < m_GridPoint[i][j].Z)
					m_ZMax = m_GridPoint[i][j].Z;
			}
		}
		//m_suV = Suv;		//20131212
		//m_ni = ni;			//20130913 应用于外推法
	}
	else
	{
		m_suV = m_oriData;
		TWOVALUE D;
		for (int i = 1; i <= m_XNum; i++)
		{
			for (int j = 1; j <= m_YNum; j++)
			{
				D.X = m_GridPoint[i][j].X;
				D.Y = m_GridPoint[i][j].Y;
				//估计此点的属性值    没有断层的估计值 + 修正值
				//m_GridPoint[i][j].Z = InsertEst(Suv, D, ni);//全部井位点的克里金估值
				//m_GridPoint[i][j].Z = Well_Near_K(D);		//邻井范围内的克里金估值20131212
				m_GridPoint[i][j].Z = DisInv(D);			//邻井范围内的距离反比估值20131212
				if (m_ZMin > m_GridPoint[i][j].Z)
					m_ZMin = m_GridPoint[i][j].Z;

				if (m_ZMax < m_GridPoint[i][j].Z)
					m_ZMax = m_GridPoint[i][j].Z;
			}

		}
	}

	//if ((int)m_oriData.size() <= 1)
	//{
	//	return ;		//处理只有一个井位点的情况20131107
	//}
	//vector<double> ni;
	//vector<Data> Suv = m_oriData; //20140421
	//PreMatrix(Suv,ni);
	//TWOVALUE D;
	//for (int i = 1;i <= m_XNum;i++)
	//{
	//	for(int j = 1;j <= m_YNum;j++)
	//	{
	//		D.X= m_GridPoint[i][j].X;
	//		D.Y = m_GridPoint[i][j].Y;
	//		//估计此点的属性值    没有断层的估计值 + 修正值
	//		m_GridPoint[i][j].Z = InsertEst(Suv, D, ni);
	//		if(m_ZMin > m_GridPoint[i][j].Z)
	//			m_ZMin = m_GridPoint[i][j].Z;

	//		if(m_ZMax < m_GridPoint[i][j].Z)
	//			m_ZMax = m_GridPoint[i][j].Z;
	//	}

	//}

	/************************************************/
	//CString Str = "z值 \n";
	//for (int i = 1;i <= m_XNum;i++)
	//{
	//	for(int j = 1;j <= m_YNum;j++)
	//	{
	//		CString s;
	//		s.Format("    %f",m_GridPoint[i][j].Z);
	//		Str += s;
	//	}
	//	Str += "\n";
	//}
	//for (int i = 0 ; i < m_oriData.size() ; i ++)
	//{
	//	CString str;
	//	str.Format("%f,    %f,    %f，   %f     ，%f\n",m_oriData[i].X,m_oriData[i].Y,m_oriData[i].Z,m_ZMax,m_ZMin);
	//	Str += str;
	//}
	//AfxMessageBox(Str);
	/************************************************/
	//m_suV = Suv;
	//m_ni = ni;				//20130913 应用于外推法
}
void Jie_Ds::SetGridXY(double exp)
{
	//这里的m_Border中应该存储的是矩形边界
	m_OriBoder = m_Border;	//原始边界信息由m_OriBoder  20140806
	vector<Data> convexBag;
	//Charact();				//求取最大最小值

							/*-------------------------------------------*/
							//兼容二级边界不闭合的情况20170718
	int OriCount = (int)m_OriBoder.size();
	if (OriCount >= 3)
	{
		Data Pt0 = m_OriBoder[0];
		Data Pt1 = m_OriBoder[OriCount - 1];
		if (GetDis(Pt0.X, Pt0.Y, Pt1.X, Pt1.Y) >= 0.0001)
		{
			//让边界闭合
			m_OriBoder.push_back(Pt0);
		}
	}

	/*-------------------------------------------*/

	/*-------------------------------------------*/
	//根据井位距离计算变异距离m_B的值 20150205  变异距离是什么？？
	double k = sqrt(pow(m_XMax - m_XMin, 2) + pow(m_YMax - m_YMin, 2));
	//cout << "变异距离：" << m_B << endl;
	if (m_B <= k)
	{
		m_B = k+2;		//主要是防止西北那边特别大的范围
	}
	/*-------------------------------------------*/

	//GetRectBoder();		//使用矩形边界（自己构造边界）20131025

	//判断是否有边界；如果有边界，就从边界计算凸包，否则从数据点计算凸包
	//20131111m_Border中一直有数值，区别是单纯的井位数据还是人工数据
	//由数据点进行凸包的计算
	if ((int)m_Border.size() <= 2)
	{
		m_Border.clear();
		//convexBag = Convex(m_oriData);
		convexBag = Withershins(m_oriData);	//测试新的凸包函数20131108 构造凸包
		//使用B样条进行凸包边界的优化
		//OptimizeBoder(convexBag,200);			
		//使用贝塞尔曲线进行凸包边界的优化
		OptimizeBorderBezier(convexBag,exp);
		m_Border = convexBag;
		m_OriBoder = m_Border;	//原始边界信息由m_OriBoder  20140806
	}
	//由边界进行凸包的计算
	else
	{
		//cout << "hello2" << endl;
		convexBag = Withershins(m_Border);  //测试20131107  构造凸包
		//使用B样条进行凸包边界的优化
		//OptimizeBoder(convexBag);
		//使用贝塞尔曲线进行凸包边界的优化
		OptimizeBorderBezier(convexBag,exp); 
		m_Border = convexBag;
	}

	std::ofstream out("convexhull_test_point.obj");
	//out << "v " << convexBag[379].X << " " << convexBag[376].Y << " " << "0" << endl;
	//out << "v " << convexBag[499].X << " " << convexBag[499].Y << " " << "0" << endl;
	/*out << "v " << convexBag[377].X << " " << convexBag[377].Y << " " << "0" << endl;
	out << "v " << convexBag[378].X << " " << convexBag[378].Y << " " << "0" << endl;
	out << "v " << convexBag[389].X << " " << convexBag[389].Y << " " << "0" << endl;
	out << "v " << convexBag[390].X << " " << convexBag[390].Y << " " << "0" << endl;
	out << "v " << convexBag[391].X << " " << convexBag[391].Y << " " << "0" << endl;*/
	//out << "v " << convexBag[0].X << " " << convexBag[0].Y << " " << "0" << endl;
	//out << "v " << convexBag[126].X << " " << convexBag[126].Y << " " << "0" << endl;
	//out << "v " << convexBag[252].X << " " << convexBag[252].Y << " " << "0" << endl;
	//out.close();
	vector<int> FourPort(4);
	

	DividedFourParts((int)(convexBag.size() - 1), FourPort);//优化边界后凸包点分为上下左右四组

	//cout << "=========================================" << endl;
	//cout << "输出凸包点分组的一些信息" << endl;
	//cout << "FourPort[0]= " << FourPort[0] << " " << "FourPort[1]= " << FourPort[1] << " " << "FourPort[2]= " << FourPort[2] << " " << "FourPort[3]= " << FourPort[3] << endl;

	vector<Data> upLine, downLine, leftLine, rightLine;

	vector<Dis> upDis, downdis, leftDis, rightDis;

	//upLine、downline、leftLine、rightLine存储了凸包四部分边界上的点的位置
	//upDis、downDis、leftDis、rightDis中的dis分量存储了对应点到第一个点的距离
	BordersPointDis(FourPort, convexBag, upLine, downLine, leftLine, rightLine,
		upDis, downdis, leftDis, rightDis);

	//计算upLine、downline、leftLine、rightLine中Dis分量占总的长度的半分比
	//计算upDis、downdis、leftDis、rightDis中angle分量，与x轴正方向的夹角
	BordersChar(upLine, downLine, leftLine, rightLine, upDis, downdis, leftDis, rightDis);

	vector<Data> DL, DR, DD, DU;
	vector<TWOVALUE> sim1(5), sim2, sim3, sim4;
	BordersPoints(FourPort, convexBag, upLine, downLine, leftLine, rightLine,
		upDis, downdis, leftDis, rightDis, DL, DR, DD, DU,
		sim1, sim2, sim3, sim4, m_XNum, m_YNum);

	double s, t;
	int XM = m_XNum, YM = m_YNum;

	int GridPoint_size = m_GridPoint.size();
	if (GridPoint_size >0)
	{
		for (int i = 0; i<GridPoint_size; i++)
		{
			vector <THRVALUE>().swap(m_GridPoint[i]);
			m_GridPoint[i].clear();
		}
	}
	vector <vector<THRVALUE>>().swap(m_GridPoint);
	m_GridPoint.clear();
	m_GridPoint.resize(m_XNum + 3);
	m_GridPoint[0].resize(m_XNum + 3);

	for (int i = 1; i <= m_XNum; i++)
	{
		m_GridPoint[i].resize(m_XNum + 3);
	}

	m_GridPoint[m_XNum + 1].resize(m_XNum + 3);
	m_GridPoint[m_XNum + 2].resize(m_XNum + 3);

	for (int i = 1; i <= XM; i++)
	{
		s = (double)(i - 1) / (XM - 1);
		for (int j = 1; j <= YM; j++)
		{
			t = (double)(j - 1) / (YM - 1);
			//注意这里t和s的使用
			m_GridPoint[i][j].X = (DL[j].X * (1 - s) + DR[j].X * s) + (DU[i].X * t + DD[i].X * (1 - t)) - (sim1[1].X + sim2[i].X + sim3[j].X + t * sim4[i].X);
			m_GridPoint[i][j].Y = (DL[j].Y * (1 - s) + DR[j].Y * s) + (DU[i].Y * t + DD[i].Y * (1 - t)) - (sim1[1].Y + sim2[i].Y + sim3[j].Y + t * sim4[i].Y);
			m_GridPoint[i][j].Z = 0.0;//20130814
		}
	}
	//利用swap函数进行vector内存的释放
	DL.clear();
	vector <Data>().swap(DL);

	DR.clear();
	vector <Data>().swap(DR);

	DD.clear();
	vector <Data>().swap(DD);

	DU.clear();
	vector <Data>().swap(DU);

	sim2.clear();
	vector <TWOVALUE>().swap(sim2);

	sim3.clear();
	vector <TWOVALUE>().swap(sim3);

	sim4.clear();
	vector <TWOVALUE>().swap(sim4);

	sim1.clear();
	vector <TWOVALUE>().swap(sim1);

	upLine.clear();
	vector <Data>().swap(upLine);

	downLine.clear();
	vector <Data>().swap(downLine);

	leftLine.clear();
	vector <Data>().swap(leftLine);

	rightLine.clear();
	vector <Data>().swap(rightLine);

	upDis.clear();
	vector <Dis>().swap(upDis);

	downdis.clear();
	vector <Dis>().swap(downdis);

	leftDis.clear();
	vector <Dis>().swap(leftDis);

	rightDis.clear();
	vector <Dis>().swap(rightDis);
}
//得到一个数的数量级
double Jie_Ds::GetMagnitude(double fNumber)
{
	//数量级
	double magnitudeValue = 1.0;

	if (fNumber == 0.0)
		return(0.0);

	//是否为负数
	bool bNegative;
	bNegative = (fNumber<0) ? true : false;

	double positiveNumber = abs(fNumber);
	if (positiveNumber == 1)
	{//等于1	
		magnitudeValue = 1.0;
	}
	else if (positiveNumber<1.0)
	{//小于1
		while (positiveNumber<1.0)
		{
			positiveNumber *= 10.0;
			magnitudeValue /= 10.0;
		}
	}
	else
	{//大于1
		while (positiveNumber>1.0)
		{
			positiveNumber /= 10.0;
			magnitudeValue *= 10.0;
		}
		magnitudeValue /= 10.0;
	}
	return magnitudeValue;
}
float Jie_Ds::FindStep(float StepMin, bool bUporDown)
{
	string str;
	float dStep, dStepOld, RetVal;
	if (StepMin == 0.0f)
		return(0.0f);

	//是否为负数
	bool bNegative = false;
	bNegative = (StepMin<0) ? true : false;

	if (bNegative)
	{//负数的绝对值的规整方向与之规整方向相反
		bUporDown = !bUporDown;
	}

	//首先按照正数计算
	StepMin = (float)fabs(StepMin);     //若小于0,则取绝对值

	if (!bUporDown)
	{//向下规整
	 //计算渐变母值
		dStep = (float)GetMagnitude(double(StepMin));//得到这个数的数量级
		dStep *= 10;

		while (StepMin < dStep)
		{
			dStepOld = dStep;
			dStep = dStep / 2.5f;
			if (StepMin < dStep)
			{
				dStepOld = dStep;
				dStep = dStep / 2;
			}
			if (StepMin < dStep)
			{
				dStepOld = dStep;
				dStep = dStep / 2.0f;
			}
		}
		RetVal = dStep;
	}
	else
	{//向上规整
	 //计算渐变母值
		dStep = (float)GetMagnitude(double(StepMin));//得到这个数的数量级
		while (StepMin > dStep)
		{
			dStep *= 2.5f;
			if (StepMin > dStep)
			{
				dStep *= 2.0f;
			}
			else
			{
				dStep /= 2.5f;
				dStep *= 2.0f;
			}
		}
		RetVal = dStep;
		//	RetVal = FindMaxFloatStep(StepMin);
	}

	//负数按正数规整后还原其符号
	if (bNegative)
	{
		RetVal *= -1;
	}
	return(RetVal);
}
void Jie_Ds::CalcSameArray()
{
	//等值线最小显示值
	m_Show_MinValue = FindStep((float)m_ZMin, false);//向下规整小值

	//等值线最大显示值
	m_Show_MaxValue = FindStep((float)(m_ZMax), true);//向上规整大值

																//等值线间隔
	m_Show_JianGeValue = (m_Show_MaxValue - m_Show_MinValue) / 2;

	m_sameArray.RemoveAll();
	double minvalue = m_Show_MinValue;
	while (minvalue < m_Show_MaxValue)
	{
		m_sameArray.Add(minvalue);
		minvalue += m_Show_JianGeValue;
	}

	if (minvalue != m_Show_MaxValue)
	{
		m_sameArray.Add(m_Show_MaxValue);
	}
}
void Jie_Ds::SetTrackValue(vector<double> Track)
{
	m_TrackValue.clear();
	for (int i = 0; i < (int)Track.size(); i++)
	{
		m_TrackValue.push_back(Track[i]);
	}
	if ((int)m_TrackValue.size() > 2)
	{
		m_valuedis = abs(m_TrackValue[1] - m_TrackValue[0]);
	}
	else
	{
		m_valuedis = 1.0;
	}
}

//得到距离A点最近的点20140916
void Jie_Ds::GetMinDis(THRVALUE A, vector<THRVALUE>Pts, THRVALUE &Point)
{
	double Distance = pow(Pts[0].X - A.X, 2) + pow(Pts[0].Y - A.Y, 2);
	Point = Pts[0];
	for (int i = 1; i < (int)Pts.size(); i++)
	{
		double d = pow(Pts[i].X - A.X, 2) + pow(Pts[i].Y - A.Y, 2);
		if (d < Distance)
		{
			Distance = d;
			Point = Pts[i];
		}
	}
}

//X方向上含断层的的等值点201301015
void Jie_Ds::X_FalutEquivalent(int X_index, int Y_index, double Value, vector<TWOVALUE>&VirtualIJ)
{
	THRVALUE Star = m_GridPoint[X_index][Y_index];
	THRVALUE End = m_GridPoint[X_index + 1][Y_index];

	TWOVALUE A, B;
	A.X = Star.X;
	A.Y = Star.Y;

	B.X = End.X;
	B.Y = End.Y;
	/*-----------------------------------------------------------------*/
	//20140916
	vector<THRVALUE>TempPts = m_XFaultPts[X_index][Y_index].FLine;
	if ((int)TempPts.size() <= 0)
	{
		return;
	}
	//vector<TWOVALUE> TempPts;
	//for (int i = 0 ; i < (int)m_vecFaultLines.size() ; i ++)
	//{
	//	FalutLine LineOne = m_vecFaultLines[i];
	//	for (int j = 0 ; j < (int)LineOne.FLine.size() - 1 ; j ++)
	//	{
	//		TWOVALUE p1,p2,Point;
	//		p1 = LineOne.FLine[j];
	//		p2 = LineOne.FLine[j+1];
	//		if (L2L_Intersect(A,B,p1,p2,Point))
	//		{
	//			TempPts.push_back(Point);
	//		}
	//	}
	//}
	/*-----------------------------------------------------------------*/
	//if (!IsInside(A))
	if (!m_GridInFault[X_index][Y_index])
	{
		/*-----------------------------------------------------------------*/
		//20140916
		//TWOVALUE Pt;
		//GetMinDis(A,TempPts,Pt);	//得到距离A点最近的点
		//THRVALUE Dataij;
		//Dataij.X = Pt.X ;
		//Dataij.Y = Pt.Y ;
		//Dataij.Z = 0;
		//GetValueByK(Dataij);
		THRVALUE Dataij;
		GetMinDis(Star, TempPts, Dataij);
		/*-----------------------------------------------------------------*/
		if ((Dataij.Z - Value) * (Star.Z - Value) < 0)
		{
			double t = abs((Value - Star.Z) / (Dataij.Z - Star.Z));
			double d1 = sqrt(pow(Dataij.X - Star.X, 2) + pow(Dataij.Y - Star.Y, 2));

			double d2 = sqrt(pow(End.X - Star.X, 2) + pow(End.Y - Star.Y, 2));

			TWOVALUE P;
			P.X = X_index + t * d1 / d2;
			P.Y = Y_index;
			VirtualIJ.push_back(P);
		}
	}

	//if (!IsInside(B))
	if (!m_GridInFault[X_index + 1][Y_index])
	{
		/*-----------------------------------------------------------------*/
		//20140916
		//TWOVALUE Pt;
		//GetMinDis(B,TempPts,Pt);	//得到距离B点最近的点
		//THRVALUE Dataij;
		//Dataij.X = Pt.X ;
		//Dataij.Y = Pt.Y ;
		//Dataij.Z = 0;
		//GetValueByK(Dataij);
		THRVALUE Dataij;
		GetMinDis(End, TempPts, Dataij);
		/*-----------------------------------------------------------------*/
		if ((Dataij.Z - Value) * (End.Z - Value) < 0)
		{
			double t = abs((End.Z - Value) / (End.Z - Dataij.Z));
			double d1 = sqrt(pow(Dataij.X - End.X, 2) + pow(Dataij.Y - End.Y, 2));

			double d2 = sqrt(pow(End.X - Star.X, 2) + pow(End.Y - Star.Y, 2));

			TWOVALUE P;
			P.X = X_index + 1 - t * d1 / d2;
			P.Y = Y_index;
			VirtualIJ.push_back(P);
		}
	}
}


//Y方向上含断层的的等值点201301015
void Jie_Ds::Y_FalutEquivalent(int X_index, int Y_index, double Value, vector<TWOVALUE>&VirtualIJ)
{
	THRVALUE Star = m_GridPoint[X_index][Y_index];
	THRVALUE End = m_GridPoint[X_index][Y_index + 1];

	TWOVALUE A, B;
	A.X = Star.X;
	A.Y = Star.Y;

	B.X = End.X;
	B.Y = End.Y;

	/*-----------------------------------------------------------------*/
	//20140916
	vector<THRVALUE> TempPts = m_YFaultPts[X_index][Y_index].FLine;
	//for (int i = 0; i < TempPts.size(); i++)
	//	cout << "v " << TempPts[i].X << " " << TempPts[i].Y << " " << "0" << endl;
	if ((int)TempPts.size() <= 0)
	{
		return;
	}
	//vector<TWOVALUE> TempPts;
	//for (int i = 0 ; i < (int)m_vecFaultLines.size() ; i ++)
	//{
	//	FalutLine LineOne = m_vecFaultLines[i];
	//	for (int j = 0 ; j < (int)LineOne.FLine.size() - 1 ; j ++)
	//	{
	//		TWOVALUE p1,p2,Point;
	//		p1 = LineOne.FLine[j];
	//		p2 = LineOne.FLine[j+1];
	//		if (L2L_Intersect(A,B,p1,p2,Point))
	//		{
	//			TempPts.push_back(Point);
	//		}
	//	}
	//}
	/*-----------------------------------------------------------------*/
	//if (!IsInside(A))
	//此网格点不能在断层内部
	if (!m_GridInFault[X_index][Y_index])
	{
		/*-----------------------------------------------------------------*/
		//20140916
		//TWOVALUE Pt;
		//GetMinDis(A,TempPts,Pt);	//得到距离A点最近的点
		//THRVALUE Dataij;
		//Dataij.X = Pt.X ;
		//Dataij.Y = Pt.Y ;
		//Dataij.Z = 0;
		//GetValueByK(Dataij);
		THRVALUE Dataij;
		GetMinDis(Star, TempPts, Dataij);
		/*-----------------------------------------------------------------*/
		if ((Dataij.Z - Value) * (Star.Z - Value) < 0)
		{
			double t = abs((Value - Star.Z) / (Dataij.Z - Star.Z));
			double d1 = sqrt(pow(Dataij.X - Star.X, 2) + pow(Dataij.Y - Star.Y, 2));
			double d2 = sqrt(pow(End.X - Star.X, 2) + pow(End.Y - Star.Y, 2));

			TWOVALUE P;
			P.X = X_index;
			P.Y = Y_index + t * d1 / d2;
			VirtualIJ.push_back(P);
		}
	}

	//if (!IsInside(B))
	if (!m_GridInFault[X_index][Y_index + 1])
	{
		/*-----------------------------------------------------------------*/
		//20140916
		//TWOVALUE Pt;
		//GetMinDis(B,TempPts,Pt);	//得到距离B点最近的点
		//THRVALUE Dataij;
		//Dataij.X = Pt.X ;
		//Dataij.Y = Pt.Y ;
		//Dataij.Z = 0;
		//GetValueByK(Dataij);
		THRVALUE Dataij;
		GetMinDis(End, TempPts, Dataij);
		/*-----------------------------------------------------------------*/
		if ((Dataij.Z - Value) * (End.Z - Value) < 0)
		{
			double t = abs((End.Z - Value) / (End.Z - Dataij.Z));
			double d1 = sqrt(pow(Dataij.X - End.X, 2) + pow(Dataij.Y - End.Y, 2));
			double d2 = sqrt(pow(End.X - Star.X, 2) + pow(End.Y - Star.Y, 2));

			TWOVALUE P;
			P.X = X_index;
			P.Y = Y_index + 1 - t * d1 / d2;
			VirtualIJ.push_back(P);
		}
	}
}
//得到所有的等值点
//void Jie_Ds::EquivalentPoints(double Value, vector<THRVALUE>&Jie_VirtualIJK)
void Jie_Ds::EquivalentPoints(double Value, vector<TWOVALUE>&VirtualIJ)
{
	IsoLine temp_Line;
	VirtualIJ.clear();
	for (int i = 1; i <= m_XNum; i++)//先求纵边上的等值点
	{
		for (int j = 1; j < m_YNum; j++)
		{
			if (m_YFault[i][j])
			{
				//并且两边有交点 20130913
				Y_FalutEquivalent(i, j, Value, VirtualIJ);//得到特殊的等值点 20131015
				continue;
			}
			//判断此边上是否有等值点
			if ((m_GridPoint[i][j].Z - Value) * (m_GridPoint[i][j + 1].Z - Value) < 0)
			{
				TWOVALUE P;
				P.X = i;
				//这里的t值一定是正的！！！！！
				double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i][j + 1].Z - m_GridPoint[i][j].Z));
				t = abs(t);
				P.Y = j + t;
				//if (t >= 1.0)
				//{
				//	int sk = 1;
				//	ASSERT(sk);
				//	CString str;
				//	str.Format("xt错误 %f,%f,%f", Value, m_GridPoint[i][j].Z, m_GridPoint[i][j + 1].Z);
				//	//AfxMessageBox(str);
				//}
				TWOVALUE RealyPoint;
				RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i][j + 1].X - m_GridPoint[i][j].X);
				RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i][j + 1].Y - m_GridPoint[i][j].Y);
				/*if (IsInside(RealyPoint))
				{
					continue;
				}
				else*/
				VirtualIJ.push_back(P);
				//Jie_VirtualIJK.push_back(P1);
					temp_Line.Logic.push_back(RealyPoint);
					//temp_Line.Value = Value;
					//Jie_IsoLine.push_back(temp_Line);
					//VirtualIJ1.push_back(RealyPoint);
			}
			else if ((m_GridPoint[i][j].Z - Value) * (m_GridPoint[i][j + 1].Z - Value) == 0)
			{
				//cout << "纵边等值点" << endl;
				if (m_GridPoint[i][j].Z == Value)
				{
					if (m_GridPoint[i][j].Z < m_GridPoint[i][j + 1].Z)
						m_GridPoint[i][j].Z -= thea;
					else
						m_GridPoint[i][j].Z += thea;
					TWOVALUE P;
					P.X = i;
					//这里的t值一定是正的！！！！！
					double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i][j + 1].Z - m_GridPoint[i][j].Z));
					t = abs(t);
					P.Y = j + t;
					TWOVALUE RealyPoint;
					RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i][j + 1].X - m_GridPoint[i][j].X);
					RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i][j + 1].Y - m_GridPoint[i][j].Y);
					VirtualIJ.push_back(P);
					temp_Line.Logic.push_back(RealyPoint);
				}
				else
				{
					if (m_GridPoint[i][j].Z < m_GridPoint[i][j + 1].Z)
						m_GridPoint[i][j+1].Z += thea;
					else
						m_GridPoint[i][j+1].Z -= thea;
					TWOVALUE P;
					P.X = i;
					//这里的t值一定是正的！！！！！
					double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i][j + 1].Z - m_GridPoint[i][j].Z));
					t = abs(t);
					P.Y = j + t;
					TWOVALUE RealyPoint;
					RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i][j + 1].X - m_GridPoint[i][j].X);
					RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i][j + 1].Y - m_GridPoint[i][j].Y);
					VirtualIJ.push_back(P);
					temp_Line.Logic.push_back(RealyPoint);
				}
			}
		}
	}
	for (int j = 1; j <= m_YNum; j++)//求横边上的等值点
	{
		for (int i = 1; i < m_XNum; i++)
		{
			if (m_XFault[i][j])
			{
				//并且两边有交点 20130913
				X_FalutEquivalent(i, j, Value, VirtualIJ);//得到特殊的等值点 20131015
				continue;
			}
			if ((m_GridPoint[i][j].Z - Value) * (m_GridPoint[i + 1][j].Z - Value) < 0)
			{
				TWOVALUE P;
				double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i + 1][j].Z - m_GridPoint[i][j].Z));
				t = abs(t);
				P.X = i + t;
				P.Y = j;
				if (t >= 1.0 || t<0.0)
				{
					//AfxMessageBox("yt错误");
				}
				TWOVALUE RealyPoint;
				RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i + 1][j].X - m_GridPoint[i][j].X);
				RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i + 1][j].Y - m_GridPoint[i][j].Y);
				/*if (IsInside(RealyPoint))
				{
					continue;
				}*/
				VirtualIJ.push_back(P);
				//Jie_VirtualIJK.push_back(P1);
				//VirtualIJ1.push_back(RealyPoint);
				temp_Line.Logic.push_back(RealyPoint);
				
			}
			else if ((m_GridPoint[i][j].Z - Value) * (m_GridPoint[i+1][j].Z - Value) == 0)
			{
				//cout << "横边等值点" << endl;
				if (m_GridPoint[i][j].Z == Value)
				{
					if (m_GridPoint[i][j].Z < m_GridPoint[i+1][j].Z)
						m_GridPoint[i][j].Z -= thea;
					else
						m_GridPoint[i][j].Z += thea;
					TWOVALUE P;
					P.X = i;
					//这里的t值一定是正的！！！！！
					double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i+1][j].Z - m_GridPoint[i][j].Z));
					t = abs(t);
					P.Y = j + t;
					TWOVALUE RealyPoint;
					RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i+1][j].X - m_GridPoint[i][j].X);
					RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i+1][j].Y - m_GridPoint[i][j].Y);
					VirtualIJ.push_back(P);
					temp_Line.Logic.push_back(RealyPoint);
				}
				else
				{
					if (m_GridPoint[i+1][j].Z < m_GridPoint[i][j].Z)
						m_GridPoint[i+1][j].Z -= thea;
					else
						m_GridPoint[i+1][j].Z += thea;
					TWOVALUE P;
					P.X = i;
					//这里的t值一定是正的！！！！！
					double t = ((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i+1][j].Z - m_GridPoint[i][j].Z));
					t = abs(t);
					P.Y = j + t;
					TWOVALUE RealyPoint;
					RealyPoint.X = m_GridPoint[i][j].X + t*(m_GridPoint[i+1][j].X - m_GridPoint[i][j].X);
					RealyPoint.Y = m_GridPoint[i][j].Y + t*(m_GridPoint[i+1][j].Y - m_GridPoint[i][j].Y);
					VirtualIJ.push_back(P);
					temp_Line.Logic.push_back(RealyPoint);
				}
			}
		}
	}
	temp_Line.Value = Value;
	Jie_IsoLine.push_back(temp_Line);
	//std::cout << "================================================" << endl;
	//std::cout << "================================================" << endl;
	//for (int i = 0; i < VirtualIJ1.size(); i++)
	//{
	//	std::cout << "v " << VirtualIJ1[i].X << " " << VirtualIJ1[i].Y << " " << "0" << endl;
    //}
	//std::cout << "================================================" << endl;
	//std::cout << "================================================" << endl;
}

//这里要注意，这种通过vector<vector>定义的二维数组，第一个值在左上角，而非左下角
void Jie_Ds::SignBorder(vector<vector<double>>& biaoji1, vector<vector<double>>& biaoji2)
{
	biaoji1.clear();
	biaoji2.clear();
	biaoji1.resize(m_XNum + 1);
	biaoji2.resize(m_XNum + 2);

	for (int i = 0; i <= m_XNum; i++)
	{
		biaoji1[i].resize(m_YNum + 2);
		biaoji2[i].resize(m_YNum + 1);
		biaoji1[i][0] = 1;
		biaoji1[i][m_YNum + 1] = 1;
		biaoji2[i][0] = 1;
		biaoji2[i][m_YNum] = 1;
	}
	
	biaoji2[m_XNum + 1].resize(m_YNum + 1);
	biaoji2[m_XNum + 1][0] = 1;
	biaoji2[m_XNum + 1][m_YNum] = 1;

	for (int i = 1; i<m_YNum; i++)
	{
		biaoji2[0][i] = 1;
		biaoji2[m_XNum + 1][i] = 1;
		biaoji1[0][i] = 1;
		biaoji1[m_XNum][i] = 1;
	}
	biaoji1[0][m_YNum] = 1;
	biaoji1[m_XNum][m_YNum] = 1;
}
//转化一个点
void Jie_Ds::GetReallyPoint(TWOVALUE A, TWOVALUE &B)
{
	double X1 = A.X;
	double Y1 = A.Y;
	int X0 = (int)X1;
	int Y0 = (int)Y1;
	double Mark = 0, C, D;
	//说明是一个网格点
	if (X1 - X0 == 0 && Y1 - Y0 == 0)
	{
		C = m_GridPoint[X0][Y0].X;
		D = m_GridPoint[X0][Y0].Y;
	}
	//说明在横轴
	else if (X1 - X0 == 0 && Y1 - Y0 != 0)
	{
		Mark = abs(Y1 - Y0);
		C = (1 - Mark) * m_GridPoint[X0][Y0].X + Mark * (m_GridPoint[X0][Y0 + 1].X);//类似于线性插值
		D = (1 - Mark) * m_GridPoint[X0][Y0].Y + Mark * (m_GridPoint[X0][Y0 + 1].Y);
	}
	else if (X1 - X0 != 0 && Y1 - Y0 == 0)
	{
		Mark = abs(X1 - X0);
		C = (1 - Mark) * m_GridPoint[X0][Y0].X + Mark * (m_GridPoint[X0 + 1][Y0].X);
		D = (1 - Mark) * m_GridPoint[X0][Y0].Y + Mark * (m_GridPoint[X0 + 1][Y0].Y);
	}
	B.X = C;
	B.Y = D;

}
//点是否在直线上 
bool Jie_Ds::IsinLineK(THRVALUE Star, THRVALUE End, THRVALUE Point)
{
	End.X = End.X - Star.X;
	End.Y = End.Y - Star.Y;
	Point.X = Point.X - Star.X;
	Point.Y = Point.Y - Star.Y;
	Star.X = 0;
	Star.Y = 0;					//平移，对判断是否一条直线不造成影响··将大数化为小数~能提高计算精度
	if (Point.X *(Point.X - End.X) >0 && Point.Y *(Point.Y - End.Y) > 0)
	{
		return false;
	}
	double A = Star.Y - End.Y;
	double B = End.X - Star.X;
	//double C = -A * Star.X- B * Star.Y;
	double C = 0;
	double D = sqrt(pow(A, 2) + pow(B, 2));
	double d = A * (Point.X - Star.X) + B * (Point.Y - Star.Y);
	d = d / D;
	if (abs(d)<0.00001)
	{
		return true;
	}
	return false;

}
bool Jie_Ds::IsinLineK(THRVALUE Star, THRVALUE End, TWOVALUE Point)
{
	End.X = End.X - Star.X;
	End.Y = End.Y - Star.Y;
	Point.X = Point.X - Star.X;
	Point.Y = Point.Y - Star.Y;
	Star.X = 0;
	Star.Y = 0;					//平移，对判断是否一条直线不造成影响··将大数化为小数~能提高计算精度
	if (Point.X *(Point.X - End.X) >0 && Point.Y *(Point.Y - End.Y) > 0)
	{
		return false;
	}
	double A = Star.Y - End.Y;
	double B = End.X - Star.X;
	//double C = -A * Star.X- B * Star.Y;
	double C = 0;
	double D = sqrt(pow(A, 2) + pow(B, 2));
	double d = A * (Point.X - Star.X) + B * (Point.Y - Star.Y);
	d = d / D;
	if (abs(d)<0.00001)
	{
		return true;
	}
	return false;

}
//点在线段中的比例 20140915
double Jie_Ds::GetIndex(THRVALUE Star, THRVALUE End, THRVALUE Point)
{
	double t = 0.0;
	if (abs(Star.X - End.X) > abs(Star.Y - End.Y))
	{
		t = (Point.X - Star.X) / (End.X - Star.X);
	}
	else
	{
		t = (Point.Y - Star.Y) / (End.Y - Star.Y);
	}
	return t;

}
//判断断层线首尾点所在的区间序号20140724
vector<FLOValue> Jie_Ds::GetFalutindex(THRVALUE GridIJ[6], GridFL Lines)
{
	vector<FLOValue> Index;			//先加入首点位置，再加末点位置
	for (int i = 0; i < (int)Lines.FLines.size(); i++)
	{
		SingleFalut Falt = Lines.FLines[i];

		/*--------------------------------------------*/
		//20140915
		//TWOVALUE Star = Falt.FLine[0];
		//TWOVALUE End  = Falt.FLine[(int)Falt.FLine.size() - 1];
		//当前断层线的首尾点
		THRVALUE Star = Falt.FLine[0];
		THRVALUE End = Falt.FLine[(int)Falt.FLine.size() - 1];
		/*--------------------------------------------*/
		FLOValue  OneFalut;
		OneFalut.FLines = Falt;
		//首点序号
		for (int j = 0; j < 5; j++)
		{
			THRVALUE StarA = GridIJ[j];
			THRVALUE EndA = GridIJ[j + 1];
			if (IsinLineK(StarA, EndA, Star))
			{
				OneFalut.Star = GetIndex(StarA, EndA, Star) + j;
			}
		}
		//尾点序号
		for (int j = 0; j < 5; j++)
		{
			THRVALUE StarA = GridIJ[j];
			THRVALUE EndA = GridIJ[j + 1];
			if (IsinLineK(StarA, EndA, End))
			{
				OneFalut.End = GetIndex(StarA, EndA, End) + j;
			}
		}
		Index.push_back(OneFalut);
	}
	return Index;

}
//从star点开始，第一条等值线确定的区域加入到数组  20140724
void Jie_Ds::GetCrossMe(THRVALUE GridIJ[6], vector<FLOValue> &FLIndex, double &Star, vector<THRVALUE> &FlautArae)
{
	int Colc = 0;			//首点为最小值
	int index = 0;			//终点为最小值
	double minx = 100;		//star后的第一个断层点
	double next = 100;		//断层对应的连接点
	//cout << FLIndex.size() << endl;
	//cout << FLIndex[0].FLines.FLine.size() << endl;
	//for (int i = 0; i < FLIndex[0].FLines.FLine.size(); i++)
	//	cout << "v " << FLIndex[0].FLines.FLine[i].X << " " << FLIndex[0].FLines.FLine[i].Y << " " << "0" << endl;;
	for (int i = 0; i < (int)FLIndex.size(); i++)	//寻找Star后的第一个断层点
	{
		FLOValue FlO = FLIndex[i];
		//cout << Star << endl;
		//cout << FlO.Star << endl;
		//cout << FlO.End << endl;
		if (FlO.Star > Star && FlO.Star < minx)
		{
			//cout << "hello1" << endl;
			Colc = 0;
			index = i;
			minx = FlO.Star;
			next = FlO.End;
		}
		if (FlO.End > Star && FlO.End < minx)
		{
			//cout << "hello2" << endl;
			Colc = 1;
			index = i;
			minx = FlO.End;
			next = FlO.Star;
		}
	}

	//cout << endl;
	//cout << "hello3" << endl;
	//cout << endl;

	//for循环没有进行操作
	if (minx == 100)
	{
		//cout << "hello4" << endl;
		FLIndex.erase(FLIndex.begin());	//删除不用的数据
		return;
	}
	if (minx >= (int)Star + 1)
	{
		for (int i = (int)Star + 1; i < minx; i++)
		{
			FlautArae.push_back(GridIJ[i]);
		}
	}
	FLOValue FlO = FLIndex[index];
	int Num = FlO.FLines.FLine.size();
	/*--------------------------------------------*/
	//20140915
	//vector<TWOVALUE> PtLine = FlO.FLines.FLine;
	vector<THRVALUE> PtLine = FlO.FLines.FLine;
	/*--------------------------------------------*/
	if (Colc == 0)							//顺序加入断层数据
	{
		for (int i = 0; i < Num; i++)
		{
			/*-----------------------------------*/
			//20140915
			//THRVALUE Pt;
			//Pt.X = PtLine[i].X;
			//Pt.Y = PtLine[i].Y;
			//Pt.Z = 0.0;
			THRVALUE Pt = PtLine[i];
			/*-----------------------------------*/

			FlautArae.push_back(Pt);
		}
	}
	else									//逆序加入断层数据
	{
		for (int i = Num - 1; i >= 0; i--)
		{
			/*-----------------------------------*/
			//20140915
			//THRVALUE Pt;
			//Pt.X = PtLine[i].X;
			//Pt.Y = PtLine[i].Y;
			//Pt.Z = 0.0;
			THRVALUE Pt = PtLine[i];
			/*-----------------------------------*/
			FlautArae.push_back(Pt);
		}
	}
	Star = next;
	FLIndex.erase(FLIndex.begin() + index);	//删除不用的数据,当前的断层线已经操作完成
}

//得到用于估值的区域20140724
vector<THRVALUE> Jie_Ds::GetFalutAera(THRVALUE GridIJ[6], GridFL Lines)
{
	vector<THRVALUE> FlautLine;
	FlautLine.push_back(GridIJ[0]);//已经把等值点加入到容器中

	//得到每一个网格内断层线的信息，以及每一条断层线的首点和尾点在此网格中的位置编号
	vector<FLOValue> FLIndex = GetFalutindex(GridIJ, Lines);
	double Star = 0;
	//for (int i = 0 ; i < 6 ; i ++)
	//{
	//	GetCrossMe(GridIJ,Lines,i,FlautLine);
	//}
	for (double i = 0; i < 6; )
	{
		GetCrossMe(GridIJ, FLIndex, i, FlautLine);
		if (FLIndex.size() < 1)
		{
			//cout << "yes" << endl;
			for (int k = i + 1; k < 6; k++)
			{
				//cout << k << endl;
				FlautLine.push_back(GridIJ[k]);
			}
			break;
		}
	}
	return FlautLine;
}
//点是否在网格上20140724
BOOL Jie_Ds::IsOnGrid(THRVALUE GridIJ[6], TWOVALUE PT)
{
	for (int i = 0; i < 5; i++)
	{
		THRVALUE P1 = GridIJ[i];
		THRVALUE P2 = GridIJ[i + 1];
		if (IsinLineK(P1, P2, PT))
		{
			return TRUE;
		}
	}
	return FALSE;
}

//得到等值点外包的虚拟属性值  GridIJ以等值点为起点的边界坐标 Lines网格内部的断层线坐标   
BOOL Jie_Ds::GetVirtualValue(THRVALUE GridIJ[6], GridFL Lines, TWOVALUE &Point)
{
	vector<THRVALUE>TempPoint;
	vector<BOOL>Mark;			//存储点为断层点还是网格点，TURE为断层点
	vector<THRVALUE>TestPoint;
	//int index = 0;
	//GetFirstLine(GridIJ,Lines,index);	//得到断层与线段的第一个交点
	//for (int i = 0 ; i < index + 1; i ++)
	//{
	//	TempPoint.push_back(GridIJ[i]);
	//	Mark.push_back(TRUE);			//TURE为网格点
	//	TestPoint.push_back(GridIJ[i]);
	//}
	//if (index < 5)						//得到交点对应的断层线数据
	//{
	//	GetCrossFault(GridIJ[index],GridIJ[index + 1],Lines,TempPoint,index);
	//}
	//int Num = Mark.size() ;
	//for (int i = Num ; i < (int)TempPoint.size() ; i ++)
	//{
	//	Mark.push_back(FALSE);
	//}
	//Num = (int)TempPoint.size() - 1;
	//THRVALUE EndP = TempPoint[Num];
	//GetLastLine(GridIJ,EndP,index);		//得到交点对应的线段，最后一断层点个位置
	//for (int i = index ; i < 6 ; i++)
	//{
	//	TempPoint.push_back(GridIJ[i]);
	//	Mark.push_back(TRUE);
	//}
	//vector<TWOVALUE> CrossPoint;
	//vector<BOOL> CrossMark;
	//double Z = GridIJ[0].Z;//首点为等值点
	//for (int i = 0 ; i < (int)TempPoint.size() - 1 ;i++)	//不计算第一节和最后一节线段
	//{
	//	double A = TempPoint[i].Z;
	//	double B = TempPoint[i+1].Z;
	//	if ((A-Z)*(B-Z)<0)
	//	{
	//		TWOVALUE P;
	//		P.X = (Z-A)*(TempPoint[i+1].X - TempPoint[i].X)/(B-A) + TempPoint[i].X;
	//		P.Y = (Z-A)*(TempPoint[i+1].Y - TempPoint[i].Y)/(B-A) + TempPoint[i].Y;
	//		CrossPoint.push_back(P);
	//		if (!Mark[i] && !Mark[i+1])
	//		{
	//			CrossMark.push_back(FALSE);
	//		}
	//		else
	//		{
	//			CrossMark.push_back(TRUE);
	//		}
	//	}
	//}
	//m_GridFault.push_back(TempPoint);//20130918
	//double mindis = 1000000000;//pow(StarA.X - EndA.X,2) + pow(StarA.Y - EndA.Y,2);
	//BOOL Judge = FALSE;
	//if ((int)CrossPoint.size()<1)
	//{
	//	AfxMessageBox("1");
	//	return TRUE;
	//}
	//for (int i = 0 ; i < (int)CrossPoint.size();i++)
	//{
	//	TWOVALUE PointB = CrossPoint[i];
	//	double d = pow(GridIJ[0].X - PointB.X,2) + pow(GridIJ[0].Y - PointB.Y,2);
	//	if (d < mindis)
	//	{
	//		mindis = d;
	//		Point = PointB; //距离首点最近的交点
	//		Judge = CrossMark[i];
	//	}
	//}
	//return Judge;

	/****************************************************/
	//实验新方法：20140724
	//if (GridIJ[0].Z > 1749 && abs(627138.65 - GridIJ[0].X) < 1 && abs(152986.58 - GridIJ[0].Y) < 1)
	//if (abs(GridIJ[0].Z - (-450)) < 1 && abs(625410.48 - GridIJ[0].X) < 1 && abs(150759.19 - GridIJ[0].Y) < 1)
	//if (abs(GridIJ[0].Z - (149)) < 1 && abs(627138.6 - GridIJ[0].X) < 1 && abs(152091.1 - GridIJ[0].Y) < 1)
	//if (abs(GridIJ[0].Z - (-450)) < 1 && abs(625417.48 - GridIJ[0].X) < 1 && abs(150753.19 - GridIJ[0].Y) < 1)
	//if (abs(GridIJ[0].Z - (-50)) < 1 && abs(626533.7 - GridIJ[0].X) < 1 && abs(153259.5 - GridIJ[0].Y) < 1)
	//{
	//	int sk = 0;
	//	AfxMessageBox("2");
	//}
	TempPoint = GetFalutAera(GridIJ, Lines);
	//for (int i = 0; i < TempPoint.size(); i++)
	//	cout << "v " << TempPoint[i].X << " " << TempPoint[i].Y << " " << "0" << endl;
	//20140915，此处之前已经估值了
	//for (int i = 0 ; i < TempPoint.size()  ; i ++)
	//{
	//	THRVALUE TP = TempPoint[i];
	//	TP.Z = 0;
	//	GetValueByK(TP);
	//	TempPoint[i] = TP;
	//}
	/*cout << "v " << GridIJ[0].X << " " << GridIJ[0].Y << " " << "0" << endl;
	cout << "v " << GridIJ[1].X << " " << GridIJ[1].Y << " " << "0" << endl;
	cout << "v " << GridIJ[2].X << " " << GridIJ[2].Y << " " << "0" << endl;
	cout << endl;*/
	TempPoint[0] = GridIJ[0];//保证首点为等值点
	TempPoint[(int)TempPoint.size() - 1] = GridIJ[0];//保证终点为等值点
	//for (int i = 0; i < TempPoint.size(); i++)
	//	cout << "v " << TempPoint[i].X << " " << TempPoint[i].Y << " " << "0" << endl;
	/****************************************************/
	vector<TWOVALUE> CrossPoint;
	vector<BOOL> CrossMark;
	double Z = GridIJ[0].Z;//首点为等值点，当前要追踪的属性值
	//这里需要明确一点，不只是网格点有高程值，之前在计算断层线和网格边的交点时，对每一个交点以及网格内部的断层点都进行了单独的估值
	for (int i = 0; i < (int)TempPoint.size() - 1; i++)	//其实这里不需要不计算第一节和最后一节线段
	{
		double A = TempPoint[i].Z;
		double B = TempPoint[i + 1].Z;
		if ((A - Z)*(B - Z)<0)
		{
			TWOVALUE P;
			//得到等值点的坐标值
			P.X = (Z - A)*(TempPoint[i + 1].X - TempPoint[i].X) / (B - A) + TempPoint[i].X;
			P.Y = (Z - A)*(TempPoint[i + 1].Y - TempPoint[i].Y) / (B - A) + TempPoint[i].Y;
			CrossPoint.push_back(P);
			//判断当前的等值点是否在网格边上
			if (!IsOnGrid(GridIJ, P))
			{
				CrossMark.push_back(FALSE);
			}
			else
			{
				CrossMark.push_back(TRUE);
			}
		}
	}
	m_GridFault.push_back(TempPoint);//20130918
	double mindis = 1000000000;//pow(StarA.X - EndA.X,2) + pow(StarA.Y - EndA.Y,2);
	BOOL Judge = FALSE;
	//这里其实是很考验插值的准确性的，要是插值的效果好，不会出现无等值点的情况
	if ((int)CrossPoint.size()<1)//说明无等值点
	{
		//cout << "no no no" << endl;
		//AfxMessageBox("1");
		return TRUE;
	}
	for (int i = 0; i < (int)CrossPoint.size(); i++)
	{
		TWOVALUE PointB = CrossPoint[i];
		//计算和首点的距离，这里的首点其实就是进入网格边的等值点
		double d = pow(GridIJ[0].X - PointB.X, 2) + pow(GridIJ[0].Y - PointB.Y, 2);
		if (d < mindis)
		{
			mindis = d;
			Point = PointB; //距离首点最近的交点，这里的意思是若是有多个等值点时，选取与首点最近的等值点
			Judge = CrossMark[i];
		}
	}
	return Judge;
}
//得到断层包围的网格区域的最后一个点
void Jie_Ds::GetPtPs(THRVALUE GridIJ[6], THRVALUE Point, int &Index)
{
	for (int i = 0; i < 5; i++)
	{
		THRVALUE Star = GridIJ[i];
		THRVALUE End = GridIJ[i + 1];
		TWOVALUE StarA, EndA, TemPoint;
		StarA.X = Star.X;
		StarA.Y = Star.Y;
		EndA.X = End.X;
		EndA.Y = End.Y;
		TemPoint.X = Point.X;
		TemPoint.Y = Point.Y;
		if (IsinLineK(StarA, EndA, TemPoint))
		{
			Index = i;
			return;
		}
	}
}
//得到比例
void Jie_Ds::GetT(THRVALUE A, THRVALUE B, THRVALUE C, double &t)
{
	double dx = B.X - A.X;
	double dy = B.Y - A.Y;
	if (abs(dx) > abs(dy))
	{
		t = (C.X - A.X) / dx;
	}
	else
	{
		t = (C.Y - A.Y) / dy;
	}
	if (t > 1.0 || t < 0.0)
	{
		int sk = 0;
	}
}
//追踪带断层的右方
void Jie_Ds::TrackRightFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	int index = m_NoFault[i][j];
	GridFL Lines = m_vecGridFL[index];


	TWOVALUE StarA;
	double Mark = abs(Line.Logic[Num - 1].Y - j);
	StarA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i][j + 1].X);
	StarA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i][j + 1].Y);

	THRVALUE Star;
	Star.X = StarA.X;
	Star.Y = StarA.Y;
	Star.Z = Value;
	//使用分块方法  20130917
	TWOVALUE Point;
	THRVALUE GridFault[6];
	GridFault[0] = Star;
	GridFault[1] = m_GridPoint[i][j];
	GridFault[2] = m_GridPoint[i + 1][j];
	GridFault[3] = m_GridPoint[i + 1][j + 1];
	GridFault[4] = m_GridPoint[i][j + 1];
	GridFault[5] = Star;

	if (!GetVirtualValue(GridFault, Lines, Point))
	{
		IsoReal.Logic.push_back(Point);
		return;
	}

	int indexi = 0;
	THRVALUE tempt;
	tempt.X = Point.X;
	tempt.Y = Point.Y;
	tempt.Z = Value;
	GetPtPs(GridFault, tempt, indexi);		//得到交点对应的线段，最后一断层点个位置
	if (indexi == 0)
	{
		//IsoReal.Logic.push_back(Point);
		return;
	}
	if (indexi == 1)
	{
		if (X_Used[i][j] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j], m_GridPoint[i + 1][j], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + t;
		tmpV.Y = j;
		Line.Logic.push_back(tmpV);
		X_Used[i][j] = 1;

		IsoReal.Logic.push_back(Point);

		TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 2)
	{
		if (Y_Used[i + 1][j] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i + 1][j], m_GridPoint[i + 1][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + 1;
		tmpV.Y = j + t;
		Line.Logic.push_back(tmpV);
		Y_Used[i + 1][j] = 1;

		IsoReal.Logic.push_back(Point);
		TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 3)
	{
		if (X_Used[i][j + 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j + 1], m_GridPoint[i + 1][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + t;
		tmpV.Y = j + 1;
		Line.Logic.push_back(tmpV);
		X_Used[i][j + 1] = 1;

		IsoReal.Logic.push_back(Point);

		TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else
	{
		//AfxMessageBox("wrong");
	}
	//TODO:外推法估值	20130913
	//THRVALUE GridIJ[5];
	//GridIJ[0] = m_GridPoint[i][j];
	//GridIJ[1] = m_GridPoint[i+1][j];
	//GridIJ[2] = m_GridPoint[i+1][j+1];
	//GridIJ[3] = m_GridPoint[i][j+1];
	//THRVALUE TempPoint;
	//TempPoint.X = StarA.X;
	//TempPoint.Y = StarA.Y;
	//TempPoint.Z = Value;
	//GetVirtualValue(TempPoint, GridIJ, Lines);

	//double A = GridIJ[0].Z;
	//double B = GridIJ[1].Z;
	//double C = GridIJ[2].Z;
	//double D = GridIJ[3].Z;			//使用外推法得到的虚拟值

	////double A = m_GridPoint[i][j].Z;
	////double B = m_GridPoint[i+1][j].Z;
	////double C = m_GridPoint[i+1][j+1].Z;
	////double D = m_GridPoint[i][j+1].Z;
	//double Z = (A + B + C + D) / 4;

	//if ((A - Value) * (B - Value) < 0 && (D - Value) * (C - Value) < 0)   //追踪三点
	//{
	//	if((A - Value) * (B - Value) < 0 &&((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
	//	{
	//		Mark = abs((Value - A) / (B - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i+1][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i+1][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}
	//		if(X_Used[i][j] == 1)
	//			return;

	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - A) / (A - B)) ;
	//		tmpV.Y = j ;	
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j] = 1;
	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪下边
	//		TrackDown(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//	else if((D - Value) * (C - Value) < 0 && ((D - Value) * (Z - Value) < 0 || ((B - Value) * (Z - Value) < 0)))
	//	{
	//		Mark = abs((Value - D) / (C - D));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j+1].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j+1].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}
	//		if(X_Used[i][j+1] == 1)
	//			return;

	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - D) / (C - D)) ;
	//		tmpV.Y = j + 1;	
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j+1] = 1;
	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪上边
	//		TrackUp(Value,Line,IsoReal,X_Used,Y_Used);

	//	}

	//}
	//else    //追踪一点
	//{
	//	if((A - Value) * (B - Value) < 0 )
	//	{
	//		Mark = abs((Value - A) / (B - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i+1][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i+1][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}
	//		if(X_Used[i][j] == 1)
	//			return;

	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - A) / (A - B)) ;
	//		tmpV.Y = j ;	
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪下边
	//		TrackDown(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//	else if((D - Value) * (C - Value) < 0 )
	//	{
	//		Mark = abs((Value - D) / (C - D));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j+1].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j+1].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}
	//		if(X_Used[i][j+1] == 1)
	//			return;

	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - D) / (C - D)) ;
	//		tmpV.Y = j + 1;	
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j+1] = 1;
	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪上边
	//		TrackUp(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//	else if ((B - Value)*(C- Value) < 0)
	//	{
	//		Mark = abs((Value - B) / (B - C));
	//		EndA.X = (1 - Mark) * m_GridPoint[i+1][j].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i+1][j].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}
	//		if(Y_Used[i+1][j] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + 1;
	//		tmpV.Y = j + abs((Value - B)/(C - B));
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i+1][j] = 1;
	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//向右追踪
	//		TrackRight(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//}
}
//向下追踪，网格内含有断层
void Jie_Ds::TrackDownFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	int index = m_NoFault[i][j - 1];
	GridFL Lines = m_vecGridFL[index];

	TWOVALUE StarA;
	double Mark = abs(Line.Logic[Num - 1].X - i);
	StarA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i + 1][j].X);
	StarA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i + 1][j].Y);
	THRVALUE Star;
	Star.X = StarA.X;
	Star.Y = StarA.Y;
	Star.Z = Value;
	//使用分块方法  20130917
	TWOVALUE Point;
	THRVALUE GridFault[6];
	//
	GridFault[0] = Star;
	GridFault[1] = m_GridPoint[i][j];
	GridFault[2] = m_GridPoint[i][j - 1];
	GridFault[3] = m_GridPoint[i + 1][j - 1];
	GridFault[4] = m_GridPoint[i + 1][j];
	GridFault[5] = Star;
	//这里判断是否追踪到网格内的断层线上，若是追踪到断层线上，终止追踪
	if (!GetVirtualValue(GridFault, Lines, Point))
	{
		IsoReal.Logic.push_back(Point);
		return;
	}
	int indexi = 0;
	THRVALUE tempt;
	tempt.X = Point.X;
	tempt.Y = Point.Y;
	tempt.Z = Value;
	GetPtPs(GridFault, tempt, indexi);		//得到交点对应的线段，最后一断层点个位置
	if (indexi == 0)
	{
		//IsoReal.Logic.push_back(Point);
		return;
	}
	if (indexi == 1)
	{
		if (Y_Used[i][j - 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j], m_GridPoint[i][j - 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i;
		tmpV.Y = j - t;
		Line.Logic.push_back(tmpV);
		Y_Used[i][j - 1] = 1;

		IsoReal.Logic.push_back(Point);

		TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 2)
	{
		if (X_Used[i][j - 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j - 1], m_GridPoint[i + 1][j - 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + t;
		tmpV.Y = j - 1;
		Line.Logic.push_back(tmpV);
		X_Used[i][j - 1] = 1;

		IsoReal.Logic.push_back(Point);
		TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 3)
	{
		if (Y_Used[i + 1][j - 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i + 1][j], m_GridPoint[i + 1][j - 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + 1;
		tmpV.Y = j - t;
		Line.Logic.push_back(tmpV);
		Y_Used[i + 1][j - 1] = 1;

		IsoReal.Logic.push_back(Point);

		TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
	}
	////TODO:外推法估值	20130913
	//THRVALUE GridIJ[5];
	//GridIJ[0] = m_GridPoint[i][j];
	//GridIJ[1] = m_GridPoint[i+1][j];
	//GridIJ[2] = m_GridPoint[i+1][j-1];
	//GridIJ[3] = m_GridPoint[i][j-1];
	//THRVALUE TempPoint;
	//TempPoint.X = StarA.X;
	//TempPoint.Y = StarA.Y;
	//TempPoint.Z = Value;
	//GetVirtualValue(TempPoint, GridIJ, Lines);

	//double A = GridIJ[0].Z;
	//double B = GridIJ[1].Z;
	//double C = GridIJ[2].Z;
	//double D = GridIJ[3].Z;			//使用外推法得到的虚拟值

	////double A = m_GridPoint[i][j].Z;
	////double B = m_GridPoint[i+1][j].Z;
	////double C = m_GridPoint[i+1][j-1].Z;
	////double D = m_GridPoint[i][j-1].Z;
	//double Z = (A + B + C + D) / 4;

	//if ((A - Value) * (D - Value) < 0  && (B - Value) * (C - Value) < 0)  //追踪三点
	//{
	//	if((A - Value) * (D - Value) < 0  && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
	//	{

	//		Mark = abs((Value - D) / (D - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j-1].X + Mark * (m_GridPoint[i][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j-1].Y + Mark * (m_GridPoint[i][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i][j-1] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i;
	//		tmpV.Y = j - abs((Value - A) / (D - A));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i][j-1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪左边
	//		TrackLeft(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//	else if((B - Value) * (C - Value) < 0  && ((B - Value) * (Z - Value) < 0 || ((D - Value) * (Z - Value) < 0)))
	//	{
	//		Mark = abs((Value - C) / (C - B));
	//		EndA.X = (1 - Mark) * m_GridPoint[i+1][j-1].X + Mark * (m_GridPoint[i+1][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i+1][j-1].Y + Mark * (m_GridPoint[i+1][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i+1][j-1] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + 1;
	//		tmpV.Y = j - abs((Value - B) / (C - B));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i+1][j-1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪右边
	//		TrackRight(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//}
	//else   //追踪一点
	//{
	//	if((A - Value) * (D - Value) < 0 )
	//	{

	//		Mark = abs((Value - D) / (D - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j-1].X + Mark * (m_GridPoint[i][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j-1].Y + Mark * (m_GridPoint[i][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i][j-1] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i;
	//		tmpV.Y = j - abs((Value - A) / (D - A));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i][j-1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪左边
	//		TrackLeft(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//	else if((B - Value) * (C - Value) < 0  )
	//	{
	//		Mark = abs((Value - C) / (C - B));
	//		EndA.X = (1 - Mark) * m_GridPoint[i+1][j-1].X + Mark * (m_GridPoint[i+1][j].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i+1][j-1].Y + Mark * (m_GridPoint[i+1][j].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i+1][j-1] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + 1;
	//		tmpV.Y = j - abs((Value - B) / (C - B));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i+1][j-1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪右边
	//		TrackRight(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//	else if ((D - Value)*(C- Value) < 0)
	//	{
	//		Mark = abs((Value - D) / (C - D));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j-1].X + Mark * (m_GridPoint[i+1][j-1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j-1].Y + Mark * (m_GridPoint[i+1][j-1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(X_Used[i][j-1] == 1)
	//		{
	//			return;
	//		}

	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - D)/(C - D));
	//		tmpV.Y = j - 1;
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j-1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//向下追踪
	//		TrackDown(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//}
}
//追踪带断层的下方
void Jie_Ds::TrackLeftFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;

	TWOVALUE StarA;
	double Mark = abs(Line.Logic[Num - 1].Y - j);
	StarA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i][j + 1].X);
	StarA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i][j + 1].Y);
	THRVALUE Star;
	Star.X = StarA.X;
	Star.Y = StarA.Y;
	Star.Z = Value;
	int index = m_NoFault[i - 1][j];
	GridFL Lines = m_vecGridFL[index];
	//使用分块方法  20130917
	TWOVALUE Point;
	THRVALUE GridFault[6];
	GridFault[0] = Star;
	GridFault[1] = m_GridPoint[i][j];
	GridFault[2] = m_GridPoint[i - 1][j];
	GridFault[3] = m_GridPoint[i - 1][j + 1];
	GridFault[4] = m_GridPoint[i][j + 1];
	GridFault[5] = Star;
	if (!GetVirtualValue(GridFault, Lines, Point))
	{
		IsoReal.Logic.push_back(Point);
		return;
	}
	int indexi = 0;
	THRVALUE tempt;
	tempt.X = Point.X;
	tempt.Y = Point.Y;
	tempt.Z = Value;
	//TODO：交点不一定在矩形之间··而是在断层点和网格点之间··，需要根据GetVirtualValue返回的Point进行判断20140724
	GetPtPs(GridFault, tempt, indexi);		//得到交点对应的线段，最后一断层点个位置
	if (indexi == 0)
	{
		//IsoReal.Logic.push_back(Point);
		return;
	}
	if (indexi == 1)
	{
		if (X_Used[i - 1][j] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j], m_GridPoint[i - 1][j], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i - t;
		tmpV.Y = j;
		Line.Logic.push_back(tmpV);
		X_Used[i - 1][j] = 1;

		IsoReal.Logic.push_back(Point);

		TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 2)
	{
		if (Y_Used[i - 1][j] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i - 1][j], m_GridPoint[i - 1][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i - 1;
		tmpV.Y = j + t;
		Line.Logic.push_back(tmpV);
		Y_Used[i - 1][j] = 1;

		IsoReal.Logic.push_back(Point);
		TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 3)
	{
		if (X_Used[i - 1][j + 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j + 1], m_GridPoint[i - 1][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i - t;
		tmpV.Y = j + 1;
		Line.Logic.push_back(tmpV);
		X_Used[i - 1][j + 1] = 1;

		IsoReal.Logic.push_back(Point);

		TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
	}
}
//追踪带断层的上方
void Jie_Ds::TrackUPFault(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	//网格内断层线的线号
	int index = m_NoFault[i][j];
	//由线号得到断层线坐标
	GridFL Lines = m_vecGridFL[index];

	TWOVALUE StarA;
	//此网格横边上的等值点的偏移量
	double Mark = abs(Line.Logic[Num - 1].X - i);
	//在此网格横边上进行线性插值得到等值点
	StarA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i + 1][j].X);
	StarA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i + 1][j].Y);
	THRVALUE Star;
	Star.X = StarA.X;
	Star.Y = StarA.Y;
	Star.Z = Value;

	//使用分块方法  20130917
	TWOVALUE Point;
	THRVALUE GridFault[6];
	GridFault[0] = Star;
	GridFault[1] = m_GridPoint[i][j];
	GridFault[2] = m_GridPoint[i][j + 1];
	GridFault[3] = m_GridPoint[i + 1][j + 1];
	GridFault[4] = m_GridPoint[i + 1][j];
	GridFault[5] = Star;
	//这里的意思是追踪到网格内的断层线上面，终止追踪，不需要再去判断向哪个方向进行追踪
	if (!GetVirtualValue(GridFault, Lines, Point))
	{
		IsoReal.Logic.push_back(Point);
		return;
	}
	int indexi = 0;
	//要追踪的等值点
	THRVALUE tempt;
	tempt.X = Point.X;
	tempt.Y = Point.Y;
	tempt.Z = Value;
	GetPtPs(GridFault, tempt, indexi);		//得到交点对应的线段，最后一断层点个位置
	//这里应该就是等值点在网格边，但是此网格边在断层内部的情况，这里后期还要做处理
	if (indexi == 0)
	{
		//IsoReal.Logic.push_back(Point);
		return;
	}
	if (indexi == 1)
	{
		if (Y_Used[i][j] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j], m_GridPoint[i][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i;
		tmpV.Y = j + t;
		Line.Logic.push_back(tmpV);
		Y_Used[i][j] = 1;

		IsoReal.Logic.push_back(Point);

		TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 2)
	{
		if (X_Used[i][j + 1] == 1)
			return;
		double t = 0;
		GetT(m_GridPoint[i][j + 1], m_GridPoint[i + 1][j + 1], tempt, t);
		TWOVALUE tmpV;
		tmpV.X = i + t;
		tmpV.Y = j + 1;
		Line.Logic.push_back(tmpV);
		X_Used[i][j + 1] = 1;

		IsoReal.Logic.push_back(Point);
		TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
	}
	else if (indexi == 3)
	{
		if (Y_Used[i + 1][j] == 1)
			return;
		double t = 0;
		THRVALUE A = m_GridPoint[i + 1][j];
		THRVALUE B = m_GridPoint[i + 1][j + 1];
		GetT(A, B, tempt, t);
		//GetT(m_GridPoint[i+1][j],m_GridPoint[i+1][j+1],tempt,t);
		TWOVALUE tmpV;
		tmpV.X = i + 1;
		tmpV.Y = j + t;
		Line.Logic.push_back(tmpV);
		Y_Used[i + 1][j] = 1;

		IsoReal.Logic.push_back(Point);

		TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
	}

	////TODO:外推法估值	20130913
	//THRVALUE GridIJ[5];
	//GridIJ[0] = m_GridPoint[i][j];
	//GridIJ[1] = m_GridPoint[i+1][j];
	//GridIJ[2] = m_GridPoint[i+1][j+1];
	//GridIJ[3] = m_GridPoint[i][j+1];
	//THRVALUE TempPoint;
	//TempPoint.X = StarA.X;
	//TempPoint.Y = StarA.Y;
	//TempPoint.Z = Value;
	//GetVirtualValue(TempPoint, GridIJ, Lines);

	//double A = GridIJ[0].Z;
	//double B = GridIJ[1].Z;
	//double C = GridIJ[2].Z;
	//double D = GridIJ[3].Z;			//使用外推法得到的虚拟值

	////double A = m_GridPoint[i][j].Z;
	////double B = m_GridPoint[i+1][j].Z;
	////double C = m_GridPoint[i+1][j+1].Z;
	////double D = m_GridPoint[i][j+1].Z;
	//double Z = (A + B + C + D) / 4;

	//if ((A - Value) * (D - Value) < 0 && (B - Value) * (C - Value) < 0)//追踪三点
	//{
	//	if((A - Value) * (D - Value) < 0  && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
	//	{

	//		Mark = abs((Value - A) / (D - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i][j] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i ;
	//		tmpV.Y = j + abs((Value - A) / (D - A));	
	//		Line.Logic.push_back(tmpV);	
	//		Y_Used[i][j] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪左边
	//		TrackLeft(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//	else if((B - Value) * (C - Value) < 0  && ((B - Value) * (Z - Value) < 0 || ((D - Value) * (Z - Value) < 0)))
	//	{
	//		Mark = abs((Value - B) / (C - B));
	//		EndA.X = (1 - Mark) * m_GridPoint[i+1][j].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i+1][j].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i+1][j] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + 1;
	//		tmpV.Y = j + abs((Value - B) / (C - B));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i+1][j] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪右边
	//		TrackRight(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//}
	//else			//追踪一点
	//{
	//	if((A - Value) * (D - Value) < 0 )
	//	{

	//		Mark = abs((Value - A) / (D - A));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j].X + Mark * (m_GridPoint[i][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j].Y + Mark * (m_GridPoint[i][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i][j] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i ;
	//		tmpV.Y = j + abs((Value - A) / (D - A));	
	//		Line.Logic.push_back(tmpV);	
	//		Y_Used[i][j] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪左边
	//		TrackLeft(Value,Line,IsoReal,X_Used,Y_Used);

	//	}
	//	else if((B - Value) * (C - Value) < 0  )
	//	{
	//		Mark = abs((Value - B) / (C - B));
	//		EndA.X = (1 - Mark) * m_GridPoint[i+1][j].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i+1][j].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(Y_Used[i+1][j] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + 1;
	//		tmpV.Y = j + abs((Value - B) / (C - B));	
	//		Line.Logic.push_back(tmpV);
	//		Y_Used[i+1][j] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//追踪右边
	//		TrackRight(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//	else if ((D - Value)*(C- Value) < 0)
	//	{
	//		Mark = abs((Value - D) / (C - D));
	//		EndA.X = (1 - Mark) * m_GridPoint[i][j+1].X + Mark * (m_GridPoint[i+1][j+1].X);
	//		EndA.Y = (1 - Mark) * m_GridPoint[i][j+1].Y + Mark * (m_GridPoint[i+1][j+1].Y);
	//		//TODO:得到交点，若有交点，则将交点加入到数组中
	//		TWOVALUE Point;
	//		if (GetCrossFault(StarA,EndA,Lines,Point))
	//		{
	//			IsoReal.Logic.push_back(Point);
	//			return;
	//		}

	//		if(X_Used[i][j+1] == 1)
	//			return;
	//		TWOVALUE tmpV;
	//		tmpV.X = i + abs((Value - D)/(C - D));
	//		tmpV.Y = j + 1;
	//		Line.Logic.push_back(tmpV);
	//		X_Used[i][j+1] = 1;

	//		//实际值
	//		TWOVALUE tmpR;
	//		GetReallyPoint(tmpV,tmpR);
	//		IsoReal.Logic.push_back(tmpR);

	//		//向上追踪
	//		TrackUp(Value,Line,IsoReal,X_Used,Y_Used);
	//	}
	//}
}

void Jie_Ds::TrackRight(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	double A = m_GridPoint[i][j].Z;
	double B = m_GridPoint[i + 1][j].Z;
	double C = m_GridPoint[i + 1][j + 1].Z;
	double D = m_GridPoint[i][j + 1].Z;
	double Z = (A + B + C + D) / 4;
	if (m_NoFault[i][j]>-1)					//网格内有断层
	{
		TrackRightFault(Value, Line, IsoReal, X_Used, Y_Used);//追踪网格内
		return;
	}
	//正常追踪
	if ((A - Value) * (B - Value) < 0 && (D - Value) * (C - Value) < 0) //追踪三个等值点
	{
		if (X_Used[i][j] == 0 && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - A) / (A - B));
			tmpV.Y = j;
			Line.Logic.push_back(tmpV);
			X_Used[i][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪下边
			TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (X_Used[i][j + 1] == 0 && ((D - Value) * (Z - Value) < 0 || ((B - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - D) / (C - D));
			tmpV.Y = j + 1;
			Line.Logic.push_back(tmpV);
			X_Used[i][j + 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪上边
			TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		}

	}
	else  //追踪一个等值点
	{
		if ((A - Value) * (B - Value) < 0 && X_Used[i][j] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - A) / (A - B));
			tmpV.Y = j;
			Line.Logic.push_back(tmpV);
			X_Used[i][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪下边
			TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if ((D - Value) * (C - Value) < 0 && X_Used[i][j + 1] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - D) / (C - D));
			tmpV.Y = j + 1;
			Line.Logic.push_back(tmpV);
			X_Used[i][j + 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪上边
			TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (Y_Used[i + 1][j] == 0 && (B - Value)*(C - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + 1;
			tmpV.Y = j + abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i + 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//向右追踪
			TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
}
void Jie_Ds::TrackDown(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	double A = m_GridPoint[i][j].Z;
	double B = m_GridPoint[i + 1][j].Z;
	double C = m_GridPoint[i + 1][j - 1].Z;
	double D = m_GridPoint[i][j - 1].Z;
	double Z = (A + B + C + D) / 4;
	if (m_NoFault[i][j - 1]>-1)					//网格内有断层
	{
		TrackDownFault(Value, Line, IsoReal, X_Used, Y_Used);//追踪网格内
		return;
	}
	//正常追踪
	if ((A - Value) * (D - Value) < 0 && (B - Value) * (C - Value) < 0) //追踪三个等值点
	{
		if (Y_Used[i][j - 1] == 0 && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i;
			tmpV.Y = j - abs((Value - A) / (D - A));//这里相当于tmpV.Y = j -1+ abs((Value - D) / (A - D));因为-1+abs((Value - D) / (A - D))=-abs((Value - A) / (D - A))
			Line.Logic.push_back(tmpV);
			Y_Used[i][j - 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪左边
			TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (Y_Used[i + 1][j - 1] == 0 && ((B - Value) * (Z - Value) < 0 || ((D - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i + 1;
			tmpV.Y = j - abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i + 1][j - 1] = 1;

			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);
			//追踪右边
			TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
	else  //追踪一个等值点
	{
		if ((A - Value) * (D - Value) < 0 && Y_Used[i][j - 1] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i;
			tmpV.Y = j - abs((Value - A) / (D - A));
			Line.Logic.push_back(tmpV);
			Y_Used[i][j - 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪左边
			TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if ((B - Value) * (C - Value) < 0 && Y_Used[i + 1][j - 1] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + 1;
			tmpV.Y = j - abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i + 1][j - 1] = 1;

			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);
			//追踪右边
			TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (X_Used[i][j - 1] == 0 && (D - Value)*(C - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - D) / (C - D));
			tmpV.Y = j - 1;
			Line.Logic.push_back(tmpV);
			X_Used[i][j - 1] = 1;
			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);
			//向下追踪
			TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
}
void Jie_Ds::TrackLeft(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	double A = m_GridPoint[i][j].Z;
	double B = m_GridPoint[i - 1][j].Z;
	double C = m_GridPoint[i - 1][j + 1].Z;
	double D = m_GridPoint[i][j + 1].Z;
	//cout << "v " << m_GridPoint[i][j].X << " " << m_GridPoint[i][j].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i-1][j].X << " " << m_GridPoint[i-1][j].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i-1][j+1].X << " " << m_GridPoint[i-1][j+1].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i][j+1].X << " " << m_GridPoint[i][j+1].Y << " " << "0" << endl;
	double Z = (A + B + C + D) / 4;
	if (m_NoFault[i - 1][j]>-1)					//网格内有断层
	{
		TrackLeftFault(Value, Line, IsoReal, X_Used, Y_Used);//追踪网格内
		return;
	}
	//正常追踪
	if ((A - Value) * (B - Value) < 0 && (D - Value) * (C - Value) < 0) //追踪三个点
	{
		if (X_Used[i - 1][j] == 0 && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i - abs((Value - A) / (B - A));//这里相当于tmpV.X = i -1+ abs((Value - B) / (A - B));因为-1+abs((Value - B) / (A - B))=-abs((Value - A) / (B - A))
			tmpV.Y = j;
			Line.Logic.push_back(tmpV);
			X_Used[i - 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪下边
			TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (X_Used[i - 1][j + 1] == 0 && ((B - Value) * (Z - Value) < 0 || ((D - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i - abs((Value - D) / (C - D));
			tmpV.Y = j + 1;
			Line.Logic.push_back(tmpV);
			X_Used[i - 1][j + 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪上边
			TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
	else    //追踪一个点
	{
		if ((A - Value) * (B - Value) < 0 && X_Used[i - 1][j] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i - abs((Value - A) / (B - A));
			tmpV.Y = j;
			Line.Logic.push_back(tmpV);
			X_Used[i - 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪下边
			TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if ((D - Value) * (C - Value) < 0 && X_Used[i - 1][j + 1] == 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i - abs((Value - D) / (C - D));
			tmpV.Y = j + 1;
			Line.Logic.push_back(tmpV);
			X_Used[i - 1][j + 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪上边
			TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (Y_Used[i - 1][j] == 0 && (B - Value)*(C - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i - 1;
			tmpV.Y = j + abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i - 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//向右追踪
			//TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
			//修改：向左追踪
			TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
}
void Jie_Ds::TrackUp(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{
	int Num = (int)Line.Logic.size();
	//当前最后一个等值点的虚拟坐标
	int i = (int)Line.Logic[Num - 1].X;
	int j = (int)Line.Logic[Num - 1].Y;
	double A = m_GridPoint[i][j].Z;
	double B = m_GridPoint[i + 1][j].Z;
	double C = m_GridPoint[i + 1][j + 1].Z;
	double D = m_GridPoint[i][j + 1].Z;
	//cout << "v " << m_GridPoint[i][j].X << " " << m_GridPoint[i][j].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i+1][j].X << " " << m_GridPoint[i+1][j].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i+1][j+1].X << " " << m_GridPoint[i+1][j+1].Y << " " << "0" << endl;
	//cout << "v " << m_GridPoint[i][j+1].X << " " << m_GridPoint[i][j+1].Y << " " << "0" << endl;
	double Z = (A + B + C + D) / 4;
	if (m_NoFault[i][j]>-1)					//网格内有断层
	{
		TrackUPFault(Value, Line, IsoReal, X_Used, Y_Used);//追踪网格内
		return;
	}

	if ((A - Value) * (D - Value) < 0 && (B - Value) * (C - Value) < 0)//追踪三个等值点
	{
		if (Y_Used[i][j] == 0 && ((A - Value) * (Z - Value) < 0 || ((C - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i;
			tmpV.Y = j + abs((Value - A) / (D - A));
			Line.Logic.push_back(tmpV);
			Y_Used[i][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪左边
			TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (Y_Used[i + 1][j] == 0 && ((B - Value) * (Z - Value) < 0 || ((D - Value) * (Z - Value) < 0)))
		{
			TWOVALUE tmpV;
			tmpV.X = i + 1;
			tmpV.Y = j + abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i + 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪右边
			TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
	else	//只能追踪一个等值点 
	{

		if (Y_Used[i][j] == 0 && (A - Value) * (D - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i;
			tmpV.Y = j + abs((Value - A) / (D - A));
			Line.Logic.push_back(tmpV);
			Y_Used[i][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪左边
			TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (Y_Used[i + 1][j] == 0 && (B - Value) * (C - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + 1;
			tmpV.Y = j + abs((Value - B) / (C - B));
			Line.Logic.push_back(tmpV);
			Y_Used[i + 1][j] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//追踪右边
			TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		}
		else if (X_Used[i][j + 1] == 0 && (D - Value)*(C - Value) < 0)
		{
			TWOVALUE tmpV;
			tmpV.X = i + abs((Value - D) / (C - D));
			tmpV.Y = j + 1;
			Line.Logic.push_back(tmpV);
			X_Used[i][j + 1] = 1;

			//实际值
			TWOVALUE tmpR;
			GetReallyPoint(tmpV, tmpR);
			IsoReal.Logic.push_back(tmpR);

			//向上追踪
			TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		}
	}
}
void Jie_Ds::TrackX(TWOVALUE A, double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{

	int i = (int)A.X;
	int j = (int)A.Y;
	if (j > 1 && j < m_YNum)					//非边界点,处于第一行与最后一行之间
	{
		//虚拟值
		IsoLine LineA;
		IsoLine LineB;
		LineA.Logic.push_back(A);
		LineB.Logic.push_back(A);


		IsoLine RLineA;
		IsoLine RLineB;


		//实际值
		TWOVALUE tmpR;
		//得到真实的等值点的坐标值
		GetReallyPoint(A, tmpR);
		RLineA.Logic.push_back(tmpR);
		RLineB.Logic.push_back(tmpR);
		TrackUp(Value, LineA, RLineA, X_Used, Y_Used);

		int Num = RLineA.Logic.size() - 1;
		if (RLineA.Logic[0].X == RLineA.Logic[Num].X && RLineA.Logic[0].Y == RLineA.Logic[Num].Y)
		{
			IsoReal = RLineA;
			X_Used[i][j] = 1;
			return;
		}
		X_Used[i][j] = 1;
		TrackDown(Value, LineB, RLineB, X_Used, Y_Used);

		//TODO:将两数组联系起来
		int NumA = (int)LineA.Logic.size();
		if (NumA > 1)
		{
			for (int k = NumA - 1; k > 0; k--) //不加入首点
			{
				TWOVALUE PointA = LineA.Logic[k];
				Line.Logic.push_back(PointA);
			}
		}
		for (int k = 0; k < (int)LineB.Logic.size(); k++)
		{
			TWOVALUE PointA = LineB.Logic[k];
			Line.Logic.push_back(PointA);
		}

		//实际坐标相连
		NumA = (int)RLineA.Logic.size();
		if (NumA > 1)
		{
			for (int k = NumA-1; k > 0; k--) //不加入首点
			{
				TWOVALUE PointA = RLineA.Logic[k];
				IsoReal.Logic.push_back(PointA);
			}
		}
		for (int k = 0; k < (int)RLineB.Logic.size(); k++)
		{
			TWOVALUE PointA = RLineB.Logic[k];
			IsoReal.Logic.push_back(PointA);
		}

	}
	else if (j == 1)//第一行
	{

		Line.Logic.push_back(A);
		TWOVALUE tmpR;
		GetReallyPoint(A, tmpR);
		IsoReal.Logic.push_back(tmpR);
		TrackUp(Value, Line, IsoReal, X_Used, Y_Used);
		X_Used[i][j] = 1;
	}
	else if (j == m_YNum)//最后一行
	{

		Line.Logic.push_back(A);
		TWOVALUE tmpR;
		GetReallyPoint(A, tmpR);
		IsoReal.Logic.push_back(tmpR);
		TrackDown(Value, Line, IsoReal, X_Used, Y_Used);
		X_Used[i][j] = 1;
	}
	//X_Used[i][j] = 1;
}
void Jie_Ds::TrackY(TWOVALUE A, double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used)
{

	int i = (int)A.X;
	int j = (int)A.Y;
	if (i > 1 && i < m_XNum)					//非边界点，处于第一列与最后一列之间
	{
		IsoLine LineA;
		IsoLine LineB;
		LineA.Logic.push_back(A);
		LineB.Logic.push_back(A);

		IsoLine RLineA;
		IsoLine RLineB;

		//实际值
		TWOVALUE tmpR;
		GetReallyPoint(A, tmpR);
		RLineA.Logic.push_back(tmpR);
		RLineB.Logic.push_back(tmpR);
		TrackLeft(Value, LineA, RLineA, X_Used, Y_Used);
		int Num = RLineA.Logic.size() - 1;
		if (RLineA.Logic[0].X == RLineA.Logic[Num].X && RLineA.Logic[0].Y == RLineA.Logic[Num].Y)
		{
			IsoReal = RLineA;
			Y_Used[i][j] = 1;
			return;
		}
		Y_Used[i][j] = 1;
		TrackRight(Value, LineB, RLineB, X_Used, Y_Used);

		//虚拟坐标相连
		int NumA = (int)LineA.Logic.size();
		if (NumA > 1)
		{
			for (int k = NumA - 1; k > 0; k--) //不加入首点
			{
				TWOVALUE PointA = LineA.Logic[k];
				Line.Logic.push_back(PointA);
			}
		}
		for (int k = 0; k < (int)LineB.Logic.size(); k++)
		{
			TWOVALUE PointA = LineB.Logic[k];
			Line.Logic.push_back(PointA);
		}
		//实际坐标相连
		NumA = (int)RLineA.Logic.size();
		if (NumA > 1)
		{
			for (int k = NumA - 1; k > 0; k--) //不加入首点
			{
				TWOVALUE PointA = RLineA.Logic[k];
				IsoReal.Logic.push_back(PointA);
			}
		}
		for (int k = 0; k < (int)RLineB.Logic.size(); k++)
		{
			TWOVALUE PointA = RLineB.Logic[k];
			IsoReal.Logic.push_back(PointA);
		}
	}
	else if (i == 1)//第一列
	{
		Line.Logic.push_back(A);
		TWOVALUE tmpR;
		GetReallyPoint(A, tmpR);
		IsoReal.Logic.push_back(tmpR);
		TrackRight(Value, Line, IsoReal, X_Used, Y_Used);
		Y_Used[i][j] = 1;
	}
	else if (i == m_XNum)  //最后一列
	{
		Line.Logic.push_back(A);
		TWOVALUE tmpR;
		GetReallyPoint(A, tmpR);
		IsoReal.Logic.push_back(tmpR);
		TrackLeft(Value, Line, IsoReal, X_Used, Y_Used);
		Y_Used[i][j] = 1;
	}
	//Y_Used[i][j] = 1;
}
//从初始值追踪等值点
void Jie_Ds::TrackPoint(double Value, IsoLine &Line, IsoLine &IsoReal, vector<vector<double>>&X_Used, vector<vector<double>>&Y_Used, TWOVALUE First)
{
	//这里之所以要进行这一步判断，是因为这里相当于一个取整的操作
	//double t = abs((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i][j + 1].Z - m_GridPoint[i][j].Z));
	//double t = abs((Value - m_GridPoint[i][j].Z) / (m_GridPoint[i+1][j].Z - m_GridPoint[i][j].Z));
	//这里的t相当在网格边上线性插值前面的系数，经过验证t在这里一定是正的
	int i = int(First.X);
	int j = (int)First.Y;
	//说明等值点是网格点
	if (i == First.X && j == First.Y)
	{
		//AfxMessageBox("等值点是网格点");			//用于测试
	}

	if (j == First.Y)								//横轴
	{
		if (X_Used[i][j] == 0)						//没有被使用过
		{
			//cout << "hello1" << endl;
			//TODO:追踪横轴
			//X_Used[int(First.X)][j] = 1;			//首点不作判断，方便以后追踪到首点
			TrackX(First, Value, Line, IsoReal, X_Used, Y_Used);	//追踪横边
		}
	}
	else											//纵轴
	{
		if (Y_Used[i][j] == 0)						//没有被使用过
		{
			//cout << i << " " << j << endl;
			//cout << "hello2" << endl;
			//TODO:追踪纵轴
			//Y_Used[int(First.X)][j] = 1;			//首点不作判断，方便以后追踪到首点
			TrackY(First, Value, Line, IsoReal, X_Used, Y_Used);	//追踪纵边
		}
	}
}
void Jie_Ds::TrackOneValue(double Value)
{
	vector<TWOVALUE> VirtualIJ;
	vector<THRVALUE> Jie_VirtualIJK;
	EquivalentPoints(Value, VirtualIJ);//得到所有的等值点
	vector<vector<double>> X_Used;
	vector<vector<double>> Y_Used;
    //给标记赋值,这里要注意这两个数组的遍历方式
	SignBorder(X_Used, Y_Used);						
	//cout << "======================================================" << endl;
	//cout << "以下是关于标记数组的输出信息：" << endl;
	//for (int i = 0; i < X_Used.size(); i++)
	//{
	//	for (int j = 0; j < X_Used[i].size(); j++)
	//	{
	//		cout << X_Used[i][j] << " ";
	//		//flag_x[i][j] = X_Used[i][j];
	//	}
	//	cout << endl;
	//}
	//cout << endl;
	//for (int i = 0; i < Y_Used.size(); i++)
	//{
	//	for (int j = 0; j < Y_Used[i].size(); j++)
	//	{
	//		cout << Y_Used[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	//cout << "======================================================" << endl;													
	//等值线追踪
	//cout << (int)VirtualIJ.size() << " ";
	for (int i = 0; i < (int)VirtualIJ.size(); i++)
	{
		IsoLine ISOLineOne;	//存储虚拟坐标
		IsoLine IsoReal;	//存储实际坐标
		TWOVALUE First = VirtualIJ[i];
		TrackPoint(Value, ISOLineOne, IsoReal, X_Used, Y_Used, First);
		if (IsoReal.Logic.size() > 30)
		{
			ISOLineOne.Value = Value;
			IsoReal.Value = Value;
			m_IsoLine.push_back(ISOLineOne);
			m_IsoRealLine.push_back(IsoReal); //20131008测试断层内部的网格线
		}
	}
	/*cout << "======================================================" << endl;
	cout << "以下是执行后关于标记数组的输出信息：" << endl;
	for (int i = 0; i < X_Used.size(); i++)
	{
		for (int j = 0; j < X_Used[i].size(); j++)
		{
			cout <<  fixed << setprecision(0)<< X_Used[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;
	for (int i = 0; i < Y_Used.size(); i++)
	{
		for (int j = 0; j < Y_Used[i].size(); j++)
		{
			cout << fixed << setprecision(0) << Y_Used[i][j] << " ";
		}
		cout << endl;
	}
	cout << "======================================================" << endl;*/
	
}
void Jie_Ds::IsolineTracking()
{

	//在此处清空等值线上数据
	m_IsoLine.clear();
	m_IsoRealLine.clear();	//20131111
	for (int i = 0; i < (int)m_TrackValue.size(); i++)
	{
		double Value = m_TrackValue[i];
		if (Value > m_ZMax || Value < m_ZMin)
		{
			continue;
		}
		else
		{
			TrackOneValue(Value);
		}
		std::ofstream out300("current_isoline.obj");
		for (int i = 0; i < m_IsoRealLine.size(); i++)
		{
			for (int j = 0; j < m_IsoRealLine[i].Logic.size(); j++)
				out300 << "v " << fixed << setprecision(5) << m_IsoRealLine[i].Logic[j].X << " " << fixed << setprecision(5) << m_IsoRealLine[i].Logic[j].Y << " " << "0" << endl;
		}
	}
}
void Jie_Ds::IsolineOpt()
{
	for (int i = 0; i < m_IsoRealLine.size(); i++)
	{
		originPoint.clear();
		curvePoint.clear();
		Vector2D p;
		for (int j = 0; j < m_IsoRealLine[i].Logic.size(); j++)
		{
			p.x = m_IsoRealLine[i].Logic[j].X;
			p.y = m_IsoRealLine[i].Logic[j].Y;
			originPoint.push_back(p);
		}
		createCurve();
		IsoLine iso;
		for (int j = 0; j < curvePoint.size(); j++)
		{	
			TWOVALUE t;
			t.X = curvePoint[j].x;
			t.Y = curvePoint[j].y;
			iso.Logic.push_back(t);
		}
		opt_IsoRealLine.push_back(iso);
		std::ofstream out("data//isoline_current.obj");
		for (int k = 0; k < iso.Logic.size(); k++)
			out << "v " << fixed << setprecision(10) << iso.Logic[k].X << " " << fixed << setprecision(10) << iso.Logic[k].Y << " " << "0" << endl;
		out.close();
	}
}
void Jie_Ds::CreateIsoLine()
{
	if (m_ZMax != m_ZMin)
	{
		vector<HwIsoLine> vec;
		m_lsoLines.swap(vec);

		int iso_size = int(m_IsoLine.size());


		for (int i = 0; i < iso_size; i++)
		{
			IsoLine il = m_IsoLine[i];

			HwIsoLine line;
			line.zValue = il.Value;
			line.ptn = il.Logic.size();
			line.pts = new HwIsoPt[line.ptn];

			for (int k = 0; k < il.Logic.size(); k++)
			{
				HwIsoPt hpt;
				hpt.x = il.Logic[k].X;
				hpt.y = il.Logic[k].Y;

				line.pts[k].x = hpt.x;
				line.pts[k].y = hpt.y;
			}

			m_lsoLines.push_back(line);
		}

	}

}



void Jie_Ds::SetOriBoder(vector<TWOVALUE> OriBoder)
{
	Jie_OriBoder.clear();
	for (int i = 0; i < (int)OriBoder.size(); i++)
	{
		Jie_OriBoder.push_back(OriBoder[i]);
	}
}
void Jie_Ds::SetOriISOLine(vector<IsoLine> IsoRealLine)
{
	//for (int i = 0; i < IsoRealLine.size(); i++)
	//	cout << IsoRealLine[i].Logic[0].X << " ";





	//IsoLine L;
	//Jie_IsoRealLine.clear();
	for (int i = 0; i < (int)IsoRealLine.size(); i++)
	{
		/*L.direction0 = IsoRealLine[i].direction0;
		L.direction1 = IsoRealLine[i].direction1;
		L.Index = IsoRealLine[i].Index;
		cout << IsoRealLine[i].Logic.size();
		for (int j = 0; j < IsoRealLine[i].Logic.size(); j++)
		{
			cout << IsoRealLine[i].Logic.size();
			cout << IsoRealLine[i].Logic[j].X << " ";
		}
			
		
		L.Value = IsoRealLine[i].Value;
		cout << "value = ;" << L.Value << endl;*/
		Jie_IsoRealLine.push_back(IsoRealLine[i]);
	}
}
bool Jie_Ds::IsinLineK(TWOVALUE Star, TWOVALUE End, TWOVALUE Point)
{
	End.X = End.X - Star.X;
	End.Y = End.Y - Star.Y;
	Point.X = Point.X - Star.X;
	Point.Y = Point.Y - Star.Y;
	Star.X = 0;
	Star.Y = 0;					//平移，对判断是否一条直线不造成影响··将大数化为小数~能提高计算精度
	if (Point.X *(Point.X - End.X) >0 && Point.Y *(Point.Y - End.Y) > 0)
	{
		return false;
	}
	double A = Star.Y - End.Y;
	double B = End.X - Star.X;
	//double C = -A * Star.X- B * Star.Y;
	double C = 0;
	double D = sqrt(pow(A, 2) + pow(B, 2));
	double d = A * (Point.X - Star.X) + B * (Point.Y - Star.Y);
	d = d / D;
	if (abs(d)<0.00001)
	{
		return true;
	}
	return false;

}
//线段是否相交,index虚拟序号
bool Jie_Ds::ISIntersect(TWOVALUE p1_s, TWOVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e, double &index)
{
	double Xmax_1, Xmax_2, Xmin_1, Xmin_2, Ymax_1, Ymax_2, Ymin_1, Ymin_2;
	double V1, V2, V3, V4;

	if (p1_s.X > p1_e.X)
	{
		Xmax_1 = p1_s.X;
		Xmin_1 = p1_e.X;
	}
	else
	{
		Xmax_1 = p1_e.X;
		Xmin_1 = p1_s.X;
	}


	if (p1_s.Y > p1_e.Y)
	{
		Ymax_1 = p1_s.Y;
		Ymin_1 = p1_e.Y;
	}
	else
	{
		Ymax_1 = p1_e.Y;
		Ymin_1 = p1_s.Y;
	}


	if (p2_s.X > p2_e.X)
	{
		Xmax_2 = p2_s.X;
		Xmin_2 = p2_e.X;
	}
	else
	{
		Xmax_2 = p2_e.X;
		Xmin_2 = p2_s.X;
	}

	if (p2_s.Y > p2_e.Y)
	{
		Ymax_2 = p2_s.Y;
		Ymin_2 = p2_e.Y;
	}
	else
	{
		Ymax_2 = p2_e.Y;
		Ymin_2 = p2_s.Y;
	}


	if (Xmax_1 < Xmin_2 || Xmin_1 > Xmax_2 || Ymin_1 > Ymax_2 || Ymax_1 < Ymin_2)   //两线段最小矩形不相交，得出两线段不相交
		return FALSE;
	else
	{
		V1 = (p1_e.X - p1_s.X) * (p2_s.Y - p1_s.Y) - (p2_s.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V2 = (p1_e.X - p1_s.X) * (p2_e.Y - p1_s.Y) - (p2_e.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V3 = (p2_e.X - p2_s.X) * (p1_s.Y - p2_s.Y) - (p1_s.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		V4 = (p2_e.X - p2_s.X) * (p1_e.Y - p2_s.Y) - (p1_e.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		//特殊情况点在直线上时候   20131015
		BOOL Mark = FALSE;
		TWOVALUE TempPoint;
		if (abs(V1)< 0.00000001)
		{
			TempPoint = p2_s;
			Mark = TRUE;
		}
		else if (abs(V2)< 0.00000001)
		{
			TempPoint = p2_e;
			Mark = TRUE;
		}
		else if (abs(V3)< 0.00000001)
		{
			TempPoint = p1_s;
			Mark = TRUE;
		}
		else if (abs(V4)< 0.00000001)
		{
			TempPoint = p1_e;
			Mark = TRUE;
		}
		if (Mark)
		{
			double t = 0;
			if (abs(TempPoint.X - p1_s.X)<abs(TempPoint.Y - p1_s.Y))
			{
				t = (TempPoint.Y - p1_s.Y) / (p1_e.Y - p1_s.Y);/////////////////////
			}
			else
			{
				t = (TempPoint.X - p1_s.X) / (p1_e.X - p1_s.X);
			}
			index = index + t;
			return TRUE;
		}
		if (V3 * V4 <= 0 && V1 * V2 <= 0)
		{
			double dy = p1_e.Y - p1_s.Y;
			double dx = p1_e.X - p1_s.X;
			double t = 0;
			if (abs(p2_e.X - p2_s.X)<0.00000000001)
			{
				t = (p2_e.X - p1_s.X) / dx;/////////////////////
			}
			else
			{
				double k = (p2_s.Y - p2_e.Y) / (p2_s.X - p2_e.X);
				t = ((p2_s.Y - p1_s.Y) - k*p2_s.X + k*p1_s.X) / (dy - k*dx);
			}
			index = index + t;
			return TRUE;
		}
		else
			return FALSE;
	}
}
//点A是否在Line内部
bool Jie_Ds::IsInside(TWOVALUE A, vector<TWOVALUE> Line)
{
	//cout << "hhh" << endl;
	for (int i = 0; i < (int)Line.size() - 1; i++)
	{
		if (IsinLineK(Line[i], Line[i + 1], A))		//点在边界上  20131023
		{
			return TRUE;
		}
	}
	double Xmax = Line[0].X;
	double Xmin = Line[0].X;
	double Ymax = Line[0].Y;
	double Ymin = Line[0].Y;
	for (int i = 1; i < (int)Line.size(); i++)
	{
		if (Line[i].X > Xmax)
		{
			Xmax = Line[i].X;
		}
		else if (Line[i].X < Xmin)
		{
			Xmin = Line[i].X;
		}

		if (Line[i].Y > Ymax)
		{
			Ymax = Line[i].Y;
		}
		else if (Line[i].Y < Ymin)
		{
			Ymin = Line[i].Y;
		}
	}
	if (A.X > Xmax || A.X <Xmin || A.Y>Ymax || A.Y < Ymin)
	{
		return FALSE;
	}
	//射线判断是否在内部
	TWOVALUE B = A;
	B.X = m_XMin - 200.0;     //AB射线
	int Sum = 0;
	for (int i = 0; i < (int)Line.size() - 1; i++)
	{
		double t = 0;
		TWOVALUE LineA = Line[i];
		TWOVALUE LineB = Line[i + 1];
		if (ISIntersect(LineA, LineB, A, B, t))
		{
			Sum = Sum + 1;
		}
	}
	if (Sum % 2 == 1)//交点为奇数个则在内部
	{
		return TRUE;
	}
	return FALSE;
}
bool Jie_Ds::L2L_Intersect(TWOVALUE p1_s, TWOVALUE p1_e, TWOVALUE p2_s, TWOVALUE p2_e, TWOVALUE &Point)
{
	double Xmax_1, Xmax_2, Xmin_1, Xmin_2, Ymax_1, Ymax_2, Ymin_1, Ymin_2;
	double V1, V2, V3, V4;

	if (p1_s.X > p1_e.X)
	{
		Xmax_1 = p1_s.X;
		Xmin_1 = p1_e.X;
	}
	else
	{
		Xmax_1 = p1_e.X;
		Xmin_1 = p1_s.X;
	}


	if (p1_s.Y > p1_e.Y)
	{
		Ymax_1 = p1_s.Y;
		Ymin_1 = p1_e.Y;
	}
	else
	{
		Ymax_1 = p1_e.Y;
		Ymin_1 = p1_s.Y;
	}


	if (p2_s.X > p2_e.X)
	{
		Xmax_2 = p2_s.X;
		Xmin_2 = p2_e.X;
	}
	else
	{
		Xmax_2 = p2_e.X;
		Xmin_2 = p2_s.X;
	}

	if (p2_s.Y > p2_e.Y)
	{
		Ymax_2 = p2_s.Y;
		Ymin_2 = p2_e.Y;
	}
	else
	{
		Ymax_2 = p2_e.Y;
		Ymin_2 = p2_s.Y;
	}


	if (Xmax_1 < Xmin_2 || Xmin_1 > Xmax_2 || Ymin_1 > Ymax_2 || Ymax_1 < Ymin_2)   //两线段最小矩形不相交，得出两线段不相交
		return false;
	else   //利用向量的叉积性质，当其中一条线段的两个端点在另一条线段的同一侧时，不相交。否则，相交。
		   //( Q1 - P1 )×( P2 - P1) * ( P2 - P1)×(Q2 - P1) >= 0。
		   //叉积的计算公式为:  P1 X P2 = x1y2 - x2y1;
	{
		V1 = (p1_e.X - p1_s.X) * (p2_s.Y - p1_s.Y) - (p2_s.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V2 = (p1_e.X - p1_s.X) * (p2_e.Y - p1_s.Y) - (p2_e.X - p1_s.X) * (p1_e.Y - p1_s.Y);
		V3 = (p2_e.X - p2_s.X) * (p1_s.Y - p2_s.Y) - (p1_s.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		V4 = (p2_e.X - p2_s.X) * (p1_e.Y - p2_s.Y) - (p1_e.X - p2_s.X) * (p2_e.Y - p2_s.Y);
		//特殊情况点在直线上时候   20131015
		if (abs(V1)< 0.00000001)
		{
			Point = p2_s;
			return true;
		}
		else if (abs(V2)< 0.00000001)
		{
			Point = p2_e;
			return true;
		}
		else if (abs(V3)< 0.00000001)
		{
			Point = p1_s;
			return true;
		}
		else if (abs(V4)< 0.00000001)
		{
			Point = p1_e;
			return true;
		}

		else if (V3 * V4 <= 0 && V1 * V2 <= 0)
		{
			double dy = p1_e.Y - p1_s.Y;
			double dx = p1_e.X - p1_s.X;
			double t = 0;
			if (abs(p2_e.X - p2_s.X)<0.00000000001)
			{
				t = (p2_e.X - p1_s.X) / dx;/////////////////////
				Point.X = p2_s.X;
				Point.Y = t*dy + p1_s.Y;
			}
			else
			{
				double k = (p2_s.Y - p2_e.Y) / (p2_s.X - p2_e.X);
				t = ((p2_s.Y - p1_s.Y) - k*p2_s.X + k*p1_s.X) / (dy - k*dx);
				Point.X = t * dx + p1_s.X;
				Point.Y = t * dy + p1_s.Y;
			}
			return true;
		}
	}
	return false;

}
bool Jie_Ds::GetCrossPt(TWOVALUE Star, TWOVALUE End, vector<TWOVALUE> Line, TWOVALUE &A)
{
	for (int j = 0; j < (int)Jie_OriBoder.size() - 1; j++)
	{
		TWOVALUE C = Jie_OriBoder[j];
		TWOVALUE B = Jie_OriBoder[j + 1];
		if (L2L_Intersect(C, B, Star, End, A))
		{
			return TRUE;
		}
	}
	return FALSE;
}
void Jie_Ds::DleaIso(IsoLine &OneIso, vector<IsoLine> &NewIso)
{
	IsoLine TempLine;
	TempLine.Value = OneIso.Value;
	//cout << "Jie_OriBorder size is :" << Jie_OriBoder.size() << endl;
	//cout << "当前的坐标：" << OneIso.Logic[0].X << " " << OneIso.Logic[0].X << endl;
	if (OneIso.Logic.size() > 0)
	{
		bool flag = IsInside(OneIso.Logic[0], Jie_OriBoder);
		if (!flag)//首点不在原始边界内，删除以前的点，并加入边界点
		{
			for (int i = 1; i < (int)OneIso.Logic.size(); i++)
			{
				if (IsInside(OneIso.Logic[i], Jie_OriBoder))
				{
					TWOVALUE Star = OneIso.Logic[i - 1];
					TWOVALUE End = OneIso.Logic[i];
					TWOVALUE A;
					if (GetCrossPt(Star, End, Jie_OriBoder, A))//得到边界和这两个点构成的线段的交点
					{
						TempLine.Logic.push_back(A);//此交点作为等值线的其中的一点
					}
					else
					{
						int sk = 0;
						//AfxMessageBox("111");
					}
					for (int j = i; j < (int)OneIso.Logic.size(); j++)
					{
						TempLine.Logic.push_back(OneIso.Logic[j]);//插入坐标
					}
					break;
				}
			}
		}
		else
		{
			TempLine = OneIso;
		}
		if ((int)TempLine.Logic.size() <= 1)	//点全部在原始边界外面
		{
			return;
		}
		IsoLine StarLine;
		StarLine.Value = TempLine.Value;
		StarLine.Logic.push_back(TempLine.Logic[0]);
		OneIso.Logic.clear();
		for (int i = 1; i < (int)TempLine.Logic.size(); i++)  //从第二个点开始插入坐标
		{
			if (IsInside(TempLine.Logic[i], Jie_OriBoder))
			{
				StarLine.Logic.push_back(TempLine.Logic[i]);
			}
			else
			{
				//CString str;
				//str.Format("首个边界点%d,%f     %f",i,TempLine.Logic[0].X,TempLine.Logic[0].Y);
				//AfxMessageBox(str);

				TWOVALUE Star = TempLine.Logic[i - 1];
				TWOVALUE End = TempLine.Logic[i];
				TWOVALUE A;
				if (GetCrossPt(Star, End, Jie_OriBoder, A))
				{
					StarLine.Logic.push_back(A);
				}
				else
				{
					int sk = 0;
					//AfxMessageBox("111");
				}

				for (int j = i; j < (int)TempLine.Logic.size(); j++)
				{
					OneIso.Logic.push_back(TempLine.Logic[j]);
				}
				break;
			}
		}
		NewIso.push_back(StarLine);
		if (OneIso.Logic.size() > 0)//如果里面还有数据，说明等值线中间部分也被边界给切分了
		{
			DleaIso(OneIso, NewIso);
		}
	}
}

void Jie_Ds::DleaIso()
{

	vector<IsoLine> NewIso;
	for (int i = 0; i < (int)Jie_IsoRealLine.size(); i++)
	{
		vector<IsoLine> TempLines;
		IsoLine LineOne = Jie_IsoRealLine[i];
	
		DleaIso(LineOne, TempLines);
		for (int j = 0; j < (int)TempLines.size(); j++)
		{
			NewIso.push_back(TempLines[j]);
		}
	}

	Jie_IsoRealLine.clear();
	Jie_IsoRealLine = NewIso;//被边界切割后的等值线
}
vector<double> Jie_Ds::Randnum(int k)
{
	vector<double> temp;
	srand((int)time(0));
	for (int i = 1; i <= k / 2; i++)
	{
		double temp_ = random(10);
		temp.push_back(temp_);
	}
	return temp;
}