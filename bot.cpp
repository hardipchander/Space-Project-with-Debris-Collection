// Hardip Chander
// This program makes the robots get the debris efficently

#include <cstdlib>
#include <iostream>
#include "bot.h"
#include <cmath>
#include <vector>
using namespace std;
const int MAX_ROBOT_NUM = 50;
int NUM;
int ROWS, COLS;

// Array for malfunctioning robots 
bool broken[50];

/* Initialization procedure, called when the game starts: */
void onStart(int num, int rows, int cols, double mpr, Area& area, ostream& log)
{
	// save the number of robots and the map size
	NUM = num;
	ROWS = rows;
	COLS = cols;

	// intialize the broken 2d array to false
	for (int b = 0; b < NUM; b++)
	{
		broken[b] = false;
	}
}

// Helper Function distance between 2 locations 
double distance(double row1, double col1, double row2, double col2)
{
	double distance = 0;
	distance = sqrt(pow((row2 - row1), 2) + pow((col2 - col1), 2));
	return distance;
}

// Helper Function that returns the number of Debris 
int countDebris(Area& area)
{
	int counter = 0;
	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (area.inspect(r, c) == DEBRIS)
			{
				counter++;
			}
		}
	}
	return counter;
}

// Helper Function move into destination 
Action move(Loc currLoc, Loc destination, Area& area)
{
	int row = currLoc.r;
	int col = currLoc.c;
	int desRow = destination.r;
	int desCol = destination.c;

	// add  this to diverse the motion 
	int mode = rand() % 2;

	// Mode 1
	if (mode == 0)
	{
		if (row > desRow)
		{
			return UP;
		}
		else if (row == desRow)
		{
			if (col < desCol)
			{
				return RIGHT;
			}
			else if (col > desCol)
			{
				return LEFT;
			}
			else
			{
				return COLLECT;
			}
		}
		else
		{
			return DOWN;
		}
	}
	// Mode 2
	else
	{
		if (col < desCol)
		{
			return RIGHT;
		}
		else if (col == desCol)
		{
			if (row > desRow)
			{
				return UP;
			}
			else if (row < desRow)
			{
				return DOWN;
			}
			else
			{
				return COLLECT;
			}
		}
		else
		{
			return LEFT;
		}
	}

}

// Helper Function that returns location of the closest debris
Loc closestDebris(int id, Area& area)
{
	Loc output;
	Loc curr = area.locate(id);
	int count = 0;

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (area.inspect(r, c) == DEBRIS)
			{
				if (count == 0)
				{
					Loc deb = { r,c };
					output = deb;
					count++;
				}
				else
				{
					Loc debris = { r,c };
					// the coordinates 
					double r1 = curr.r;
					double c1 = curr.c;
					double r2 = debris.r;
					double c2 = debris.c;
					double r3 = output.r;
					double c3 = output.c;
					double dist = distance(r1, c1, r2, c2);
					double distBefore = distance(r1, c1, r3, c3);
					if (dist < distBefore)
					{
						output = debris;
					}
				}
			}
		}
	}

	return output;
}

// Helper Function that returns location of the closest debris Updated Version !!! 
Loc closestDebrisUpdate(int id, Area& area, Loc without)
{
	Loc output;
	Loc curr = area.locate(id);
	int count = 0;

	for (int ro = 0; ro < ROWS; ro++)
	{
		for (int co = 0; co < COLS; co++)
		{
			if (without.r != ro || without.c != co)
			{
				if (area.inspect(ro, co) == DEBRIS)
				{
					if (count == 0)
					{
						Loc deb = { ro,co };
						output = deb;
						count++;
					}
					else
					{
						Loc debris = { ro,co };
						// the coordinates 
						double r1 = curr.r;
						double c1 = curr.c;
						double r2 = debris.r;
						double c2 = debris.c;
						double r3 = output.r;
						double c3 = output.c;
						double dist = distance(r1, c1, r2, c2);
						double distBefore = distance(r1, c1, r3, c3);
						if (dist < distBefore)
						{
							output = debris;
						}
					}
				}
			}
		}
	}

	return output;
}

