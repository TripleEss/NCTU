#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<utility>

using namespace std;

void createKB(string);
void CNF(vector<string>&);
void CNF_EQUIVALENCE(vector<string>&);
void CNF_IMPLICATION(vector<string>&);
void CNF_NOT(vector<string>&);
void CNF_StandardizeVariable(vector<string>&);
void CNF_EXIST(vector<string>&);
void CNF_FORALL(vector<string>&);
void CNF_Distribution(vector<string>&);

void removeBrackets(vector<string>&);

vector<string> getBinaryOperandLF(vector<string>);
vector<string> getBinaryOperandRT(vector<string>);
vector<string> getUnaryOperandRT(vector<string>);

vector<string> variable;
vector<string> constant;
vector<pair<string, int>> predicate;
vector<vector<string>> fol;
vector<vector<pair<bool, string>>> cnf;

int main()
{
	string fname;
	cin >> fname;

	createKB(fname);

	for (unsigned int i = 0; i < fol.size(); i++)
		CNF(fol[i]);

	cin >> fname;
}

void createKB(string fname)
{
	fname += ".KB";
	ifstream fkb(fname);

	cout << endl << "--------------- KB START ---------------" << endl << endl;

	string str;
	while (getline(fkb, str))
	{
		stringstream sstr(str);
		while (getline(sstr, str, ' '))
		{
			if (str == "Variables:")
			{
				while (getline(sstr, str, ','))
					variable.push_back(str);

				cout << "# VARIABLES" << endl;
				for (unsigned int i = 0; i < variable.size(); i++)
					cout << variable[i] << endl;
				cout << endl;
			}
			else if (str == "Constants:")
			{
				while (getline(sstr, str, ','))
					constant.push_back(str);

				cout << "# CONSTANTS" << endl;
				for (unsigned int i = 0; i < constant.size(); i++)
					cout << constant[i] << endl;
				cout << endl;
			}
			else if (str == "Predicates:")
			{
				while (getline(sstr, str, ')'))
					predicate.push_back(make_pair(str, 0));

				for (unsigned int i = 0; i < predicate.size(); i++)
				{
					stringstream sstr(predicate[i].first);

					getline(sstr, str, '(');
					predicate[i].first = str;
					if (i>0)
						predicate[i].first.erase(predicate[i].first.begin());

					while (getline(sstr, str, ','))
						predicate[i].second++;
				}

				cout << "# PREDICATES" << endl;
				for (unsigned int i = 0; i < predicate.size(); i++)
					cout << predicate[i].first << " : " << predicate[i].second << endl;
				cout << endl;
			}
			else
			{
				while (getline(fkb, str))
				{
					if (str.empty())
						continue;

					sstr = (stringstream)str;

					vector<string> tmpfol;
					while (getline(sstr, str, ' '))
					{
						if (str.empty())
							continue;

						while (str[0] == '[')
						{
							tmpfol.push_back("[");
							str.erase(str.begin());
						}

						int rtbracket = 0;
						while (str[str.size() - 1] == ']')
						{
							rtbracket++;
							tmpfol.push_back("]");
							str.erase(str.end() - 1);
						}

						if (str.empty())
							continue;

						if (tmpfol.size() > 0)
							tmpfol.insert(tmpfol.begin() + tmpfol.size() - rtbracket, str);
						else
							tmpfol.push_back(str);
					}
					fol.push_back(tmpfol);
				}
				fol.pop_back();

				cout << "# LOGIC" << endl;
				for (unsigned int i = 0; i < fol.size(); i++)
				{
					for (unsigned int j = 0; j < fol[i].size(); j++)
						cout << fol[i][j] << " ";
					cout << endl;
				}
				cout << endl;
				break;
			}
		}
	}
}

void CNF(vector<string> &logic)
{
	cout << endl << "-------------- CNF START ---------------" << endl << endl;
	for (unsigned int i = 0; i < logic.size(); i++)
		cout << logic[i] << " ";
	cout << endl << endl;

	CNF_EQUIVALENCE(logic);
	CNF_IMPLICATION(logic);
	CNF_NOT(logic);
	CNF_StandardizeVariable(logic);
	CNF_EXIST(logic);
	CNF_FORALL(logic);
	CNF_Distribution(logic);
	removeBrackets(logic);
}

