#include "Cell.h"
#include "Net.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <cmath>
#include <random>
#include <algorithm>
#include <map>

using namespace std;

Cell findCell(string id, vector<vector<Cell>> placement);
Cell findCell2(string id, map<string, Cell> &hash);
void updateHash(vector<vector<Cell>> placement, map<string, Cell>& hash);
vector<Net> updateNets(vector<vector<Cell>> placement, map<string, Cell>& hash);
float Cost1(vector<vector<Cell>> placement, vector<Net> paths); //total nets
float Cost2(vector<vector<Cell>> placement, vector<Net> paths); //critical net
float Cost3(vector<vector<Cell>> placement); //row difference
float Cost4(vector<vector<Cell>> placement, vector<Net> paths); //congestion
float normalizeCost(float c1, float c2, float c3, float c4, float maxC1, float maxC2, float maxC3, float maxC4);
float increaseProbability(float deltaC, float temp);
float tempSchedule(float curTemperature);
void printstuff1(vector<vector<Cell>> placement);
vector<vector<Cell>> Perturb(vector<vector<Cell>> table);

int main()
{
	Cell c("test", 1001, 0, 0);
	Net n(c, { c });
	string temp;
	string temp2;
	int temp3;
	int count = 0;
	int colCount = 0;
	int totalCells = 0;
	int totalWeight = 0;
	int curWeight = 0;
	int numrows = 6;
	int rowWeight;
	vector<Cell> templst;
	vector<vector<Cell>> table, newTable;
	map<string, Cell> hash;
	ifstream weights;
	ifstream nets;

	string NetTemp;
	string NetTemp2;
	string NetTemp4;
	vector <Cell> nodetemp;
	int index;
	vector<Net> templst2;
	int snt2;

	float curTemperature = 4000000;
	float finalTemperature = .1;
	float maxCost1, maxCost2, maxCost3, maxCost4;
	float curCost, newCost;
	float randNum;

	srand(time(NULL));

	weights.open("ibm02.are", ios::in);
	if (weights.fail()) {
		cerr << "This file does not exist." << endl;
	};

	while (getline(weights, temp))
	{
		//cout << temp << "\n";
		temp2 = temp.substr(0, temp.find(" "));
		temp3 = stoi(temp.substr(temp.find(" ")));// weight
		totalWeight += temp3;
		c.setId(temp2);
		c.setWeight(temp3);
		templst.push_back(c);
	}
	//cout << lst[0][12751].getId() << endl;

	rowWeight = totalWeight / numrows;

	while (!templst.empty())
	{
		table.push_back(vector<Cell>());
		colCount = 0;
		while (curWeight <= rowWeight && !templst.empty())
		{
			templst.back().setR(count);
			templst.back().setC(colCount);
			table[count].push_back(templst.back());
			curWeight += templst.back().getWeight();
			//cout << hash.begin() ->first << endl;
			templst.pop_back();
			totalCells++;
			colCount++;
		}
		curWeight = 0;
		count++;
	}

	//initial placement created
	cout << "init placed" << endl;

	updateHash(table, hash);

	cout << "hash made" << endl;

	templst2 = updateNets(table, hash);

	cout << "nets set" << endl;

	//make the max Costs so we can normalize and compare later
	maxCost1 = Cost1(table, templst2);
	maxCost2 = Cost2(table, templst2);
	maxCost3 = Cost3(table);
	maxCost4 = Cost4(table, templst2);
	curCost = normalizeCost(maxCost1, maxCost2, maxCost3, maxCost4, maxCost1, maxCost2, maxCost3, maxCost4);
	while (curTemperature > finalTemperature)
	{
		for (int i = 0; i < (totalCells/500); i++)
		{
			newTable = Perturb(table);
			newCost = normalizeCost(Cost1(newTable, templst2), Cost2(newTable, templst2), Cost3(newTable), Cost4(newTable, templst2), 
				maxCost1, maxCost2, maxCost3, maxCost4);
			//cout << newCost <<endl;

			randNum = (float)rand() / RAND_MAX;
			//cout << randNum << endl;

			if (newCost < curCost)
			{
				table = newTable;
				updateHash(table, hash);
				templst2 = updateNets(table, hash);
				curCost = newCost;
			}
			else if (randNum > increaseProbability((newCost - curCost), curTemperature) && curTemperature > 100)
			{
				table = newTable;
				updateHash(table, hash);
				templst2 = updateNets(table, hash);
				curCost = newCost;
				cout << "Random jump up!" << endl;
			}
		}
		curTemperature = tempSchedule(curTemperature);
		cout << curCost << ", " << curTemperature << endl;
	}
	printstuff1(table);
}

Cell findCell(string id, vector<vector<Cell>> placement)
{
	for (int j = 0; j < placement.size(); j++) // finds coords of starting Cell
	{
		for (int k = 0; k < placement[j].size(); k++)
		{
			if (placement[j][k].getId() == id)
			{
				return placement[j][k];
			}
		}
	}
	cout << "ERROR - findCell found nothing" << endl;
	return Cell();
}

