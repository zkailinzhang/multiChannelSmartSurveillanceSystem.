#ifndef _PARA_FILE_H_
#define _PARA_FILE_H_

#include <string>

using namespace std;
//��������·��


void prepForModifyData(string filename,string startwords,string endwords);
bool fileIsNone(string filename);
void InitParaFile(string filename);

#endif