void CNF_EQUIVALENCE(vector<string> &logic)
{
	// remove EQUIVALENCE
	// P <=> Q ~ [ [ [P] => [Q] ] & [ [Q] => [P] ] ]

	cout << "###### remove EQUIVALENCE start" << endl << endl;

	vector<string> Pstack;
	vector<string> Qstack;

	Pstack.clear();
	Qstack = logic;
	while (!Qstack.empty())
	{
		if (Qstack[0] != "<=>")
		{
			Pstack.push_back(Qstack[0]);
			Qstack.erase(Qstack.begin());
		}
		else
		{
			Qstack.erase(Qstack.begin());

			vector<string> P = getBinaryOperandLF(Pstack);
			vector<string> Q = getBinaryOperandRT(Qstack);

			logic.clear();
			for (unsigned int i = 0; i < Pstack.size() - P.size(); i++)
				logic.push_back(Pstack[i]);
			logic.push_back("[");
			logic.push_back("[");
			logic.push_back("[");
			for (unsigned int i = 0; i < P.size(); i++)
				logic.push_back(P[i]);
			logic.push_back("]");
			logic.push_back("=>");
			logic.push_back("[");
			for (unsigned int i = 0; i < Q.size(); i++)
				logic.push_back(Q[i]);
			logic.push_back("]");
			logic.push_back("]");
			logic.push_back("&");
			logic.push_back("[");
			logic.push_back("[");
			for (unsigned int i = 0; i < Q.size(); i++)
				logic.push_back(Q[i]);
			logic.push_back("]");
			logic.push_back("=>");
			logic.push_back("[");
			for (unsigned int i = 0; i < P.size(); i++)
				logic.push_back(P[i]);
			logic.push_back("]");
			logic.push_back("]");
			logic.push_back("]");
			for (unsigned int i = Q.size(); i < Qstack.size(); i++)
				logic.push_back(Qstack[i]);

			Pstack.clear();
			Qstack = logic;

			cout << "# remove EQUIVALENCE P<=>Q" << endl;
			for (unsigned int i = 0; i < logic.size(); i++)
				cout << logic[i] << " ";
			cout << endl << endl;
		}
	}
}

void CNF_IMPLICATION(vector<string> &logic)
{
	// remove IMPLICATION
	// P => Q ~ [ ! [P] | [Q] ]

	cout << "###### remove IMPLICATION start" << endl << endl;

	vector<string> Pstack;
	vector<string> Qstack;

	Pstack.clear();
	Qstack = logic;
	while (!Qstack.empty())
	{
		if (Qstack[0] != "=>")
		{
			Pstack.push_back(Qstack[0]);
			Qstack.erase(Qstack.begin());
		}
		else
		{
			Qstack.erase(Qstack.begin());

			vector<string> P = getBinaryOperandLF(Pstack);
			vector<string> Q = getBinaryOperandRT(Qstack);

			logic.clear();
			for (unsigned int i = 0; i < Pstack.size() - P.size(); i++)
				logic.push_back(Pstack[i]);
			logic.push_back("[");
			logic.push_back("!");
			logic.push_back("[");
			for (unsigned int i = 0; i < P.size(); i++)
				logic.push_back(P[i]);
			logic.push_back("]");
			logic.push_back("|");
			logic.push_back("[");
			for (unsigned int i = 0; i < Q.size(); i++)
				logic.push_back(Q[i]);
			logic.push_back("]");
			logic.push_back("]");
			for (unsigned int i = Q.size(); i < Qstack.size(); i++)
				logic.push_back(Qstack[i]);

			Pstack.clear();
			Qstack = logic;

			cout << "# remove IMPLICATION P=>Q" << endl;
			for (unsigned int i = 0; i < logic.size(); i++)
				cout << logic[i] << " ";
			cout << endl << endl;
		}
	}
}