Cell findCell2(string id, map<string, Cell> &hash)
{
	if (hash.count(id) ) {
//		cout << hash[id].getWeight() << endl;
		return hash[id];
	}
	cout << "ERROR - findCell found nothing" << endl;
	return Cell();
}

void updateHash(vector<vector<Cell>> placement, map<string, Cell>& hash)
{
	for (int i = 0; i < placement.size(); i++) {
		for (int j = 0; j < placement[i].size(); j++) {
			hash[placement[i][j].getId()] = placement[i][j];
		}
	}
}

vector<Net> updateNets(vector<vector<Cell>> placement, map<string, Cell>& hash)
{
	Cell c("test", 1001, 0, 0);
	Net n(c, { c });
	string NetTemp;
	string NetTemp2;
	string NetTemp4;
	vector <Cell> nodetemp;
	int index;
	vector<Net> templst2;
	int snt2;
	ifstream nets;
	
	nets.open("ibm02.net", ios::in);
	if (nets.fail()) {
		cerr << "This file does not exist." << endl;
	};

	while (getline(nets, NetTemp))
	{

		NetTemp2 = NetTemp.substr(0, NetTemp.find(" ")); //id
		snt2 = NetTemp2.size();
		index = snt2 + 1;
		//index = NetTemp.find(snt2+1 );
		NetTemp4 = NetTemp.substr(index, 1); // s or l
		if (NetTemp4 == "s") {
			if (!nodetemp.empty()) {
				n.setNodes(nodetemp);
				nodetemp.clear();
				templst2.push_back(n);
				n.setFirstNode(findCell2(NetTemp2, hash));
			}
			else {
				n.setFirstNode(findCell2(NetTemp2, hash));
			}
		}
		else {
			nodetemp.push_back(findCell2(NetTemp2, hash));

		}


	}
	n.setNodes(nodetemp);
	templst2.push_back(n);

	nets.close();
	return templst2;
}

float Cost1(vector<vector<Cell>> placement, vector<Net> paths)
{
	float totalLen = 0;
	float curLen, rowDif, colDif;
	Cell strNode;
	for (Net i : paths)// for every Net object in vector
	{
		strNode = i.getFirstNode();
		for (Cell j : i.getNodes()) //for every Cell that starting Cell is connected to
		{
			rowDif = abs(j.getR() - strNode.getR()) + 1; //calculate abs value of difference in coords
			colDif = abs(j.getC() - strNode.getC()) + 1; //+1 is used to ensure value is never 0
			curLen = rowDif * colDif; //calcualate a value to represent length of net
			totalLen += curLen; //add to summation of all net lengths
		}
	}
	return totalLen;
}

float Cost2(vector<vector<Cell>> placement, vector<Net> paths)
{
	float maxLen = 0;
	float curLen, rowDif, colDif;
	Cell strNode;
	for (Net i : paths)// for every Net object in vector
	{
		strNode = i.getFirstNode();
		for (Cell j : i.getNodes()) //for every Cell that starting Cell is connected to
		{
			rowDif = abs(j.getR() - strNode.getR()) + 1; //calculate abs value of difference in coords
			colDif = abs(j.getC() - strNode.getC()) + 1; //+1 is used to ensure value is never 0
			curLen = rowDif * colDif;
			if (curLen > maxLen)
				maxLen = curLen;
		}
	}
	return maxLen;
}

float Cost3(vector<vector<Cell>> placement) {
	float max;
	float min;
	float temp = 0;
	float dif;
	//6 rows
	vector<float> totalweight;
	for (int i = 0; i < placement.size(); i++) {
		temp = 0;
		for (int j = 0; j < placement[i].size(); j++) {
			temp = temp + placement[i][j].getWeight();
		}
		totalweight.push_back(temp);
		//cout << temp << endl;
	}

	//finding minimum  & maximum element
	min = *min_element(totalweight.begin(), totalweight.end());
	max = *max_element(totalweight.begin(), totalweight.end());
	dif = max - min;
	//cout << dif << endl;
	return dif;

}

float Cost4(vector<vector<Cell>> placement, vector<Net> paths)
{
	vector<vector<int>> netAmt; //amount of nets in various regions
	Cell strNode;
	int maxVal = 0;
	int maxRow = 0;

	for (int i = 0; i < placement.size(); i++)
	{
		if (placement[i].size() > maxRow)
			maxRow = placement[i].size();
	}
	int colCutoff = (maxRow / 6) + 1;

	netAmt =
	{
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
		{0,0,0,0,0,0},
	};

	for (Net i : paths)// for every Net object in vector
	{
		strNode = i.getFirstNode();
		for (Cell j : i.getNodes()) //for every Cell that starting Cell is connected to
		{
			for (int k = fmin(strNode.getR(), j.getR()); k < fmax(strNode.getR(), j.getR()); k++)
			{
				netAmt[k][fmin((strNode.getC() / colCutoff), (j.getC() / colCutoff))]++;
			}
			for (int l = fmin((strNode.getC() / colCutoff), (j.getC() / colCutoff)); l <= fmax((strNode.getC() / colCutoff), (j.getC() / colCutoff)); l++)
			{
				netAmt[fmax(strNode.getR(), j.getR())][l]++;
			}
		}
	}

	for (int x = 0; x < 6; x++)
	{
		for (int y = 0; y < 6; y++)
		{
			if(netAmt[x][y] > maxVal)
				maxVal = netAmt[x][y];
			//cout << netAmt[x][y] << ", ";
		}
		//cout << endl;
	}

	return maxVal;
}

