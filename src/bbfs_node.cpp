#include "include/ARRIVAL/parseRegEx/regexToAutomata.cc"
#include "fsa/fsa.cc"

#ifndef random_H
#define random_H
#include "include/ARRIVAL/random/random.cc"
#endif

#ifndef Graph_H
#define Graph_H
#include "include/ARRIVAL/Graph/Graph.cc"
#endif

#ifdef __MACH__
#include <mach/mach.h>
#endif

#include<bits/stdc++.h>

#define all(c) (c).begin(),(c).end()
#define tr(c,i) for(typeof((c).begin()) i = (c).begin(); i != (c).end(); i++)
#define present(c,x) ((c).find(x) != (c).end())
#define cpresent(c,x) (find(all(c),x) != (c).end())

template<class A, class B, class C>
class triple{
    public:
        A first;
        B second;
        C third;
        triple(A a,B b,C c){
            first=a;
            second=b;
            third=c;
        }
};

inline int findLabel(Graph *g,int v){
    if(g->nodes[v]->labels.size()==1) return -1;
	return g->nodes[v]->labels[1];
}

void horizonFWD(Graph *g,int src,int horizon,unordered_map<int,set<vector<int>>>& fwd){
    queue<vector<int>> q;
    q.push({src});
    while(!q.empty()){

        auto t = q.front();
        q.pop();
        fwd[t.back()].insert(t);
        if(t.size()>=horizon)
            continue;

        Node *currNode = g->nodes[t.back()];
        for(int i=0; i<currNode->allFwdEdges.size();i++){
            if(find(t.begin(),t.end(),currNode->allFwdEdges[i])!=t.end())  // SIMPLE PATH CHECK
                continue;
            t.push_back(currNode->allFwdEdges[i]);
            q.push(t);
            t.pop_back();
        }
    }
}

void horizonBWD(Graph *g,int dest,int horizon,unordered_map<int,set<vector<int>>>& fwd,map<vector<int>,int>& pathStrings){
    set<vector<int>> distPaths;
    queue<pair<vector<int>,unordered_set<int>>> q;
    q.push({{dest},{dest}});

    while(!q.empty()){
        auto t = q.front();
        q.pop();
        if(fwd.find(t.first.back())!=fwd.end()){
            int temp=t.first.back();
            t.first.pop_back();
            for(auto i:fwd[temp]){
                bool flag=0;
                for(auto j:i){
                    if(j!=temp && t.second.find(j)!=t.second.end()){
                        flag=1;
                        break;
                    }
                }
                if(flag)
                    continue;
                copy(t.first.rbegin(),t.first.rend(),back_inserter(i));
                auto it = distPaths.insert(i);
                if(it.second){
                    vector<int> path;
                    for(auto j:i)
                        path.push_back(findLabel(g,j));
                    pathStrings[path]++;
                }
            }
            t.first.push_back(temp);
        }

        if(t.first.size()>=horizon)
            continue;

        Node *currNode = g->nodes[t.first.back()];

        for(int i=0; i<currNode->allBwdEdges.size();i++){
            if(find(t.second.begin(),t.second.end(),currNode->allBwdEdges[i])!=t.second.end())  // SIMPLE PATH CHECK
                continue;
           t.first.push_back(currNode->allBwdEdges[i]);
           t.second.insert(currNode->allBwdEdges[i]);
           q.push(t);
           t.first.pop_back();
           t.second.erase(currNode->allBwdEdges[i]);
        }
    }

    // for(auto i:distPaths){
    //     for(auto j:i)
    //         cout<<j<<" ";
    //     cout<<endl;
    // }
}