void CNF_NOT(vector<string> &logic)
{
	// drive in NOT
	// ! R

	cout << "###### drive in NOT start" << endl << endl;

	vector<string> Ostack;
	vector<string> Pstack;
	vector<string> Qstack;
	vector<string> Rstack;

	Ostack.clear();
	Rstack = logic;
	while (!Rstack.empty())
	{
		bool drivein = false;

		if (Rstack[0] != "!")
		{
			Ostack.push_back(Rstack[0]);
			Rstack.erase(Rstack.begin());
		}
		else
		{
			Rstack.erase(Rstack.begin());

			logic.clear();
			for (unsigned int i = 0; i < Ostack.size(); i++)
				logic.push_back(Ostack[i]);

			vector<string> R = getUnaryOperandRT(Rstack);

			// ! A
			if (R.size() == 1)
			{
				logic.push_back("!");
				logic.push_back(R[0]);

				for (unsigned int i = R.size(); i < Rstack.size(); i++)
					logic.push_back(Rstack[i]);

				Ostack.push_back("!");
				Ostack.push_back(R[0]);
				Rstack.erase(Rstack.begin());

				continue;
			}

			// ! [ A(x) ] ~ ! A(x)
			if (R[0] == "[" && R[2] == "]")
			{
				logic.push_back("!");
				logic.push_back(R[1]);

				for (unsigned int i = R.size(); i < Rstack.size(); i++)
					logic.push_back(Rstack[i]);

				cout << "# remove []" << endl;
				for (unsigned int i = 0; i < logic.size(); i++)
					cout << logic[i] << " ";
				cout << endl << endl;

				Ostack.push_back("!");
				Ostack.push_back(R[1]);
				Rstack.erase(Rstack.begin(), Rstack.begin() + 3);

				continue;
			}

			// ! [Exist x Q] ~ [ Forall x [ ! [Q] ] ]
			Pstack.clear();
			Qstack = R;
			while (!Qstack.empty())
			{
				if (Qstack[0] != "Exist")
				{
					Pstack.push_back(Qstack[0]);
					Qstack.erase(Qstack.begin());
				}
				else
				{
					Qstack.erase(Qstack.begin());

					logic.push_back("[");
					logic.push_back("Forall");
					logic.push_back(Qstack[0]);
					logic.push_back("[");
					logic.push_back("!");
					logic.push_back("[");
					for (int i = 1; i < (signed)(Qstack.size() - Pstack.size()); i++)
						logic.push_back(Qstack[i]);
					logic.push_back("]");
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = R.size(); i < Rstack.size(); i++)
						logic.push_back(Rstack[i]);

					cout << "# drive in NOT !Exist" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					Ostack.clear();
					Rstack = logic;

					drivein = true;
					break;
				}
			}
			if (drivein)
				continue;

			// ! [Forall x Q] ~  [ Exist x [ ! [Q] ] ]
			Pstack.clear();
			Qstack = R;
			while (!Qstack.empty())
			{
				if (Qstack[0] != "Forall")
				{
					Pstack.push_back(Qstack[0]);
					Qstack.erase(Qstack.begin());
				}
				else
				{
					Qstack.erase(Qstack.begin());

					logic.push_back("[");
					logic.push_back("Exist");
					logic.push_back(Qstack[0]);
					logic.push_back("[");
					logic.push_back("!");
					logic.push_back("[");
					for (int i = 1; i < (signed)(Qstack.size() - Pstack.size()); i++)
						logic.push_back(Qstack[i]);
					logic.push_back("]");
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = R.size(); i < Rstack.size(); i++)
						logic.push_back(Rstack[i]);

					cout << "# drive in NOT !Forall" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					Ostack.clear();
					Rstack = logic;

					drivein = true;
					break;
				}
			}
			if (drivein)
				continue;

			// ! [ P | Q ] ~ [ ! [P] & ! [Q] ]
			Pstack.clear();
			Qstack = R;
			while (!Qstack.empty())
			{
				if (Qstack[0] != "|")
				{
					Pstack.push_back(Qstack[0]);
					Qstack.erase(Qstack.begin());
				}
				else
				{
					Qstack.erase(Qstack.begin());

					vector<string> P = getBinaryOperandLF(Pstack);
					vector<string> Q = getBinaryOperandRT(Qstack);

					if (P.size() + Q.size() + 3 != R.size())
					{
						cout << "###" << endl;
						cout << "R: ";
						for (unsigned int i = 0; i < R.size(); i++)
							cout << R[i] << " ";
						cout << endl;
						break;
					}

					for (int i = 0; i < (signed)(Pstack.size() - P.size() - 1); i++)
						logic.push_back(Pstack[i]);
					logic.push_back("[");
					logic.push_back("!");
					logic.push_back("[");
					for (unsigned int i = 0; i < P.size(); i++)
						logic.push_back(P[i]);
					logic.push_back("]");
					logic.push_back("&");
					logic.push_back("!");
					logic.push_back("[");
					for (unsigned int i = 0; i < Q.size(); i++)
						logic.push_back(Q[i]);
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = Q.size(); i < Qstack.size() - 1; i++)
						logic.push_back(Qstack[i]);
					for (unsigned int i = R.size(); i < Rstack.size(); i++)
						logic.push_back(Rstack[i]);

					cout << "# drive in NOT ![P|Q]" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					Ostack.clear();
					Rstack = logic;

					drivein = true;
					break;
				}
			}
			if (drivein)
				continue;

			// ! [ P & Q ] ~ [ ! [P) | ! [Q] ]
			Pstack.clear();
			Qstack = R;
			while (!Qstack.empty())
			{
				if (Qstack[0] != "&")
				{
					Pstack.push_back(Qstack[0]);
					Qstack.erase(Qstack.begin());
				}
				else
				{
					Qstack.erase(Qstack.begin());

					vector<string> P = getBinaryOperandLF(Pstack);
					vector<string> Q = getBinaryOperandRT(Qstack);

					if (P.size() + Q.size() + 3 != R.size())
					{
						cout << "###" << endl;
						cout << "R: ";
						for (unsigned int i = 0; i < R.size(); i++)
							cout << R[i] << " ";
						cout << endl;
						break;
					}

					for (int i = 0; i < (signed)(Pstack.size() - P.size() - 1); i++)
						logic.push_back(Pstack[i]);
					logic.push_back("[");
					logic.push_back("!");
					logic.push_back("[");
					for (unsigned int i = 0; i < P.size(); i++)
						logic.push_back(P[i]);
					logic.push_back("]");
					logic.push_back("|");
					logic.push_back("!");
					logic.push_back("[");
					for (unsigned int i = 0; i < Q.size(); i++)
						logic.push_back(Q[i]);
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = Q.size(); i < Qstack.size() - 1; i++)
						logic.push_back(Qstack[i]);
					for (unsigned int i = R.size(); i < Rstack.size(); i++)
						logic.push_back(Rstack[i]);

					cout << "# drive in NOT ![P&Q]" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					Ostack.clear();
					Rstack = logic;

					drivein = true;
					break;
				}
			}
			if (drivein)
				continue;

			// ! [! P] ~ [P]
			Pstack.clear();
			Qstack = R;
			while (!Qstack.empty())
			{
				if (Qstack[0] != "!")
				{
					Pstack.push_back(Qstack[0]);
					Qstack.erase(Qstack.begin());
				}
				else
				{
					Qstack.erase(Qstack.begin());

					logic.push_back("[");
					logic.push_back("[");
					for (int i = 0; i < (signed)(Qstack.size() - Pstack.size()); i++)
						logic.push_back(Qstack[i]);
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = R.size(); i < Rstack.size(); i++)
						logic.push_back(Rstack[i]);

					cout << "# drive in NOT ![!P]" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					Ostack.clear();
					Rstack = logic;

					drivein = true;
					break;
				}
			}
			if (drivein)
				continue;
		}
	}
}

