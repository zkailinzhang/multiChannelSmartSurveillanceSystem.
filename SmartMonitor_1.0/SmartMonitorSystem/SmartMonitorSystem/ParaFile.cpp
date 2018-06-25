#include "ParaFile.h"
#include <opencv2/core/core.hpp>
#include <fstream>

using namespace cv;
using namespace std;

extern string para_T1 ;
extern string para_T2;
extern string para_T3;
extern string para_T4;

extern string para_M1;
extern string para_M2;
extern string para_M3;
extern string para_M4;

bool fileIsNone(string filename)
{
	fstream _file;  
	_file.open(filename,ios:: in );  
	if ( ! _file)  
	{  
		return true; 
	}  
	else   
	{  
		return false; 
	}  
}

void InitParaFile(string filename)
{
	string s = "OK!";
	char type;
	if (filename == para_T1)
	{
		type = 0x01;
	}
	else if(filename == para_T2)
	{
		type = 0x02;
	}
	else if(filename == para_T3)
	{
		type = 0x03;
	}
	else if(filename == para_T4)
	{
		type = 0x04;
	}
	else if(filename == para_M1)
	{
		type = 0x11;
	}
	else if(filename == para_M2)
	{
		type = 0x12;
	}
	else if(filename == para_M3)
	{
		type = 0x013;
	}
	else if(filename == para_M4)
	{
		type = 0x14;
	}
	FileStorage fs(filename,FileStorage::WRITE);
	fs <<"Initiate" <<s;
	fs <<"NumandType" << type;
	fs.release();
}
void prepForModifyData(string filename,string startwords,string endwords)
{

	FileStorage fs1(filename,FileStorage::READ);
	FileNode n = fs1[startwords];
	int type =0;
	if (n.isNone())
	{
		type = 1;
	}
	else
	{
		type = 2;
	}
	fs1.release();

	if (type ==1)
	{
		return;
	}
	else
	{
		fstream ff(filename);
		ostringstream os;
		os << ff.rdbuf();
		ff.close();

		string content(os.str());
		string::size_type pos1 = content.find(startwords);
		string::size_type pos2 = content.find(endwords);
		string appsymbol = "...";
		
		if (appsymbol == content.substr(pos1-8,3))
		{
			content = content.erase(pos1-8,pos2+endwords.length()+2-pos1+8+2);
		}
		else
		{
			content = content.erase(pos1,pos2+endwords.length()+2-pos1+2);
		}

		fstream ff1(filename,std::ios::out);
		ff1.close();

		fstream ff2(filename,std::ios::app);
		ff2 << content;
		ff2.close();
		return;
	}


}
