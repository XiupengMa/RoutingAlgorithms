//
//  main.cpp
//  DistanceVector
//
//  Created by Show on 11/5/14.
//  Copyright (c) 2014 Show. All rights reserved.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

//#define DEBUGING 1
#define UNREACHABLE -999


class RoutingTable{
private:
    int totalNodes;
    int **costs;
    int **physicalLinks;
    int **next;
    void updateSelf(int src){
        bool changed = false;
        for (int i = 1; i < totalNodes; i++) { // for each possible node
            int old = costs[src][i];
            costs[src][i] = physicalLinks[src][i];
            next[src][i] = physicalLinks[src][i] == UNREACHABLE ? UNREACHABLE : i;
            for (int j = 1; j < totalNodes; j++) {
                if (physicalLinks[src][j] != UNREACHABLE && costs[j][i] != UNREACHABLE && next[j][i] != src) { // for each direct neighbor
                    if (costs[src][j] != UNREACHABLE && costs[src][src] > physicalLinks[src][j] + costs[src][j]) {
                        cout<<"negative cycle detected!"<<endl;
                        exit(1);
                    }
                    if (costs[src][i] == UNREACHABLE || costs[src][i] > physicalLinks[src][j] + costs[j][i]) {
                        costs[src][i] = physicalLinks[src][j] + costs[j][i];
                        next[src][i] = j;
                        //cout<<"change to "<<src<<":"<<i<<":"<<costs[src][i]<<" through "<<j<<endl;
                    }
                }
            }
            if (old != costs[src][i]) {
                changed = true;
            }
        }
        if (changed) {
            //cout<<"inform neighbors"<<endl;
            informNeighbors(src);
        }
    }
    void informNeighbors(int src){
        for (int i = 1; i < totalNodes; i++) {
            if (src != i && physicalLinks[src][i] != UNREACHABLE) {
                updateSelf(i);
            }
        }
    }
public:
    RoutingTable(int n){
        totalNodes = n + 1;
        costs = new int *[totalNodes];
        next = new int *[totalNodes];
        physicalLinks = new int *[totalNodes];
        for (int i = 0; i < totalNodes; i++) {
            costs[i] = new int[totalNodes];
            next[i] = new int[totalNodes];
            physicalLinks[i] = new int[totalNodes];
        }
        for (int i = 0; i < totalNodes; i++) {
            for (int j = 0; j < totalNodes; j++) {
                costs[i][j] = i == j ? 0 : UNREACHABLE;
                physicalLinks[i][j] = i == j ? 0 : UNREACHABLE;
                next[i][j] = i == j ? i : UNREACHABLE;
            }
        }
    }
    ~RoutingTable(){
        for (int i = 0; i < totalNodes; i++) {
            delete [] costs[i];
            delete [] next[i];
            delete [] physicalLinks[i];
        }
        delete costs;
        delete next;
        delete physicalLinks;
    }
    void setCost(int src, int dest, int cost){
        physicalLinks[src][dest] = cost;
        physicalLinks[dest][src] = cost;
//        if (costs[src][dest] == UNREACHABLE) {
//            costs[src][dest] = cost;
//            next[src][dest] = dest;
//        }
        updateSelf(src);
        updateSelf(dest);
    }
    void printCostTable(){
        cout<<"Cost Table:"<<endl;
        for (int i = 1; i < totalNodes; i++) {
            for (int j = 1; j < totalNodes; j++) {
                cout<<costs[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    void printNextTable(){
        cout<<"Next Table:"<<endl;
        for (int i = 1; i < totalNodes; i++) {
            for (int j = 1; j < totalNodes; j++) {
                cout<<next[i][j]<<" ";
            }
            cout<<endl;
        }
    }
    
    string singleNodeTable(int src){
        string table;
        ostringstream oss(table);
        for (int i =1; i <totalNodes; i++) {
            oss<<i<<" "<<next[src][i]<<" "<<costs[src][i]<<endl;
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
    
//    void converge(){
//        for (int i = 1; i < totalNodes; i++) {
//            updateSelf(i);
//        }
//    }
    string getPath(int src, int dest){
        string result;
        ostringstream oss(result);
        int currentNode = src;
        while (currentNode != dest) {
            oss<<currentNode<<" ";
            currentNode = next[currentNode][dest];
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
        //rt->converge();
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
        //cout<<"Read:"<<source<<" "<<dest<<" "<<cost<<endl;
        rt->setCost(source, dest, cost);
        //rt->printCostTable();
        //rt->printNextTable();
    }
    //rt->converge();
    //rt->printCostTable();
    //rt->printNextTable();
#ifdef DEBUGING
    cout<<"Topology:"<<endl<<topoContent<<endl;
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