void CNF_StandardizeVariable(vector<string> &logic)
{
	// rename variables apart

	cout << "###### standardize variable start" << endl << endl;

	int quntifier = 0;
	for (unsigned int i = 0; i < logic.size(); i++)
	if (logic[i] == "Exist" || logic[i] == "Forall")
		quntifier++;

	if (quntifier <= variable.size())
		return;
	else
	{
		int add = quntifier - variable.size();
		variable.resize(quntifier);
		for (int c = 'A' + 25; add > 0; c--)
		{
			unsigned int i = 0;
			for (; i< variable.size(); i++)
			if (variable[i][0] == (char)c)
				break;

			if (i == variable.size())
			{
				variable[variable.size() - add] = (char)c;
				add--;
			}
		}
	}

	int q = 0;
	for (unsigned int i = 0; i < logic.size(); i++)
	{
		if (logic[i] == "Exist" || logic[i] == "Forall")
		{
			string var = logic[i + 1];
			logic[i + 1] = variable[q];

			vector<string> Rstack(logic.begin() + i + 2, logic.end());
			vector<string> R = getUnaryOperandRT(Rstack);

			for (unsigned int j = i + 2; j < i + 2 + R.size(); j++)
			{
				for (unsigned int c = 1; c < logic[j].size(); c++)
				{
					if (logic[j][c] == var[0] && (logic[j][c - 1] == '(' || logic[j][c + 1] == ')' || (logic[j][c - 1] == ',' && logic[j][c + 1] == ',')))
						logic[j][c] = variable[q][0];
				}
			}

			q++;
			i++;

			cout << "# standardize variable" << endl;
			for (unsigned int j = 0; j < logic.size(); j++)
				cout << logic[j] << " ";
			cout << endl << endl;
		}
	}
}

