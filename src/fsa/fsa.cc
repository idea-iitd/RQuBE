#include<bits/stdc++.h>
using namespace std;

#define sz(a) int((a).size())
#define pb push_back
#define all(c) (c).begin(),(c).end()
#define tr(c,i) for(typeof((c).begin()) i = (c).begin(); i != (c).end(); i++)
#define present(c,x) ((c).find(x) != (c).end())
#define cpresent(c,x) (find(all(c),x) != (c).end())

int k=1,maxStringLength=-1,minStringLength=INT_MAX;
unordered_map<string,int> prefix, sets;  //prefix = prefix to state number, sets = set to state number
int state_index=0;
unordered_set<int> finalStates;
unordered_set<string> alpha;

//Have to create "alpha,minStringLength,maxStringLength" each time, before calling construct

struct trieNode {
    unordered_map<string,trieNode*> children;
    vector<pair<int,string>> states;
    bool EOW;
};

trieNode* newNode(){
    trieNode* node=new trieNode;
    node->EOW=0;
    return node;
}

void insert(trieNode *root,vector<string> &s){
    for(long int i=0;i<s.size();i++){
        if(root->children.find(s[i])==root->children.end()){
            trieNode *child=newNode();
            if(i==s.size()-1)
                child->EOW=1;
            root->children[s[i]]=child;
        }
        else if(i==s.size()-1)
            root->children[s[i]]->EOW=1;
        root=root->children[s[i]];
    }
}

void print(trieNode *root){
    if(root==NULL)
        return;
    tr(alpha,i){
        if(!present(root->children,*i))
            continue;
        cout<<*i<<" "<<root->children[*i]->EOW<<" ";
        for(long int j=0;j<root->children[*i]->states.size();j++){
            cout<<"("<<root->children[*i]->states[j].first<<","<<root->children[*i]->states[j].second<<") ";
        }
        cout<<endl;
        print(root->children[*i]);
    }
}

void calPrefix(trieNode *root){
    if(root==NULL)
        return;
    tr(alpha,i){
        if(!present(root->children,*i))
            continue;
        calPrefix(root->children[*i]);
        for(long int j=0;j<root->children[*i]->states.size();j++){
            if(root->children[*i]->states[j].first+1<=k)
                if(root->children[*i]->states[j].second=="E")
                    root->states.pb(make_pair(root->children[*i]->states[j].first+1,*i));
                else
                    root->states.pb(make_pair(root->children[*i]->states[j].first+1,*i+"."+root->children[*i]->states[j].second)); 
        }
    }
    if(root->EOW==1)
        root->states.pb(make_pair(0,"E"));
    if(root->states.size()==0)
        root->states.pb(make_pair(2*k,"T"));
}

void findMap(trieNode *root, string pre){
    if(root==NULL)
       return;

    tr(root->children,i){
        string state_string ="";
        for(long int j =0; j< i->second->states.size(); j++){
            state_string += i->second->states[j].second;
            
            if(j<i->second->states.size()-1)
                state_string += ".";
        }
        if(state_string!="" && !present(sets,state_string)){
            sets.insert({state_string,state_index++});
        }
        if(state_string!="")
            prefix.insert(make_pair(pre +"." +i->first, sets[state_string]));
        findMap(i->second, pre+ "."+i->first);
    }
}

void findMapDriver(trieNode *root){
    if(root==NULL)
        return;
    
    string state_string ="";
    for(long int j =0; j< root->states.size(); j++){
        state_string += root->states[j].second;
        
        if(j<root->states.size()-1)
            state_string += ".";
    }
    if(state_string!="" && !present(sets,state_string))
        sets.insert({state_string,state_index++});
    if(state_string!="")
        prefix[""] = sets[state_string];

    findMap(root,"");
}

void findFinalStates(vector<vector<string>> &input){
    string str="";
    tr(input,i){
        string temp="";
        tr(*i, j)
            temp = temp + "."+*j;
        if(i->size()<=k)
            str=str+"."+temp;
        finalStates.insert(prefix[temp]);
    }
    if(str!="")
        str=str.substr(1,str.length()-1);
}

void transitions(vector<unordered_map<int,unordered_set<int>>> &transition_table){
    tr(prefix, i){
        tr(alpha, j){
            if(present(prefix,i->first+ "."+ *j))
                transition_table[i->second][stoi(*j)].insert(prefix[i->first+ "."+ *j]);
        } 
    }
}


bool isAccepted(vector<unordered_map<int,unordered_set<int>>> &transition_table, const vector<int> &path, int pos, int curState){    
    if(pos == path.size())
        if present(finalStates,curState) 
            return true;
        else
            return false;
    int s= path[pos];
    bool a  = false;
    if(present(transition_table[curState], s)){
        for(auto j = transition_table[curState][s].begin();j!=transition_table[curState][s].end();j++){
            a = a || isAccepted(transition_table, path,pos+1, *j);
            if(a)
                return a;
        }
    }
    return false;
}

void construct(vector<vector<string>>& input,int tail,vector<unordered_map<int,unordered_set<int>>>& transition_table){
    prefix.erase(prefix.begin(),prefix.end());
    sets.erase(sets.begin(),sets.end());
    finalStates.erase(finalStates.begin(),finalStates.end());
    state_index=0;
    k=tail;

    trieNode *root=newNode();
    tr(input,i)   // Insert each sequence in trie
        insert(root,*i);
    calPrefix(root);
    
    findMapDriver(root);
    findFinalStates(input);

    transition_table.resize(state_index,unordered_map<int,unordered_set<int>>(alpha.size()));
    transitions(transition_table);
}
