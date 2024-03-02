/*
compile: g++ -std=c++17 -o essay-search.exe ./*.cpp
run: ./essay-search.exe data query.txt output.txt
*/


#define FILE_EXTENSION ".txt"
#include<fstream>
#include<string>
#include<cstring>
#include<vector>
#include<iostream>
#include<set>
#include<algorithm>
#include<chrono>
#include<unordered_map>
#define str_l 1024
#define word_l 128


using namespace std;

// Utility Func


struct Node
{
	struct Node* child[26];
	bool end;
};

class Trie
{	
public:
	Trie()
	{};
	Node* root;
};

Node *makenode() {
	Node *newn =  new Node;
	
	newn->end = false;
	
	for (int i = 0; i < 26; i++)
		newn->child[i] = NULL;
	
	return newn;
}

void insert(Node *root, Node* root_r, char* s) {
	Node *curr = root;
	Node *curr_r = root_r;
	int len = strlen(s);
	
		for (int i = 0; i < len; i++)
		{
			if(s[i] <= 90) s[i] += 32;
			if(s[len-1-i] <= 90) s[len-1-i] += 32;
			int index = s[i] - 'a';
			int index2 = s[len-1-i] - 'a';
			if (!curr->child[index])
				curr->child[index] = makenode();
		
			curr = curr->child[index];

			if (!curr_r->child[index2])
				curr_r->child[index2] = makenode();
		
			curr_r = curr_r->child[index2];
		}
		
	curr->end = true;
	curr_r->end = true;
}

bool search(Node *root, char* s, bool exact) {
	Node *curr = root;
	int len = strlen(s);
	
	for (int i = 0; i < len; i++)
	{
		//if(s[i] <= 90) s[i] += 32;
		int index = s[i] - 'a';
		if (!curr->child[index])
			return false;
	
		curr = curr->child[index];
	}

	if(exact)
		return (curr->end);
	else
		return true;
}


vector<char*> Titles;
vector<Trie*> content;
vector<Trie*> content_reverse;
int text_num;

unordered_map<string, vector<int>> dp;
unordered_map<string, vector<int>> dp2;
unordered_map<string, vector<int>> dp3;


vector<int> searchfor(char* s, bool exact, bool reverse) {
	vector<int> ans;
	if(!reverse) {
		for(int i=0; i<text_num; i++) {
			if(search(content[i]->root, s, exact)) {
				ans.push_back(i);
			}
		}
	}
	else {
		for(int i=0; i<text_num; i++) {
			if(search(content_reverse[i]->root, s, exact)) {
				//if(i==496) cout << s;
				ans.push_back(i);
			}
		}
	}

	return ans;
}


char* Getline(FILE* fi) 
{
	char* s = (char*)malloc(str_l);
	int a;
	if(a = fgets(s,str_l, fi) != NULL) {
		return s;
	}
	//string a;
	s[0] = '\0';
	return s;

}


// string parser : output vector of strings (words) after parsing
vector<char*> word_parse(vector<char*> tmp_string, Trie* posi, Trie* rev){
	vector<char*> parse_string;
	for(auto& word : tmp_string){
		char new_str[word_l] = {'\0'};
		int len = strlen(word);
		int ncount=0;
    	for(int i=0; i<len; i++){
			if(isalpha(word[i]))
				new_str[ncount++] = word[i];
		}
		parse_string.emplace_back(new_str);
		//printf("%s ", new_str);
		insert(posi->root, rev->root, new_str);
		//insert(rev->root, new_str, true);
		//free(new_str);
	}
	return parse_string;
}

vector<char*> split(const char* str, const string& delim) {
	vector<char*> res;
	if("" == str) return res;
	//先將要切割的字串從string型別轉換為char*型別
	char * strs = (char*)malloc(str_l) ; //不要忘了
	strcpy(strs, str);

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		//string s = p; //分割得到的字串轉換為string型別
		res.push_back(p); //存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}

vector<string> splits(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;
	//先將要切割的字串從string型別轉換為char*型別
	char * strs = new char[str.length() + 1] ; //不要忘了
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; //分割得到的字串轉換為string型別
		res.push_back(s); //存入結果陣列
		p = strtok(NULL, d);
	}

	return res;
}