// Helper function that returns a vector of broken IDS
vector<int> numBroken()
{
	vector<int> output;
	for (int b = 0; b < NUM; b++)
	{
		if (broken[b] == true)
		{
			output.push_back(b);
		}
	}
	return output;
}

// Helper Function return closestRobot to broke in the form of a int 
int closeRobot(int brokenID, Area& area)
{
	int output = 0;
	int counter = 0;
	double rowBR = area.locate(brokenID).r;
	double colBR = area.locate(brokenID).c;
	for (int x = 0; x < NUM; x++)
	{
		if (x != brokenID)
		{
			if (counter == 0)
			{
				output = x;
				counter++;
			}
			else
			{
				double rowCurr = area.locate(x).r;
				double colCurr = area.locate(x).c;
				double dist = distance(rowCurr, colCurr, rowBR, colBR);

				double rowBef = area.locate(output).r;
				double colBef = area.locate(output).c;
				double distBef = distance(rowBef, colBef, rowBR, colBR);

				if (dist < distBef)
				{
					output = x;
				}
			}
		}
	}
	return output;
}

// Main Action occurs here 
Action onRobotAction(int id, Loc loc, Area& area, ostream& log)
{
	// Current Location of the robot 
	int row = loc.r;
	int col = loc.c;

	int sameID = -1;

	// Malfunction Code 
	int nearBrokenID = -1;
	vector<int> listBroken = numBroken();
	if (listBroken.size() != 0)
	{
		int len = listBroken.size();
		for (int v = 0; v < len; v++)
		{
			int idNear = closeRobot(listBroken[v], area);
			if (id == idNear)
			{
				nearBrokenID = listBroken[v];
			}
		}
	}

	// Move towards the broken robot 
	if (nearBrokenID != -1)
	{
		// In the chain update the values 
		// Very important 
		Loc brokeR = area.locate(nearBrokenID);
		if (row == (brokeR.r + 1) && col == brokeR.c)
		{
			broken[nearBrokenID] = false;
			nearBrokenID = -1;
			return REPAIR_UP;
		}
		else if (row == (brokeR.r - 1) && col == brokeR.c)
		{
			broken[nearBrokenID] = false;
			nearBrokenID = -1;
			return REPAIR_DOWN;
		}
		else if (row == brokeR.r && (col - 1) == brokeR.c)
		{
			broken[nearBrokenID] = false;
			nearBrokenID = -1;
			return REPAIR_LEFT;
		}
		else if (row == brokeR.r && (col + 1) == brokeR.c)
		{
			broken[nearBrokenID] = false;
			nearBrokenID = -1;
			return REPAIR_RIGHT;
		}

		Action mal = move(loc, brokeR, area);
		return mal;
	}

	// Strategy move towards the closest debris
	if (area.inspect(row, col) == DEBRIS)
	{
		if (sameID != -1)
		{
			sameID = -1;
		}
		return COLLECT;
	}
	else
	{
		Loc closeD = closestDebris(id, area);
		Action m = move(loc, closeD, area);

		// Making the code more efficent
		int countD = countDebris(area);
		if (countD >= NUM)
		{
			for (int s = 0; s < NUM; s++)
			{
				if (s != id)
				{
					// geting the close Debiris for each ID and then compare it 
					Loc close = closestDebris(s, area);
					// if the close debris equals the same loc for this id 
					if (close.r == closeD.r && close.c == closeD.c)
					{
						sameID = s;
					}
				}
			}
		}
		// handle if two robots have the same loc
		if (sameID != -1)
		{
			Loc clo = closestDebris(sameID, area);
			Loc updateClosestDeb = closestDebrisUpdate(id, area, clo);
			m = move(loc, updateClosestDeb, area);
		}

		return m;
	}

}

// When robot breaks here 
void onRobotMalfunction(int id, Loc loc, Area& area, ostream& log)
{
	log << "Robot " << id << " is damaged." << endl;
	broken[id] = true;
}

// display time method 
// not going to use it that much 
void onClockTick(int time, ostream& log)
{
	//if(time % 100 == 0) log << time << " ";
}

