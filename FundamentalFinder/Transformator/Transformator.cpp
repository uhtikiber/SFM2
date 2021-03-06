#include "pch.h"
#include "Transformator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;


Transformator::Transformator(string folderNameIn, string folderNameOut) : folderNameIn(folderNameIn), folderNameOut(folderNameOut)
{
}


Transformator::~Transformator()
{
}

void Transformator::transformFile(ifstream & fin, ofstream & fout, string name, bool no_new_lines) {
	char c;
	char i, j, z;
	char p;
	string s, temp;
	fout << name << " = ";
	while (fin >> c) {
		if (c == 'q') { //case for q_i_j
			fin >> c; // "_"
			fin >> i;
			fin >> c;
			if (c != '_') { // 1 <= i <= 10
				z = c;
				fin >> c;
			}
			else {
				z = '1';
			}
			fin >> j;
			temp = "Q[";
			if (z == '0') {
				temp.push_back('9');
			}
			else {
				temp.push_back(i - 1);
			}
			temp += "][";
			temp.push_back(j - 1);
			temp.push_back(']');
			s.append(temp);
			temp.clear();
		}
		else if ((c == '+' || c == '-') && !no_new_lines) {
			if (!s.empty()) {
				//s.pop_back(); //presumably there will be spaces between symbols
				fout << s << ";" << endl;
				fout << name << " " << c << "= "; // += or -=
				s.clear(); \
			}
		}
		else if (c == '^') { //case for ^ -> pow(,)
							 // there are no spaces surrounding ^
			fin >> p; //power
			i = '^';
			while (i != 'Q' && i != 'L') {
				i = s.back();
				s.pop_back();
				temp.push_back(i);
			}
			reverse(temp.begin(), temp.end());
			s += "pow(" + temp + ", " + p + ")";
			temp.clear();
		}
		else if (c == ';') {
			fout << s << ';' << endl;
			s.clear();
		}
		else {
			s.push_back(c);
		}
	}
	fout << s;
	fout << ";" << endl;
}


void Transformator::transformCoefficients() {
	ifstream fin;
	ofstream fout;
	string fileNameIn, fileNameOut, numString;
	for (int i = 0, num = '0'; i < 11; i++, num++) {
		if (i != 10)
			numString = num;
		else
			numString = "10";
		fileNameIn = folderNameIn + "poly" + numString + ".txt";
		fin.open(fileNameIn);

		fileNameOut = folderNameOut + "poly" + numString + ".cpp";
		fout.open(fileNameOut);

		fout << "#include \"stdafx.h\"" << endl << "#include <cmath>" << endl << endl;
		fout << "void find_" << (num - '0') << "_coefficient(double & X, double ** Q) {" << endl;
		transformFile(fin, fout, "X");
		fout << "}";

		fin.close();
		fout.close();
	}
}

void Transformator::transformForL2() {
	ofstream fout(folderNameOut + "coeffsL2.cpp");
	fout << "#include \"stdafx.h\"" << endl << "#include <cmath>" << endl << endl;
	fout << "void findCoefficientsForL2Poly(double * L2, double ** Q, double L_1) {" << endl;
	fout << "double a0, a1, a2;" << endl;
	string numStr;
	for (char i = '0'; i < '3'; i++) {
		numStr = i;
		ifstream fin(folderNameIn + "coeffs_for_L2" + numStr + ".txt");
		transformFile(fin, fout, "a" + numStr);
		fin.close();
	}
	fout << "if (a == 0) return -c / b;" << endl;
	fout << "else return (-b + sqrt(b * b - 4 * a * c)) / (2 * a)" << endl;
	fout << "}";
	fout.close();
}

void Transformator::transformForF23() {
	ifstream fin(folderNameIn + "coeffs_for_F_2_3.txt");
	ofstream fout(folderNameOut + "findF23.cpp");
	fout << "#include \"stdafx.h\"" << endl << "#include <cmath>" << endl << endl;
	fout << "void findCoefficientsForF23( MatrixXd & Q, double L_1, double L_2) {" << endl;
	fout << "double X;" << endl;
	transformFile(fin, fout, "X", true);
	fout << "return X;" << endl;
	fout << "}";
	fin.close();
	fout.close();
}

void Transformator::transformAll() {
	transformCoefficients();
	transformForL2();
	transformForF23();
}