void CNF_EXIST(vector<string> &logic)
{
	// remove EXIST
	// Exist X Exist Y [F(X,Y)] ~ [F($v1,$v2)]

	cout << "###### remove EXIST start" << endl << endl;

	int q = 49;
	for (unsigned int i = 0; i < logic.size(); i++)
	{
		if (logic[i] == "Exist")
		{
			string append;

			for (unsigned int j = 1; j < i; j++)
			if (logic[j - 1] == "Forall")
				append.append("."+logic[j]);

			string var = logic[i + 1];

			vector<string> Rstack(logic.begin() + i + 2, logic.end());
			vector<string> R = getUnaryOperandRT(Rstack);

			for (unsigned int j = i + 2; j < i + 2 + R.size(); j++)
			{
				for (unsigned int c = 1; c < logic[j].size(); c++)
				{
					if (logic[j][c] == var[0] && (logic[j][c - 1] == '(' || logic[j][c + 1] == ')' || (logic[j][c - 1] == ',' && logic[j][c + 1] == ',')))
					{
						string newvar = "$v";
						newvar.push_back((char)q);
						newvar.append(append);

						logic[j].erase(logic[j].begin() + c);
						logic[j].insert(c, newvar);
					}
				}
			}

			logic.erase(logic.begin() + i, logic.begin() + i + 2);
			q++;
			i--;

			cout << "# remove EXIST" << endl;
			for (unsigned int j = 0; j < logic.size(); j++)
				cout << logic[j] << " ";
			cout << endl << endl;
		}
	}
}

void CNF_FORALL(vector<string> &logic)
{
	// remove FORALL
	// Forall [P] ~ [P]

	cout << "###### remove FORALL start" << endl << endl;

	for (unsigned int i = 0; i < logic.size(); i++)
	{
		if (logic[i] == "Forall")
		{
			logic.erase(logic.begin() + i, logic.begin() + i + 2);

			i--;

			cout << "# remove FORALL" << endl;
			for (unsigned int j = 0; j < logic.size(); j++)
				cout << logic[j] << " ";
			cout << endl << endl;
		}
	}
}

void CNF_Distribution(vector<string> &logic)
{
	// distribution OR over AND
	// P | [R & S] ~ [ [ [P] | [R] ] & [ [P] | [S] ] ]

	cout << "###### distribution OR over AND start" << endl << endl;

	vector<string> Pstack;
	vector<string> Qstack;
	vector<string> Rstack;
	vector<string> Sstack;

	Pstack.clear();
	Qstack = logic;
	while (!Qstack.empty())
	{
		if (Qstack[0] != "|")
		{
			Pstack.push_back(Qstack[0]);
			Qstack.erase(Qstack.begin());
		}
		else
		{
			Qstack.erase(Qstack.begin());

			vector<string> P = getBinaryOperandLF(Pstack);
			vector<string> Q = getBinaryOperandRT(Qstack);

			Rstack.clear();
			Sstack = Q;
			while (!Sstack.empty())
			{
				if (Sstack[0] != "&")
				{
					Rstack.push_back(Sstack[0]);
					Sstack.erase(Sstack.begin());
				}
				else
				{
					Sstack.erase(Sstack.begin());

					vector<string> R = getBinaryOperandLF(Rstack);
					vector<string> S = getBinaryOperandRT(Sstack);

					logic.clear();
					for (int i = 0; i < (signed)(Pstack.size() - P.size() - 1); i++)
						logic.push_back(Pstack[i]);

					logic.push_back("[");
					logic.push_back("[");
					logic.push_back("[");
					for (unsigned int i = 0; i < P.size(); i++)
						logic.push_back(P[i]);
					logic.push_back("]");
					logic.push_back("|");
					logic.push_back("[");
					for (unsigned int i = 0; i < R.size(); i++)
						logic.push_back(R[i]);
					logic.push_back("]");
					logic.push_back("]");
					logic.push_back("&");
					logic.push_back("[");
					logic.push_back("[");
					for (unsigned int i = 0; i < P.size(); i++)
						logic.push_back(P[i]);
					logic.push_back("]");
					logic.push_back("|");
					logic.push_back("[");
					for (unsigned int i = 0; i < S.size(); i++)
						logic.push_back(S[i]);
					logic.push_back("]");
					logic.push_back("]");
					logic.push_back("]");
					for (unsigned int i = Q.size(); i < Qstack.size(); i++)
						logic.push_back(Qstack[i]);

					Pstack.clear();
					Qstack = logic;

					cout << "# distribute P|[R&S]" << endl;
					for (unsigned int i = 0; i < logic.size(); i++)
						cout << logic[i] << " ";
					cout << endl << endl;

					break;
				}
			}
		}
	}
}