int main(int argc, char *argv[])
{

    // INPUT :
	// 1. data directory in data folder
	// 2. number of txt files
	// 3. output route

    //auto t1 = chrono::high_resolution_clock::now();

    string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	// Read File & Parser Example
	//text_num = stoi(query);

	for(int i=0; ; i++) {
		text_num = i;
		char file[128] = {'\0'};
		char* title_name = (char*)malloc(str_l);
		char* tmp = (char*)malloc(str_l);
		FILE* fi;
		//fstream fi;
		vector<char*> tmp_string;

		char s[16] ={'\0'};
		sprintf(s,"%d.txt", i);
		strcpy(file, data_dir.c_str());
		strcat(file, s);

		// from data_dir get file ....
		// eg : use 0.txt in data directory
		fi = fopen(file, "r");
		
		if(!fi)
			break;

		Trie* posi = new Trie();
		Trie* rev = new Trie();
		posi->root = makenode();
		rev->root = makenode();

		// GET TITLENAME
		//getline(fi, title_name);
		title_name =  Getline(fi);

		// GET TITLENAME WORD ARRAY
		tmp_string = split(title_name, " ");

		vector<char*> title = word_parse(tmp_string, posi, rev);

		// for(auto &word : title){
		// 	cout << word << endl;
		// }

		// GET CONTENT LINE BY LINE
		//while(getline(fi, tmp)){
		while(true) {
			tmp = Getline(fi);
			if(tmp[0] == '\0')
				break;
			// GET CONTENT WORD VECTOR
			tmp_string = split(tmp, " ");

			// PARSE CONTENT
			vector<char*> content = word_parse(tmp_string, posi, rev);

			//for(auto &word : content){
			//	cout << word << endl;
			//}
			//......
		}
		//cout << title_name;
		Titles.push_back(title_name);
		content.push_back(posi);
		content_reverse.push_back(rev);

		// CLOSE FILE
		//fi.close();
		fclose(fi);
	}

	//auto t2 = chrono::high_resolution_clock::now();
	//auto ms_int = chrono::duration_cast<chrono::milliseconds>(t2 - t1);
	//std::cout << ms_int.count() << "ms\n";

	////////////////////////////////////////////////////////
	//Query
	FILE* fh;
	fh = fopen(query.c_str(), "r");
	
	char* tmp = (char*)malloc(str_l);
	vector<char*> tmp_string;
	//vector<int> ans;
	bool plus = false;
	bool minus = false;
	FILE* fo;
	fo = fopen(output.c_str(), "w");
	
	if(!fh)
		cout << "Can't open query\n";
	if(!fo)
		cout << "AAAAAAAAAAAAAA\n";

	while (true) {
		tmp = Getline(fh);
		if(tmp[0]  == '\0')
			break;
		tmp_string = split(tmp, " ");
		vector<int> ans(text_num+1);

		//vector<int> ans_vec(text_num+1);
		int ans_vec[10005] = {0};

		for(auto& s : tmp_string) {
			int len = strlen(s);
			char new_str[word_l] = {'\0'};
			int ncount=0;
				if(isalpha(s[0])) {
					for(int i=0; i<len; i++) {
						char c = s[i];
						if(isalpha(c)) {
							if(c <= 90) c += 32;
							new_str[ncount++] = c;
						}
					}

					bool same = false;
					string new_s = new_str;
					
					if(plus) {
						vector<int> tmp(text_num+1);
						auto it = dp.find(new_s);
						if(it != dp.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str,false,false);
							pair<string,vector<int>> a(new_s, tmp);
							dp.insert(a);
						}

						int tmp2[10005] = {0};
						for(auto c : tmp) {
							tmp2[c] = 1;
						}
						
						for(int i=0; i<text_num; i++) {
							if(ans_vec[i] == 1) {
								if(tmp2[i] == 0) {
									ans_vec[i] = 0;
								}
							}
						}
						plus = false;
					}
					else if(minus) {
						vector<int> tmp(text_num+1);

						auto it = dp.find(new_s);
						if(it != dp.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str,false,false);
							pair<string,vector<int>> a(new_s, tmp);
							dp.insert(a);
						}

						
						for(auto n : tmp) {
							ans_vec[n] = 1;
						}
						minus = false;
					}
					else {

						auto it = dp.find(new_s);
						if(it != dp.end()) {
								same = true;
								ans = it->second;
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}

						if(!same) {
							ans = searchfor(new_str,false,false);
							pair<string,vector<int>> a(new_s, ans);
							dp.insert(a);
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}
					}
				}
				else if(s[0] == '"'){
					for(int i=0; i<len; i++) {
						char c = s[i];
						if(isalpha(c)) {
							if(c <= 90) c += 32;
							new_str[ncount++] = c;
						}
					}

					bool same = false;
					string new_s = new_str;

					if(plus) {
						
						vector<int> tmp(text_num+1);

						auto it = dp2.find(new_s);
						if(it != dp2.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str,true,false);
							pair<string,vector<int>> a(new_s, tmp);
							dp2.insert(a);
						}

						int tmp2[10005] = {0};
						for(auto c : tmp) {
							tmp2[c] = 1;
						}
						
						for(int i=0; i<text_num; i++) {
							if(ans_vec[i] == 1) {
								if(tmp2[i] == 0) {
									ans_vec[i] = 0;
								}
							}
						}
						
						plus = false;
					}
					else if(minus) {
						
						vector<int> tmp(text_num+1);

						auto it = dp2.find(new_s);
						if(it != dp2.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str,true,false);
							pair<string,vector<int>> a(new_s, tmp);
							dp2.insert(a);
						}

						for(auto n : tmp) {
							ans_vec[n] = 1;
						}
						minus = false;
					}
					else {
						
						auto it = dp2.find(new_s);
						if(it != dp2.end()) {
								same = true;
								ans = it->second;
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}

						if(!same) {
							ans = searchfor(new_str,true,false);
							pair<string,vector<int>> a(new_s, ans);
							dp2.insert(a);
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}

					}
				}
				else if(s[0] == '*') {
					for(int i=0; i<len; i++) {
						char c = s[i];
						if(isalpha(c)) {
							if(c <= 90) c += 32;
							new_str[ncount++] = c;
						}
					}
					string new_str2 = new_str;
					reverse(new_str2.begin(), new_str2.end());
					char new_str3[word_l] = {'\0'};
					for(int j=0; j<new_str2.length(); j++)
						new_str3[j] = new_str2[j];

					bool same = false;

					if(plus) {
						vector<int> tmp(text_num+1);

						auto it = dp3.find(new_str2);
						if(it != dp3.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str3,false,true);
							pair<string,vector<int>> a(new_str2, tmp);
							dp3.insert(a);
						}
						
						int tmp2[10005] = {0};
						for(auto c : tmp) {
							tmp2[c] = 1;
						}
						
						for(int i=0; i<text_num; i++) {
							if(ans_vec[i] == 1) {
								if(tmp2[i] == 0) {
									ans_vec[i] = 0;
								}
							}
						}
						plus = false;
					}
					else if(minus) {
						vector<int> tmp(text_num+1);

						auto it = dp3.find(new_str2);
						if(it != dp3.end()) {
							same = true;
							tmp = it->second;
						}
						if(!same) {
							tmp = searchfor(new_str3,false,true);
							pair<string,vector<int>> a(new_str2, tmp);
							dp3.insert(a);
						}
						
						for(auto n : tmp) {
							ans_vec[n] = 1;
						}
						minus = false;
					}
					else {
						auto it = dp3.find(new_str2);
						if(it != dp3.end()) {
								same = true;
								ans = it->second;
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}

						if(!same) {
							ans = searchfor(new_str3,false,true);
							pair<string,vector<int>> a(new_str2, ans);
							dp3.insert(a);
							for(auto c : ans) {
								ans_vec[c] = 1;
							}
						}
						
					}
				}
				else if(s[0] == '+') {
					plus = true;
				}
				else if(s[0] == '/') {
					minus = true;
				}
		}

		/*if(ans.empty())
			fwrite("Not Found!\n", 1, 11, fo);
		else {
			for(auto& i : ans) {
				fwrite(Titles[i], 1, strlen(Titles[i]), fo);
			}
		}*/

		bool has_ans = false;
		for(int i=0; i<text_num; i++) {
			if(ans_vec[i] == 1) {
				fwrite(Titles[i], 1, strlen(Titles[i]), fo);
				has_ans = true;
			}
		}
		if(!has_ans)
			fwrite("Not Found!\n", 1, 11, fo);
	}

	//auto t3 = chrono::high_resolution_clock::now();
	//ms_int = chrono::duration_cast<chrono::milliseconds>(t3 - t2);

    /* Getting number of milliseconds as a double. */
    //chrono::duration<double, std::milli> ms_double = t3 - t1;

    //std::cout << ms_int.count() << "ms\n";
    //std::cout << ms_double.count() << "ms\n";
}


// 1. UPPERCASE CHARACTER & LOWERCASE CHARACTER ARE SEEN AS SAME.
// 2. FOR SPECIAL CHARACTER OR DIGITS IN CONTENT OR TITLE -> PLEASE JUST IGNORE, YOU WONT NEED TO CONSIDER IT.
//    EG : "AB?AB" WILL BE SEEN AS "ABAB", "I AM SO SURPRISE!" WILL BE SEEN AS WORD ARRAY AS ["I", "AM", "SO", "SURPRISE"].
// 3. THE OPERATOR IN "QUERY.TXT" IS LEFT ASSOCIATIVE
//    EG : A + B / C == (A + B) / C