float normalizeCost(float c1, float c2, float c3, float c4, float maxC1, float maxC2, float maxC3, float maxC4)
{
	float normC1 = c1 / maxC1;
	float normC2 = c2 / maxC2;
	float normC3 = c3 / maxC3;
	float normC4 = c4 / maxC4;
	return (normC1 + normC2 + normC3 + normC4);
}

vector<vector<Cell>> Perturb(vector<vector<Cell>> table)
{
	int randomRow1, randomCol1, randomRow2, randomCol2;
	int randomChoice;
	vector<vector<Cell>> newTable;
	Cell cell, cell2, holdCell;
	vector<Cell>::iterator it, it2;

	newTable = table;

	randomRow1 = rand() % table.size();
	randomCol1 = rand() % table[randomRow1].size();
	cell = newTable[randomRow1][randomCol1];
	/*cout << "cell1 pre perturb " << newTable[randomRow1][randomCol1].getId() << " " << newTable[randomRow1][randomCol1].getR() << "," << 
		newTable[randomRow1][randomCol1].getC() << endl;*/

	randomRow2 = rand() % table.size();
	randomCol2 = rand() % table[randomRow2].size();

	randomChoice = rand() % 2;
	if (randomChoice == 0)
	{
		cell2 = newTable[randomRow2][randomCol2];
		/*cout << "cell2 pre perturb " << newTable[randomRow2][randomCol2].getId() << " " << newTable[randomRow2][randomCol2].getR() << "," <<
			newTable[randomRow2][randomCol2].getC() << endl;*/

		holdCell.setId(cell.getId());
		holdCell.setWeight(cell.getWeight());
		cell.setId(cell2.getId());
		cell.setWeight(cell2.getWeight());
		cell2.setId(holdCell.getId());
		cell2.setWeight(holdCell.getWeight());

		newTable[randomRow1][randomCol1] = cell;
		newTable[randomRow2][randomCol2] = cell2;

		/*cout << "cell1 post perturb " << newTable[randomRow1][randomCol1].getId() << " " << newTable[randomRow1][randomCol1].getR() << "," <<
			newTable[randomRow1][randomCol1].getC() << endl;
		cout << "cell2 post perturb " << newTable[randomRow2][randomCol2].getId() << " " << newTable[randomRow2][randomCol2].getR() << "," <<
			newTable[randomRow2][randomCol2].getC() << endl;*/
	}

	else
	{
		it = newTable[randomRow1].begin();
		it2 = newTable[randomRow2].begin();
		newTable[randomRow1].erase(it + randomCol1);

		for (int i = randomCol1; i < newTable[randomRow1].size(); i++)
		{
			newTable[randomRow1][i].setC(newTable[randomRow1][i].getC() - 1);
		}

		/*cout << "new cell1 " << newTable[randomRow1][randomCol1].getId() << " " << newTable[randomRow1][randomCol1].getR() << "," <<
			newTable[randomRow1][randomCol1].getC() << endl;*/

		cell.setR(randomRow2);
		cell.setC(randomCol2);

		newTable[randomRow2].insert(it2 + randomCol2, cell);


		/*cout << "cell1 post perturb " << newTable[randomRow2][randomCol2].getId() << " " << newTable[randomRow2][randomCol2].getR() << "," <<
			newTable[randomRow2][randomCol2].getC() << endl;*/

		for (int i = randomCol2 + 1; i < newTable[randomRow2].size(); i++)
		{
			newTable[randomRow2][i].setC(newTable[randomRow2][i].getC() + 1);
		}
	}

	return newTable;
}

float increaseProbability(float deltaC, float temp)
{
	float exponent = (deltaC * -1) / temp;
	float result = exp(exponent);
	return result;
}

float tempSchedule(float curTemperature)
{
	float newTemp;

	if (curTemperature > 15000)
		newTemp = curTemperature * .8;
	else if(curTemperature > 5000)
		newTemp = curTemperature * .95;
	else
		newTemp = curTemperature * .8;

	return newTemp;
}

void printstuff1(vector<vector<Cell>> placement)
{
	int rowW = 0;

	ofstream MyFile("output1.txt");
	for (int i = 0; i < placement.size(); i++) {
		rowW = 0;

		for (int j = 0; j < placement[i].size(); j++) {
			if (placement[i][j].getId()[0] == 'p') {
				MyFile << placement[i][j].getR() << " " << rowW << " " << placement[i][j].getWeight() << endl;
				rowW++;
			}
			else {
				MyFile << placement[i][j].getR() << " " << rowW << " " << placement[i][j].getWeight() << endl;

				rowW += placement[i][j].getWeight();
			}
		}

	}
	MyFile.close();
}
