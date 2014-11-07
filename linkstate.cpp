//
//  main.cpp
//  linkstate
//
//  Created by Show on 11/4/14.
//  Copyright (c) 2014 Show. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;
//#define DEBUGING 1
#define UNREACHABLE -999

int smallOne(int x, int y){
    return x < y ? x : y;
}

class RoutingTable{
private:
    int id;
    int **costs;
    int **prefix;
    int **physicalCosts;
    int **physicalPrefix;
    int totalNodes;
    void convergeOneNode(int src){
        //cout<<"node "<<src<<endl;
        bool flag[totalNodes];
        int processedCount = 0;
        for (int i = 0; i < totalNodes; i++) {
            flag[i] = false;
        }
        while (processedCount < totalNodes) {
            //cout<<"count"<<processedCount<<endl;
            int min = INT16_MAX;
            int minIndex = -1;
            for (int i = 1; i < totalNodes; i++) {
                //cout<<"flag:"<<flag[i]<<" costs:"<< costs[src][i] << endl;
                if (flag[i] == false && costs[src][i] != UNREACHABLE && costs[src][i] < min) {
                    min = costs[src][i];
                    minIndex = i;
                }
            }
            if (minIndex == -1) {
                break;
            }
            for (int i = 1; i < totalNodes; i++) {
                if (flag[i] == 0 && costs[minIndex][i] != UNREACHABLE) {
                    if (costs[src][i] == UNREACHABLE) {
                        costs[src][i] = costs[src][minIndex] + costs[minIndex][i];
                        prefix[src][i] = getDirectSufix(minIndex,i);
                        //cout<<"new "<<src << "->" << i << costs[src][i] << endl;
                        continue;
                    }
                    int old = costs[src][i];
                    costs[src][i] = smallOne(costs[src][i],
                                             costs[src][minIndex] + costs[minIndex][i]);
                    prefix[src][i] = old == costs[src][i] ? prefix[src][i] : getDirectSufix(minIndex,i);
                    //cout<<"changed "<<src << "->" << i << " old:" << old << "new:" << costs[src][i] << endl;
                }
            }
            processedCount++;
            flag[minIndex] = 1;
        }
    }
    void copyPhysical(){
        for (int i = 0; i < totalNodes; i++) {
            for (int j = 0; j < totalNodes; j++) {
                costs[i][j] = physicalCosts[i][j];
                prefix[i][j] = physicalPrefix[i][j];
            }
        }
    }
    int getDirectSufix(int src, int dest){
        int currentNode = src;
        while (prefix[currentNode][dest] != currentNode) {
            currentNode = prefix[currentNode][dest];
        }
        return currentNode;
    }
public:
    RoutingTable(int n){
        totalNodes = n + 1;
        costs = new int *[totalNodes];
        for (int i = 0; i < totalNodes; i++) {
            costs[i] = new int[totalNodes];
        }
        prefix = new int *[totalNodes];
        for (int i = 0; i < totalNodes; i++) {
            prefix[i] = new int[totalNodes];
        }
        physicalCosts = new int *[totalNodes];
        for (int i = 0; i < totalNodes; i++) {
            physicalCosts[i] = new int[totalNodes];
        }
        physicalPrefix = new int *[totalNodes];
        for (int i = 0; i < totalNodes; i++) {
            physicalPrefix[i] = new int[totalNodes];
        }
        for (int i = 0; i < totalNodes; i++) {
            for (int j = 0; j < totalNodes; j++) {
                //costs[i][j] = i == j ? 0 : UNREACHABLE;
                //prefix[i][j] = i == j ? i : UNREACHABLE;
                physicalCosts[i][j] = i == j ? 0 : UNREACHABLE;
                physicalPrefix[i][j] = i == j ? i : UNREACHABLE;
            }
        }
    }
    ~RoutingTable(){
        for (int i = 0; i < totalNodes; i++) {
            delete [] costs[i];
            delete [] prefix[i];
            delete [] physicalCosts[i];
            delete [] physicalPrefix[i];
        }
        delete costs;
        delete prefix;
        delete physicalCosts;
        delete physicalPrefix;
    }
    
    int getCost(int src, int dest){
        return costs[src][dest];
    }
    
    void setCost(int src, int dest, int cost){
        physicalCosts[src][dest] = cost;
        physicalCosts[dest][src] = cost;
        physicalPrefix[src][dest] = cost == UNREACHABLE ? UNREACHABLE : src;
        physicalPrefix[dest][src] = cost == UNREACHABLE ? UNREACHABLE : dest;
    }
    
    void converge(){
        copyPhysical();
        for (int i = 1; i < totalNodes; i++) {
            convergeOneNode(i);
        }
    }
    
    string singleNodeTable(int src){
        string table;
        ostringstream oss(table);
        for (int i =1; i <totalNodes; i++) {
            oss<<i<<" "<<prefix[i][src]<<" "<<costs[src][i]<<endl;
        }
        return oss.str();
    }
    
    string routingTable(){
        string table;
        for (int i = 1; i < totalNodes; i++) {
            table.append(singleNodeTable(i));
            table.append("\n");
        }
        return table;
    }
    
    void printPrefixTable(){
        for (int i = 1; i < totalNodes; i++) {
            for (int j = 1; j < totalNodes; j++) {
                cout << prefix[i][j] << " ";
            }
            cout << endl;
        }
    }
    
