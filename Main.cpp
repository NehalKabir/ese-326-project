#include "Cell.h"
#include "Net.h"
#include <string.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <math.h>
#include <random>

using namespace std;

Cell findCell(string id, vector<vector<Cell>> placement);
float Cost1(vector<vector<Cell>> placement, vector<Net> paths);
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
	vector<vector<Cell>> table;
	ifstream weights;
	ifstream nets;

	string NetTemp;
	string NetTemp2;
	string NetTemp3;
	string NetTemp4;
	vector <Cell> nodetemp;
	int index;
	vector<Net> templst2;
	int snt2;

	float curTemperature = 4000000;
	float finalTemperature = .1;

	srand(time(NULL));

	weights.open("ibm01.are", ios::in);
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
			templst.pop_back();
			totalCells++;
			colCount++;
		}
		curWeight = 0;
		count++;
	}
	//initial placement created
	cout << "init placed" << endl;

	nets.open("ibm01.net", ios::in);
	if (nets.fail()) {
		cerr << "This file does not exist." << endl;
	};

	while (getline(nets, NetTemp))
	{

		NetTemp2 = NetTemp.substr(0, NetTemp.find(" "));
		snt2 = NetTemp2.size();
		index = snt2 + 1;
		//index = NetTemp.find(snt2+1 );
		NetTemp4 = NetTemp.substr(index, 1);
		if (NetTemp4 == "s") {
			if (!nodetemp.empty()) {
				n.setNodes(nodetemp);
				nodetemp.clear();
				templst2.push_back(n);
				n.setFirstNode(findCell(NetTemp2, table));
			}
			else {
				n.setFirstNode(findCell(NetTemp2, table));
			}
		}
		else {
			nodetemp.push_back(findCell(NetTemp2, table));

		}


	}
	n.setNodes(nodetemp);
	templst2.push_back(n);
	cout << "nets recorded" << endl;
	//for (int i = 0; i < templst2.size(); i++) {
	//	// Printing the element at
	//	// index 'i' of vector
	//	cout << templst2[i].getFirstNode() << endl;
	//	for (string j : templst2[i].getNodes()) {
	//		cout << j << endl;
	//	}
	//}

	cout << Cost1(table, templst2) << endl;

	while (curTemperature > finalTemperature)
	{
		for (int i = 0; i < (totalCells * 800); i++)
		{

		}
	}
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

vector<vector<Cell>> Perturb(vector<vector<Cell>> table)
{
	int randomRow1, randomCol1, randomRow2, randomCol2;
	int randomChoice;
	vector<vector<Cell>> newTable;

	newTable = table;
	randomRow1 = rand() % table.size();
	randomCol1 = rand() % table[randomRow1].size();
	Cell cell = newTable[randomRow1][randomCol1];
	randomChoice = rand() % 2;
	if (randomChoice == 0)
	{

	}
	else
	{

	}
	return newTable;
}
