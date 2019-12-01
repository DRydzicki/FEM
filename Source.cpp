// funkcja generacji elementów i wezlow
// wypelnienie H local 

#include<iostream>
#include<vector>
#include<fstream>
#include<string>
#include<conio.h>

using namespace std;

struct node{
	int BC;
};

struct element{
	double K;
	double S;
	double L;
	int ID[2];
	double H[2][2];
	double Hbc[2][2];
	double P[2];
};


struct GlobalData {
	double K, S, L, alpha, tinf, q;
	int MN;
	int ME; 
	double dL;
	double** Hg;
	double** Pg;
	double** tg;
};


void AssignFlags(node*,int);
GlobalData GetData(string);
element* buildElements(GlobalData, node*);
void Calculate(element*,int);

int main() {
	GlobalData Data;
	Data = GetData("Data.txt");
	//cout << Data.ME;
	node *N=new node[Data.ME];
	
	AssignFlags(N, Data.ME);
	element* Elements;
	Elements = buildElements(Data,N);
	Calculate(Elements, Data.ME);
	
}


void AssignFlags(node *N,int NumofEl) {
	for (int i = 0; i < NumofEl; i++) {
		if (i == 0)
			N[i].BC = 1;
		else if (i == NumofEl -1 )
			N[i].BC = 2;
		else
			N[i].BC = 0;
	}
}

GlobalData GetData(string filename) {
	fstream file;
	file.open(filename);
	GlobalData Data;
	if (file.good()) {
		string temp;
		getline(file, temp);
		Data.K = stod(temp);	
		getline(file, temp);
		Data.S = stod(temp);
		getline(file, temp);
		Data.L= stod(temp);
		getline(file, temp);
		Data.alpha = stod(temp);
		getline(file, temp);
		Data.tinf = stod(temp);
		getline(file, temp);
		Data.q = stod(temp);
		getline(file, temp);
		Data.MN = stoi(temp);
		file.close();

		Data.ME = Data.MN - 1;
		Data.dL = Data.L / Data.ME;
		Data.Hg = new double* [Data.MN];
		for (int i = 0; i < Data.MN; i++)
			Data.Hg[i] = new double[Data.MN];
		Data.Pg = new double* [Data.MN];
		for (int i = 0; i < Data.MN; i++)
			Data.Pg[i] = new double[Data.MN];
		Data.tg = new double* [Data.MN];
		for (int i = 0; i < Data.MN; i++)
			Data.tg[i] = new double[Data.MN];	
	}
	return Data;
}

element* buildElements(GlobalData Data,node* N) {
	element* Elements = new element[Data.ME];
	for (int i = 0; i < Data.ME; i++) {
		Elements[i].K = Data.K;
		Elements[i].S = Data.S;
		Elements[i].L = Data.dL;
		double C = Elements[i].S * Elements[i].K / Elements[i].L;
		Elements[i].H[0][0] = C;
		Elements[i].H[0][1] = -C;
		Elements[i].H[1][0] = -C;
		Elements[i].H[1][1] = C;
		Elements[i].ID[0]=i;
		Elements[i].ID[1] = i + 1;
		
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				Elements[i].Hbc[j][k] = 0;
			}
		}

		for (int j = 0; j < 2; j++)
			Elements[i].P[j] = 0;

		if (N[Elements[i].ID[0]].BC == 1 || N[Elements[i].ID[1]].BC == 1)
			Elements[i].Hbc[0][0] = Data.alpha*Elements[i].S;
		else if (N[Elements[i].ID[0]].BC == 2 || N[Elements[i].ID[1]].BC == 2)
			Elements[i].Hbc[1][1] = Data.alpha * Elements[i].S;

		if (N[Elements[i].ID[0]].BC == 1 || N[Elements[i].ID[1]].BC == 1)
			Elements[i].P[0] = Data.q*Elements[i].S;
		else if (N[Elements[i].ID[0]].BC == 2 || N[Elements[i].ID[1]].BC == 2)
			Elements[i].P[1] = -Data.alpha * Data.tinf * Elements[i].S;

	}

	return Elements;
}

void Calculate(element* Elements,int ME) {
	
	vector<vector<double>> HG;
	HG.resize(ME, vector<double>(ME, 0));

	

	for (int i = 0; i < ME-1; i++) {	
		HG[i][i] += Elements[i].H[0][0] + Elements[i].Hbc[0][0];
		HG[i][i+1] += Elements[i].H[0][1] + Elements[i].Hbc[0][1];
		HG[i+1][i] += Elements[i].H[1][0] + Elements[i].Hbc[1][0];
		HG[i+1][i+1] += Elements[i].H[1][1] + Elements[i].Hbc[1][1];	
	}



	for (int i = 0; i < HG.size(); i++)
	{
		for (int j = 0; j < HG.size(); j++) {
			cout << HG[i][j] << " ";
		}
		cout << endl;
	}



}