void removeBrackets(vector<string> &logic)
{
	// remove brackets

	cout << "###### remove brackets start" << endl << endl;

	vector<int> lfidx;
	vector<int> rtidx;

	lfidx.clear();
	rtidx.clear();
	for (int i = 0; i < logic.size(); i++)
	{
		if (logic[i] == "[")
		{
			lfidx.push_back(i);
		
			int bracket = 1;
			for (int j = i + 1; j < logic.size(); j++)
			{
				if (logic[j] == "[")
					bracket++;
				else if (logic[j] == "]")
					bracket--;

				if (bracket == 0)
				{
					rtidx.push_back(j);
					break;
				}
			}
		}
	}

	for (unsigned int i = 0; i < logic.size(); i++)
		cout << logic[i] << " ";
	cout << endl;
	for (unsigned int idx = 0; idx < lfidx.size(); idx++)
		cout << "(" << lfidx[idx] << "," << rtidx[idx] << ")";
	cout << endl;

	for (unsigned int idx = 0; idx < lfidx.size(); idx++)
	{
		if ((rtidx[idx] - lfidx[idx] <= 3) // [a] ~ a + [!a] ~ !a
			|| (idx < lfidx.size() - 1 && lfidx[idx + 1] == lfidx[idx] + 1 && rtidx[idx + 1] == rtidx[idx] - 1)) // [[A]] ~ [A]
		{
			cout << "(" << lfidx[idx] << "," << rtidx[idx] << ")";

			logic.erase(logic.begin() + rtidx[idx]);
			logic.erase(logic.begin() + lfidx[idx]);

			for (unsigned int i = 0; i < lfidx.size(); i++)
			{
				if (i == idx)
					continue;

				if (lfidx[i] > lfidx[idx] && lfidx[i] > rtidx[idx])
					lfidx[i] -= 2;
				else if (lfidx[i] > lfidx[idx])
					lfidx[i]--;
				else if (lfidx[i] > rtidx[idx])
					lfidx[i]--;

				if (rtidx[i] > lfidx[idx] && rtidx[i] > rtidx[idx])
					rtidx[i] -= 2;
				else if (rtidx[i] > lfidx[idx])
					rtidx[i]--;
				else if (rtidx[i] > rtidx[idx])
					rtidx[i]--;
			}

			lfidx.erase(lfidx.begin() + idx);
			rtidx.erase(rtidx.begin() + idx);

			idx--;

			cout << "# remove bracket" << endl;
			for (unsigned int i = 0; i < logic.size(); i++)
				cout << logic[i] << " ";
			cout << endl;
			for (unsigned int i = 0; i < lfidx.size(); i++)
				cout << "(" << lfidx[i] << "," << rtidx[i] << ")";
			cout << endl;
		}
	}

	// [[A | B] | C] ~ [A | B | C]
	for (unsigned int idx = 0; idx < lfidx.size(); idx++)
	{
		if (rtidx[idx] - lfidx[idx] > 5)
			continue;

		for (unsigned int l = lfidx[idx] + 1; l < rtidx[idx]; l++)
		{
			if (logic[l] == "|")
			{
				cout << "(" << lfidx[idx] << "," << rtidx[idx] << ")";

				logic.erase(logic.begin() + rtidx[idx]);
				logic.erase(logic.begin() + lfidx[idx]);

				for (unsigned int i = 0; i < lfidx.size(); i++)
				{
					if (i == idx)
						continue;

					if (lfidx[i] > lfidx[idx] && lfidx[i] > rtidx[idx])
						lfidx[i] -= 2;
					else if (lfidx[i] > lfidx[idx])
						lfidx[i]--;
					else if (lfidx[i] > rtidx[idx])
						lfidx[i]--;

					if (rtidx[i] > lfidx[idx] && rtidx[i] > rtidx[idx])
						rtidx[i] -= 2;
					else if (rtidx[i] > lfidx[idx])
						rtidx[i]--;
					else if (rtidx[i] > rtidx[idx])
						rtidx[i]--;
				}

				lfidx.erase(lfidx.begin() + idx);
				rtidx.erase(rtidx.begin() + idx);

				idx--;

				cout << "# remove bracket" << endl;
				for (unsigned int i = 0; i < logic.size(); i++)
					cout << logic[i] << " ";
				cout << endl;
				for (unsigned int i = 0; i < lfidx.size(); i++)
					cout << "(" << lfidx[i] << "," << rtidx[i] << ")";
				cout << endl;
				break;
			}
		}
	}
}

