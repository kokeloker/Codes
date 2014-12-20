#include <iostream>
#include <vector>
#include <string>

using namespace std;

//the first row contain the rules, the next rows contains the results of applying that rule
string alfa_reglas[3][4] = {
	"(A∧B)"	,"~(A∨B)"	,"~(A→B)"	,"~~A",
	"A"		,"~A"		,"A"		,"A",
	"B"		,"~B"		,"~B"		,""
};
string beta_reglas[3][5] = {
	"(A∨B)"	,"~(A∧B)"	,"(A→B)",	"(A↔B)",	"~(A↔B)",
	"A"		,"~A"		,"~A",		"A,B",		"A,~B",
	"B"		,"~B"		,"B",		"~A,~B",	"~A,B"
};
string delta_reglas[3][2] = {
    "",    "",
    "",    ""
};
vector <string> getPremisas(string data){
	int pos = 0;
	int i;
	//splits the data and returns a vector
	vector <string> premisas;
	while (data.find(",")!=string::npos){
		premisas.push_back(data.substr(0,data.find(",")));
		data = data.substr(data.find(",")+1);
	}
	premisas.push_back(data);
	return premisas;
}

int checkParentesis(string data){
	int count = 0;
	int i;
	for (i=0; i<data.length(); i++){
		if (data[i] == '('){
			count++;
		}
		if (data[i] == ')'){
			count--;
		}
	}
	return count == 0;
}

vector<string> getOperators(string data){
	vector <string> v;
	//if data doesn't have a ( it means that there's no connector
	if (data.find("(")==string::npos){
		return v;
	}
	//stores the first ( and all at its left
	string inicio = data.substr(0,data.find("(")+1);
	//stores the last ) and all at its right
	string fin = data.substr(data.rfind(")"),data.length());
	//removes the first ( and all at its left
	string aux = data.substr(data.find("(")+1);
	//removes the last ) and all at its right
	aux = aux.substr(0,aux.rfind(")"));
	string conector[4] = {"↔","→","∧","∨"};
	int i;
	for (i=0; i<4; i++){
		v.clear();
		int pos = 0;
		//search a connector and splits the data, for example:
		//A↔B is splited into A, ↔, and B, where ↔ its the connector
		while ((pos=aux.find(conector[i],pos+1))!=string::npos){
			string A = aux.substr(0,pos);
			string B = aux.substr(pos+conector[i].length(),aux.length());
			//if A and B has balanced parenthesis, then A and B are a Well-formed formula
			if (checkParentesis(A) && checkParentesis(B)){
				v.push_back(inicio+"A"+conector[i]+"B"+fin);
				v.push_back(A);
				v.push_back(B);
				return v;
			}
		}	
	}
	v.clear();
	return v;
}

string replace (string old, string str, string data){
	//replaces all the ocurrences of old in data with str
	int pos = 0;
    while ((pos = data.find(old, pos)) != string::npos) {
         data.replace(pos, old.length(), str);
         pos += str.length();
    }
    return data;
}

int isClosed(string data){
	//check if this branch can be closed
	vector<string> v = getPremisas(data);
	int i,j;
	for (i=0;i<v.size()-1; i++){
		for (j=i+1;j<v.size(); j++){
			if (v[i].compare("~"+v[j])==0 || v[j].compare("~"+v[i])==0){
				return 0;
			}
		}
	}
	//validate that there's no operator left
	if (data.find("↔")!=string::npos || data.find("→")!=string::npos ||
		data.find("∧")!=string::npos || data.find("∨")!=string::npos){
		return -1;
	}
	//if none of the previous then its a open branch
	return 1;
}

string getNewData(vector<string> premisas){
	int i,j;
	//makes a new string with all the data of the vector, only data that are not duplicated
	string newdata = premisas[0];
	for (i=1;i<premisas.size(); i++){
		int valida = 1;
		vector<string> aux = getPremisas(newdata);
		for (j=0;j<aux.size(); j++){
			if (premisas[i].compare(premisas[j])==0){
				valida = 0;
			}
		}
		if (valida){
			newdata = newdata + "," + premisas[i];
		}
	}
	return newdata;
}

string swapAlphaValues(int i, int j, string old, vector<string> v){
	string tmp = alfa_reglas[i][j];
	tmp = replace(old,v[i],tmp);
	return tmp;	
}

string swapBetaValues(int i, int j, string old, vector<string> v){
	string tmp = beta_reglas[i][j];
	tmp = replace(old,v[i],tmp);
	tmp = replace("~~","",tmp);
	return tmp;	
}

