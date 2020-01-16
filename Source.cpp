#include<iostream>
#include<fstream>
#include<string>


using namespace std;


struct element{
	int BC;
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
	double** Hg;
	double* Pg;
	double* Tg; 
	int** element;
};


GlobalData GetData(string);
element* buildElements(GlobalData);
void Calculate(element*, GlobalData);

int main() {
	GlobalData Data;
	Data = GetData("Data.txt");

	element* Elements;
	Elements = buildElements(Data);

	Calculate(Elements, Data);
	
	Elements = NULL;
	delete Elements;
}

//BUDOWA PLIKU:
//K
//S
//L
//alpha
//temp otoczenia
//q
//ilosc wez³ów
//[element N](id wêz³a 1) (id wêz³a 2) (BC warunki brzegowe)
//ilosc elementow musi byc mniejsza o 1 niz ilosc wezlow

//odczytanie danych z pliku
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

		//ilosc elementow
		Data.ME = Data.MN - 1;
		Data.element = new int* [Data.ME];

		//tworzenie 'podstawy' pod element
		//wpisanie id wez³ów oraz warunek brzegowy
		for (int i = 0; i < Data.ME; i++)
			Data.element[i] = new int[3];
		for (int i = 0; i < Data.ME; i++)
			for (int j = 0; j < 3; j++)
				file >> Data.element[i][j];
		file.close();

		//ustalanie wielkosci macierzy glownych
		Data.Hg = new double* [Data.MN];
		for (int i = 0; i < Data.MN; i++)
			Data.Hg[i] = new double[Data.MN];
		Data.Pg = new double[Data.MN];
		Data.Tg = new double[Data.MN];
	}
	return Data;
}

//tworzenie tablicy elementow
element* buildElements(GlobalData Data) {
	element* Elements = new element[Data.ME];
	bool flagCheck = true;
	for (int i = 0; i < Data.ME; i++) {
		Elements[i].L = Data.L / Data.ME;  //dlugosc pojedynczego elementu. 
		
		double C = Data.S * Data.K / Elements[i].L;
		Elements[i].H[0][0] = C;
		Elements[i].H[0][1] = -C;
		Elements[i].H[1][0] = -C;
		Elements[i].H[1][1] = C;
		//id wez³ów oraz bc z pliku
		Elements[i].ID[0] = Data.element[i][0];
		Elements[i].ID[1] = Data.element[i][1];
		Elements[i].BC = Data.element[i][2];
		
		//zerowanie macierzy hbc oraz P
		for (int j = 0; j < 2; j++) {
			for (int k = 0; k < 2; k++) {
				Elements[i].Hbc[j][k] = 0;
			}
		}
		for (int j = 0; j < 2; j++)
			Elements[i].P[j] = 0;


		//uzupelnianie macierzy hbc oraz P warunkami brzegowymi
		//flagCheck weryfikuje warunki brzegowe w pierwszym wezle 
		if (flagCheck && Elements[i].BC == 1) {
			Elements[i].P[0] = Data.q * Data.S;
			flagCheck = false;
		}
		else if (flagCheck && Elements[i].BC == 2) {
			Elements[i].P[0] = -1 * Data.alpha * Data.S * Data.tinf;
			Elements[i].Hbc[0][0] = Data.alpha * Data.S;
			flagCheck = false;
		}
		else if (Elements[i].BC == 1) {
			Elements[i].P[1] = Data.q * Data.S;
		}
		else if (Elements[i].BC == 2) {
			Elements[i].P[1] = -1 * Data.alpha * Data.S * Data.tinf;
			Elements[i].Hbc[1][1] = Data.alpha * Data.S;
		}
	}

	return Elements;
}

void Calculate(element* Elements,GlobalData Data) {
	
	
	//ZEROWANIE macierzy g³ównych
	for (int i = 0; i < Data.MN; i++)
		for (int j = 0; j < Data.MN; j++)
			Data.Hg[i][j] = 0;
	for (int i = 0; i < Data.MN; i++)
		Data.Pg[i] = 0;
	for (int i = 0; i < Data.MN; i++)
		Data.Tg[i] = 0;


	for (int i = 0; i < Data.MN-1; i++) {	
		Data.Hg[i][i] += Elements[i].H[0][0] + Elements[i].Hbc[0][0];
		Data.Hg[i][i+1] += Elements[i].H[0][1] + Elements[i].Hbc[0][1];
		Data.Hg[i+1][i] += Elements[i].H[1][0] + Elements[i].Hbc[1][0];
		Data.Hg[i+1][i+1] += Elements[i].H[1][1] + Elements[i].Hbc[1][1];
		Data.Pg[i] += Elements[i].P[0]; 
		Data.Pg[i+1] += Elements[i].P[1];
	}

	cout << "MACIERZ H: \n";
	for (int i = 0; i < Data.MN; i++)
	{
		for (int j = 0; j < Data.MN; j++) {
			cout << Data.Hg[i][j] << " ";
		}
		cout << endl;
	}

	cout << endl;

	cout << "MACIERZ P: \n";
	for (int i = 0; i < Data.MN; i++)
	{
		cout << Data.Pg[i] << endl;

	}
	
	//odwróc macierz HG

	//t=inverseHG * P

	cout << endl;

	cout << "TEMPERATURY: \n";
	for (int i = 0; i < Data.MN; i++)
	{
		cout << Data.Tg[i] << endl;

	}
	
	Data.Hg = NULL;
	Data.Pg = NULL;
	Data.Tg = NULL;
	delete Data.Hg, Data.Pg, Data.Tg;
}