vector<string> getBinaryOperandLF(vector<string> Pstack)
{
	vector<string> invOperand;
	vector<string> Operand;

	int Pstack_lfbracket = 0;
	for (int i = Pstack.size() - 1; i >= 0; i--)
	{
		if (Pstack[i] == "[")
			Pstack_lfbracket++;
		else if (Pstack[i] == "]")
			Pstack_lfbracket--;

		if (Pstack_lfbracket == 1)
			break;

		invOperand.push_back(Pstack[i]);
	}

	for (unsigned int i = 0; i < invOperand.size(); i++)
		Operand.push_back(invOperand[invOperand.size() - 1 - i]);

	cout << "P: ";
	for (unsigned int i = 0; i < Operand.size(); i++)
		cout << Operand[i] << " ";
	cout << endl;

	return Operand;
}

vector<string> getBinaryOperandRT(vector<string> Qstack)
{
	vector<string> Operand;

	int Qstack_rtbracket = 0;
	for (unsigned int i = 0; i < Qstack.size(); i++)
	{
		if (Qstack[i] == "]")
			Qstack_rtbracket++;
		else if (Qstack[i] == "[")
			Qstack_rtbracket--;

		if (Qstack_rtbracket == 1)
			break;

		Operand.push_back(Qstack[i]);
	}

	cout << "Q: ";
	for (unsigned int i = 0; i < Operand.size(); i++)
		cout << Operand[i] << " ";
	cout << endl;

	return Operand;
}

vector<string> getUnaryOperandRT(vector<string> Rstack)
{
	vector<string> Operand;

	int Rstack_lfbracket = 0;
	int Rstack_rtbracket = 0;

	if (Rstack[0] == "[" || Rstack[0] == "Exist" || Rstack[0] == "Forall")
	{
		for (unsigned int i = 0; i < Rstack.size(); i++)
		{
			if (Rstack[i] == "[")
				Rstack_lfbracket++;
			else if (Rstack[i] == "]")
				Rstack_rtbracket++;

			Operand.push_back(Rstack[i]);

			if (Rstack_lfbracket == Rstack_rtbracket && Rstack_lfbracket != 0)
				break;
		}

		if (Rstack_lfbracket == 0)
		{
			Operand.clear();
			for (unsigned int i = 0, j = 0; i <= j && j < Rstack.size(); i++)
			{
				if (Rstack[i] == "!")
					j++;
				Operand.push_back(Rstack[i]);
			}
		}
	}
	else
	{
		Operand.push_back(Rstack[0]);
	}

	cout << "R: ";
	for (unsigned int i = 0; i < Operand.size(); i++)
		cout << Operand[i] << " ";
	cout << endl;

	return Operand;
}