void searchAllDestinations(int src, Graph *g,vector<unordered_map<int,unordered_set<int>>> &transition_table,unordered_map<int,int> &destinations,int searchDepth){

    queue<pair<triple<int,int,int>,unordered_set<int>>> q;  // triple = node,state,length
    int tempLabel=findLabel(g,src);
    if(present(transition_table[0],tempLabel)){
        tr(transition_table[0][tempLabel],j){
            q.push({triple<int,int,int>(src,*j,0),{src}});
        }
    }
    else return;

    while(q.size()!=0){
        pair<triple<int,int,int>,unordered_set<int>> temp = q.front();
        
        q.pop();
        if(temp.first.third >= searchDepth)   // Exceeded search depth
            continue;
        
        if(present(finalStates,temp.first.second)){   // Final state
            destinations[temp.first.first]++;
        }

        Node *currNode = g->nodes[temp.first.first];
        for(int i =0; i < currNode->allFwdEdges.size(); i++){
            int d = currNode->allFwdEdges[i];
            if(temp.second.find(d)!=temp.second.end())  // Simple path
                continue;
            int label = findLabel(g,d);
            if(present(transition_table[temp.first.second],label)){
                tr(transition_table[temp.first.second][label],j){
                    temp.second.insert(d);
                    q.push({triple<int,int,int>(d,*j,temp.first.third+1),temp.second});
                    temp.second.erase(d);
                }
            }
        }
        if(destinations.size()>50000)
            break;
    }
}

bool sortByValDec(const pair<int, float> &a, const pair<int, float> &b){
    return (a.second > b.second);
}

void store_nfa(vector<unordered_map<int,unordered_set<int>>> &transition_table,string fn){
    ofstream myOutFile(fn+".nfa");
    for(auto i:transition_table){
        for(auto j:i){
            myOutFile<<j.first<<" ";
            for(auto k:j.second)
                myOutFile<<k<<",";
            myOutFile<<" ";
        }
        myOutFile<<endl;
    }

    myOutFile<<"\nFINAL STATES"<<endl;
    for(auto i:finalStates)
        myOutFile<<i<<",";
    myOutFile<<endl;

    myOutFile.close();
}

void load_nfa(vector<unordered_map<string, set<long int>>> &transition_table,string fn){
    ifstream file(fn+".nfa");
    string line,word;
    int count_states=0;
    while (getline(file, line)){
        if(line=="FINAL STATES")
            break;
        stringstream s1(line);
        unordered_map<string,set<long int>> temp;
        while(getline(s1,word, ' ')){
            string transit,c;
            getline(s1,transit,' ');
            stringstream s2(transit);

            while(getline(s2,c,','))
                temp[word].insert(stoi(c));
        }
        transition_table.push_back(temp);
    }
    getline(file,line);
    stringstream s1(line);
    while(getline(s1,word, ',')){
        finalStates.insert(stoi(word));
    }
}

pair<double,int> initFSA(Graph *g,vector<vector<string>> &train,vector<vector<int>> &validation,vector<vector<string>> &input,int src,int dest,int horizon,unordered_map<int,set<vector<int>>>& fwd,string res,int store=0)
{
    auto start = chrono::system_clock::now();
    double temp=0;
    vector<unordered_map<int,unordered_set<int>>> transition_table;
    int tail;
    bool flag=1;
	for(tail=maxStringLength;tail>1 && !validation.empty();tail--){
        construct(train,tail,transition_table);
        double acc=0;
        for(auto j:validation)
            if(isAccepted(transition_table,j,0,0))
                acc++;
        if((acc/validation.size())>=0.9){
            flag=0;
            break;
        }
        transition_table.clear();
    }

    transition_table.clear();
    construct(input,tail,transition_table);
    
    unordered_map<int,int> destinations;
    searchAllDestinations(src,g,transition_table,destinations,4);
    if(destinations.size()<1){
        return {0.0,1};
    }
    cout<<destinations.size()<<" SIZE"<<endl;
    vector<pair<int, int>> destinationsVec;
    vector<triple<int,int,int>> all_dest_acc;
    tr(destinations, i){
        destinationsVec.push_back(make_pair(i->first, i->second));
    }
    sort(destinationsVec.begin(), destinationsVec.end(), sortByValDec);
    auto end = std::chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end-start;
    temp+=elapsed_seconds.count();

    ofstream myOutFile(res+".csv");
    myOutFile<<src<<","<<dest<<","<<flag<<","<<tail<<endl;

    start = std::chrono::system_clock::now();
    int max_accepted_strings=0;
    for(int i=0; i<min(INT_MAX,(int)destinationsVec.size()); i++){
        map<vector<int>,int> pathStrings;
        horizonBWD(g,dest,(horizon/2)+(horizon%2),fwd,pathStrings);

        int acceptedCount=0;
        int totalCount=0;
        for(auto j:pathStrings){
            totalCount+=j.second;
            if(isAccepted(transition_table,j.first,0,0))
                acceptedCount+=j.second;
        }
        end = chrono::system_clock::now();
        elapsed_seconds = end-start;
        temp+=elapsed_seconds.count();

        myOutFile<<destinationsVec[i].first;
        if(totalCount!=0){
            myOutFile<<","<<acceptedCount/(float)totalCount;
            myOutFile<<","<<acceptedCount;
            myOutFile<<","<<totalCount<<endl;
            all_dest_acc.push_back(triple<int,int,int>(destinationsVec[i].first,acceptedCount,acceptedCount/(float)totalCount));
        }

        start = chrono::system_clock::now();
        if(max_accepted_strings<acceptedCount)
            max_accepted_strings=acceptedCount;
    }
    myOutFile.close();
    return {temp,0};
}