string alphaRules(string data){
	int i,j;
	int alfa = 1;
	while (alfa){
		alfa = 0;
		vector <string> premisas = getPremisas(data);
		for (i=0; i<premisas.size(); i++){
			vector<string> v = getOperators(premisas[i]);
			if (v.size() > 0){
				for (j=0; j<4; j++){
					if (v[0].compare(alfa_reglas[0][j])==0){
						premisas[i] = swapAlphaValues(1,j,"A",v);
						premisas.push_back(swapAlphaValues(2,j,"B",v));
						alfa = 1;
					}
				}
			}
			premisas[i] = replace("~~","",premisas[i]);
		}		
		data = getNewData(premisas);
	}
	return data;
}

int rama(string data){
	int i,j;
	//apply all the alpha rules that can be done to this data
	data = alphaRules(data);
	//-1 need to apply more rules, 0 means closed, 1 is opend
	int closed = isClosed(data);
	if (closed == 1){
		return 0;
	}
	if (closed == -1){
		//here we need to apply some beta rules		
		vector <string> premisas = getPremisas(data);
		for (i=0; i<premisas.size(); i++){
			vector<string> v = getOperators(premisas[i]);
			if (v.size() > 0){
				for (j=0; j<5; j++){
					if (v[0].compare(beta_reglas[0][j])==0){
						string aux = premisas[i];
						premisas[i] = swapBetaValues(1,j,"A",v);
						premisas[i] = replace("B",v[2],premisas[i]);
						//makes a new branch with a recursive call
						if (rama(getNewData(premisas))==0){
							//if that branch is a close one then this
							//branchs is a close one too
							return 0;
						}						
						premisas[i] = swapBetaValues(2,j,"B",v);
						premisas[i] = replace("A",v[1],premisas[i]);
						if (rama(getNewData(premisas))==0){
							//if that branch is a close one then this
							//branchs is a close one too
							return 0;
						}
					}
				}
			}
		}		
	}
    return 1;
}

int isValidData(string data){
	int i;
	int operators = 0;
	int p = 0;
	//to be valid, there must be a pair of () for every connector
	string conector[4] = {"↔","→","∧","∨"};
	string parentesis[2] = {"(",")"};
	for (i=0; i<4; i++){
		int pos = 0;
		while ((pos=data.find(conector[i],pos+1))!=string::npos){
			operators++;
		}
	}
	data = " "+data+" ";
	for (i=0; i<2; i++){
		int pos = 0;
		while ((pos=data.find(parentesis[i],pos+1))!=string::npos){
			p++;
		}
	}
	return (operators*2) == p;
}

string prepareData(string data){
	int i;
	string oldConector[6] = {" ","¬","<->","->","^","v"};
	string newConector[6] = {"","~","↔","→","∧","∨"};
	for (i=0; i<6; i++){
		data = replace(oldConector[i],newConector[i],data);	
	}	
	return data;
}

string unPrepareData(string data){
	data = replace(" ","",data);
	int i;
	string oldConector[4] = {"↔","→","∧","∨"};
	string newConector[4] = {"<->","->","^","v"};
	for (i=0; i<4; i++){
		data = replace(oldConector[i],newConector[i],data);	
	}	
	return data;
}

int main(){
	//string data = "(~p->(q^r)),~(q->p)";
	//string data = "~(~(~(p->q)->(q->p))->(~(p->q)->(q->p)))";
	//string data = "(p->r),(q->r),~((pvq)->r)";
	//string data = "(((pvq)<->~(pvq))v((~pvq)->(~((q^r)->~p)^(r->~(qvp)))))";
	//string data = "((pvq)->(rvs)),((r^t)->s),((r^¬t)->¬u),¬(p->(sv¬u))";
	string data = "((pvq)<->¬r),(¬p->s),(¬t->q),((s^t)->u),¬(r->u)";

	//replaces simbols and blank spaces
	data = prepareData(data);

	//validates if this data is a "well-formed formula" without any exception like "p^q->r" or "(p^q)->r"
	//((p^q)->r) is a well-formed formula and its a valid data for this program
	if (!isValidData(data)){
		cout << "Expresion no valida." << endl;
		return 0;
	}
	//executes the semantic board
	if (rama(data)){
		//replaces simbols for a better view on console
		cout << unPrepareData(data) << " es consecuencia logica" << endl;	
	} else{
		cout << unPrepareData(data) << " no es consecuencia logica" << endl;	
	}	
	return 0;
}