    void printCostTable(){
        for (int i = 1; i < totalNodes; i++) {
            for (int j = 1; j < totalNodes; j++) {
                cout << costs[i][j] << " ";
            }
            cout << endl;
        }

    }
    
    //get path from src to dest, return total cost
    string getPath(int src, int dest){
        string result;
        ostringstream oss(result);
        vector<int> path;
        int currentNode = dest;
        while (currentNode != src) {
            currentNode = prefix[src][currentNode];
            path.push_back(currentNode);
        }
        for(vector<int>::iterator it = path.end() - 1; it >= path.begin(); it--){
            oss<<*it<<" ";
        }
        return oss.str();
    }
    
    string sendMsg(int src, int dest, string msg){
        string result;
        ostringstream oss(result);
        oss<<"from "<<src<<" to "<<dest<<" hops "<<getPath(src, dest)<<"message "<<msg;
        return oss.str();
    }
};

string sendMsgs(string msgContent, RoutingTable *rt){
    string result;
    //cout<<"send msgs:"<<endl;
    int source, dest;
    string msgLine, msg;
    istringstream msgIss(msgContent);
    while (getline(msgIss, msgLine)) {
        size_t msgStart = msgLine.find(" ",msgLine.find(" ",0) + 1) + 1;
        msg = msgLine.substr(msgStart);
        istringstream lineIss(msgLine);
        if(!(lineIss >> source >> dest)){
            cout<<"File Format Error!"<<endl;
            exit(1);
        }
        result.append(rt->sendMsg(source, dest, msg) + "\n");
        //cout<<rt->sendMsg(source, dest, msg)<<endl;
    }
    return result;
}

string makeChange(string changeContent, RoutingTable *rt, string msgContent){
    string result;
    string changeLine;
    int src, dest, cost;
    istringstream iss(changeContent);
    while (getline(iss, changeLine)) {
        istringstream changeIss(changeLine);
        if(!(changeIss>>src>>dest>>cost)){
            cout<<"File Format Error!"<<endl;
            exit(1);
        }
        rt->setCost(src, dest, cost);
        //cout<<"converging.."<<endl;
        rt->converge();
        //rt->printCostTable();
        //cout<<"prefix"<<endl;
        //rt->printPrefixTable();
        result.append(rt->routingTable());
        //cout<<"sending msg..."<<endl;
        result.append(sendMsgs(msgContent, rt) + "\n");
    }
    return result;
}

int main(int argc, const char * argv[]) {
    string topoFile;
    string msgFile;
    string changesFile;
    string outputFile;
    string topoContent;
    string msgContent;
    string changesContent;
    string outputContent;
    ifstream *infile;
    int nodeCount = -1;
    int changeCount = 0;
    int msgCount = 0;
    
    if (argc != 4) {
        cout << "usage: linkstate topo-le message-le changes-le" << endl;
        exit(1);
    }
    
    topoFile = string(argv[1]);
    msgFile = string(argv[2]);
    changesFile = string(argv[3]);
    outputFile = "output.txt";
    
    ofstream ofs(outputFile);
    
#ifdef DEBUGING
    cout << "topology: " << topoFile << endl;
    cout << "msg:" << msgFile << endl;
    cout << "changes" << changesFile << endl;
#endif
    
    //read nodes and initialize
    string line;
    infile = new ifstream(topoFile);
    while (std::getline(*infile, line))
    {
        int source, dest, cost;
        istringstream iss(line);
        if (!(iss>>source>>dest>>cost)) {
            cout<<"File Format Error!"<<endl;
            exit(1);
        }
        if (source > nodeCount) {
            nodeCount = source;
        }
        if (dest > nodeCount) {
            nodeCount = dest;
        }
        topoContent.append(line + "\n");
    }
    
    RoutingTable *rt = new RoutingTable(nodeCount);
    std::istringstream iss(topoContent);
    int source, dest, cost;
    while (iss >> source >> dest >> cost) {
        //cout<<source<<" "<<dest<<" "<<cost<<endl;
        rt->setCost(source, dest, cost);
    }
    rt->converge();
    //cout<<"cost table"<<endl;
    //rt->printCostTable();
    //cout<<"prefix table"<<endl;
    //rt->printPrefixTable();
#ifdef DEBUGING
    cout<<topoContent<<endl;
    //cout<<"Routing Table:"<<endl;
    //cout<<rt->routingTable();
    //cout<<endl;
#endif
    ofs << rt->routingTable();
    
    //read and send msgs
    infile = new ifstream(msgFile);
    while (std::getline(*infile, line))
    {
        msgContent.append(line + "\n");
        msgCount++;
    }
#ifdef DEBUGING
    cout<<msgContent<<endl;
#endif
    ofs<<sendMsgs(msgContent,rt)<<endl;
    

    //read changes
    infile = new ifstream(changesFile);
    while (std::getline(*infile, line))
    {
        changesContent.append(line + "\n");
        changeCount++;
    }
#ifdef DEBUGING
    cout<<changesContent<<endl;
#endif
    
    
    ofs<<makeChange(changesContent, rt, msgContent)<<endl;

    infile->close();
    ofs.close();
    
    return 0;
}