int main(int argc, char *argv[])
{
    // Creates random exemplar pairs and stores them in a csv file almog with their neighbours and their actual confidence scores
    char *edgeFile = argv[1];
    char *labelFile = argv[2];
    string res = argv[3];              // Directory where to put these csv files
    int st = atoi(argv[4]);            // start number of example to create
    int en = atoi(argv[5]);            // end example number
    int horizon = atoi(argv[6]);       // Path Length

    string attrFile = "include/files/att.txt";
    int dir=1;      // Change horizon according to the length required in the forward and backward walk
    Graph *newG = new Graph(edgeFile, labelFile, &attrFile[0], dir);
    cout<<newG->walkLength<<" "<<newG->numWalks<<endl;
    Random *rands = new Random(newG->numEdges, 1);
    srand(time(NULL));
    double time=0;
    while(st<en){
        maxStringLength = INT_MIN;
        minStringLength = INT_MAX;
        alpha.clear();

        int src = 1+rand()%newG->nodes.size();
        int dest = 1+rand()%newG->nodes.size();
        if(src==dest || dest==newG->nodes.size() || src==newG->nodes.size())
            continue;

        auto start = chrono::system_clock::now();
        map<vector<int>,int> pathStrings;
        unordered_map<int,set<vector<int>>> fwd;
        horizonFWD(newG,src,(horizon/2),fwd);
        horizonBWD(newG,dest,(horizon/2)+(horizon%2),fwd,pathStrings);
        vector<vector<string>> input,train;
        vector<vector<int>> train_check,validation;
        int totalPaths=0,n=pathStrings.size();
        for(auto i:pathStrings){
            vector<string> temp;
            bool flag=1;
            totalPaths += i.second;
            for(auto j:i.first){
                temp.push_back(to_string(j));
                if(alpha.find(to_string(j))==alpha.end())
                    flag=0;
                alpha.insert(to_string(j));
            }
            int len=temp.size();
            maxStringLength = max(len,maxStringLength);
            minStringLength = min(len,minStringLength);
            input.push_back(temp);
            if(flag && validation.size()<(n*0.2))               // 80-20 split
                validation.push_back(i.first);
            else train.push_back(temp);
        }

        cout<<"input "<<input.size()<<" "<<totalPaths<<endl;
        if(input.size()<3){
           cout<<"size less than 3"<<endl;
            continue;
	    }
        // random_shuffle(v.begin(), v.end());

        auto end = std::chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end-start;
        double temp1=elapsed_seconds.count();
        auto temp2 = initFSA(newG,train,validation,input,src,dest,horizon,fwd,res+"/"+to_string(st));
        if(temp2.second){
            cout<<"failed"<<endl;
	        continue;
	    }
        cout<<st<<" :DONE\n";
        st++;
        time+=temp1+temp2.first;
    }
    cout<<"TIME "<<time<<endl;

    return 0;
}

