#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cctype>
#include <set>
#include <map>
#include <algorithm>

using namespace std;

struct Vert {
    int a, b, n, l;
};

class Graph {
private:
    vector<Vert> edges;
    set<pair<int, int>> uniqueEdgesAB;
    set<pair<int, int>> uniqueEdgesBN;
    int lineNum;
public:
    int addEdge(const Vert& edge) {
        if (uniqueEdgesAB.count({ edge.a, edge.b }) > 0) {
            return -1;
        }
        if (!uniqueEdgesBN.insert({ edge.b, edge.n }).second) {
            return -2;
        }
        uniqueEdgesAB.insert({ edge.a, edge.b });
        edges.push_back(edge);
        lineNum = edge.l;
        return 1;
    }

    const vector<Vert>& getEdges() const {
        return edges;
    }
    const int getLineNumber() const {
        return lineNum;
    }

    int findSink() const {
        set<int> allVertices;
        set<int> outVertices;

        for (const auto& edge : edges) {
            allVertices.insert(edge.a);
            allVertices.insert(edge.b);
            outVertices.insert(edge.a);
        }

        for (int vertex : allVertices) {
            if (outVertices.find(vertex) == outVertices.end()) {
                return vertex;
            }
        }
        return -1;
    }

    int edgesCount() const {
        int maxElement = 1;
        for (const auto& edge : edges) {
            if (edge.a > maxElement)
                maxElement = edge.a;
            if (edge.b > maxElement)
                maxElement = edge.b;
        }
        return maxElement;
    }
};

void buildParents(const Graph& graph, vector <vector <int>>& parents, const vector<Vert>& edges) {
    for (const auto& edge : edges)
        parents[edge.b - 1].push_back(edge.a);
    return;
}

void printPrefix(vector<vector<int>>& parents, int startVertex, ofstream &outFile) {
    outFile << startVertex;
    vector<int> checkedVertex = parents[startVertex - 1];
    if (!checkedVertex.empty()) {
        outFile << "(";
        for (int i = 0; i < checkedVertex.size(); ++i) {
            printPrefix(parents, checkedVertex[i], outFile);
            if (i < checkedVertex.size() - 1)
                outFile << ",";
        }
        outFile << ")";
    }
}

void findPrefix(const Graph& graph, const string& fileName) {
    const vector<Vert>& edges = graph.getEdges();
    set<int> visitedVertices;

    int startVertex = graph.findSink();

    int maxEl = graph.edgesCount();
    vector<int> dop;
    vector <vector <int>> parents(maxEl, dop);
    buildParents(graph, parents, edges);

    ofstream outFile(fileName, ios::out | ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return;
    }

    printPrefix(parents, startVertex, outFile);

    outFile.close();
}

bool fileToStruct(const string& fileName, Graph& graph) {
    bool isGoodResult = true;
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return false;
    }

    string line;
    size_t lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;
        istringstream iss(line);

        char nextChar;
        bool findOpen = true;
        bool errorPrint1 = true;
        while (iss >> nextChar) {
            if (nextChar != '(' && findOpen) {
                if (errorPrint1) {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                }
                while (nextChar != '(') {
                    if (!(iss >> nextChar))
                        return false;
                }
            }
            errorPrint1 = false;
            findOpen = false;
            Vert edge;
            string a, b, n;
            if (nextChar == '(') {

                while (iss >> nextChar && isdigit(nextChar)) {
                    a += nextChar;
                }

                if (a == "0") {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  вершины 0 быть не может\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }
                if (nextChar == '\n') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    continue;
                }
                if (isalpha(nextChar)) {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  некорректные данные (введены буквы вместо цифр)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }
                if (nextChar != ',') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }
                iss >> ws >> nextChar;
                while (iss && isdigit(nextChar)) {
                    b += nextChar;
                    iss >> nextChar;
                }

                if (b == "0") {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  вершины 0 быть не может\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }
                if (nextChar == '\n') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    continue;
                }
                if (isalpha(nextChar)) {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  некорректные данные (введены буквы вместо цифр)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }

                if (nextChar != ',') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }

                iss >> ws >> nextChar;
                while (iss && nextChar != ')' && isdigit(nextChar)) {
                    n += nextChar;
                    iss >> nextChar;
                }

                if (isalpha(nextChar)) {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  некорректные данные (введены буквы вместо цифр)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }
                if (nextChar == '\n') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    continue;
                }
                if (nextChar != ')') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
                    findOpen = true;
                    continue;
                }

                if (!a.empty() && !b.empty() && !n.empty()) {
                    edge.a = stoi(a);
                    edge.b = stoi(b);
                    edge.n = stoi(n);
                    edge.l = lineNumber;
                    int errorCode = graph.addEdge(edge);
                    if (errorCode == -1) {
                        cout << "Ошибка в строке " + to_string(lineNumber) + ":  повторяющаяся дуга " + a + " -> " + b << "\n";
                        isGoodResult = false;
                    }
                    if (errorCode == -2) {
                        cout << "Ошибка в строке " + to_string(lineNumber) + ":  дуга с номером " + n + " в вершину " + b + " уже существует\n";
                        isGoodResult = false;
                    }
                }
            }
            else {
                cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                isGoodResult = false;
            }

            iss >> ws >> nextChar;
            if (nextChar != ',' && nextChar != '\n' && !iss.eof()) {
                cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                isGoodResult = false;
            }
            if (iss.eof())
                break;
        }
    }
    return isGoodResult;
}


bool checkNumbers(const Graph& graph, set<int>& uniqueA, set<int>& uniqueB) {
    const vector<Vert>& edges = graph.getEdges();
    map<int, set<int>> incomingEdges;

    for (const auto& edge : edges) {
        uniqueB.insert(edge.b);
        uniqueA.insert(edge.a);
        incomingEdges[edge.b].insert(edge.n);
    }
    for (const auto& vertex : uniqueA) {
        if (vertex > *max_element(uniqueB.begin(), uniqueB.end())) {
            cout << "Ошибка в строке " + to_string(graph.getLineNumber()) + ": Неправильная нумерация вершин. Номер вершины a больше количества вершин.\n";
            return false;
        }
    }

    for (const auto& entry : incomingEdges) {
        const auto& edgesSet = entry.second;
        for (int i = 1; i <= *max_element(edgesSet.begin(), edgesSet.end()); ++i) {
            if (edgesSet.find(i) == edgesSet.end()) {
                cout << "Ошибка в строке " + to_string(graph.getLineNumber()) + ":  неправильно заданы номера дуг\n";
                return false;
            }
        }
    }
    return true;
}

string findInStr(string const& str, int n) {
    if (str.length() < n) {
        return str;
    }
    return str.substr(0, n);
}

bool isCyclic(int vertex, const vector<Vert>& edges, set<int>& visited, set<int>& recStack) {
    visited.insert(vertex);
    recStack.insert(vertex);

    for (const auto& edge : edges) {
        if (edge.a == vertex) {
            int neighbor = edge.b;

            if (visited.find(neighbor) == visited.end()) {
                if (isCyclic(neighbor, edges, visited, recStack)) {
                    return true;
                }
            }
            else if (recStack.find(neighbor) != recStack.end()) {
                return true;
            }
        }
    }

    recStack.erase(vertex);
    return false;
}

bool checkCycle(const Graph& graph, vector<int> &cycleLines) {
    const auto& edges = graph.getEdges();
    set<int> visited;
    set<int> recStack;
    bool haveCycle = false;
    for (const auto& edge : edges) {
        int vertex = edge.a;
        int lineNum = edge.l;
        if (visited.find(vertex) == visited.end()) {
            if (isCyclic(vertex, edges, visited, recStack)) {
                cycleLines.push_back(lineNum);
                haveCycle = true;
            }
        }
    }
    if (haveCycle)
        return true;
    return false;
}


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    Graph graph;
    set<int> uniqueA;
    set<int> uniqueB;
    string input1, input2, output1, output2;
    for (int i = 0; argv[i]; i++)
    {
        string checkStr = string(argv[i]);
        if (checkStr.length() > 4) {
            string ifStr1 = findInStr(checkStr, 7);
            string ifStr2 = findInStr(checkStr, 8);
            string subStr = checkStr.substr(7, checkStr.length());
            string subStr2 = checkStr.substr(8, checkStr.length());
            if (ifStr1 == "input1=") {
                input1 = subStr;
            }
            if (ifStr1 == "input2=") {
                input2 = subStr;
            }
            if (ifStr2 == "output1=") {
                output1 = subStr2;
            }
            if (ifStr2 == "output2=") {
                output2 = subStr2;
            }
        }
    }
    bool isGoodResult = fileToStruct(input1, graph);
    if (!checkNumbers(graph, uniqueA, uniqueB)) {
        isGoodResult = false;
    }
    vector<int> cycleLines;
    if (checkCycle(graph, cycleLines)) {
        for (auto i: cycleLines)
            cout << "Ошибка в строке " << i << ":  в графе существует цикл\n";
        return 0;
    }
    if (isGoodResult) {
        findPrefix(graph, output1);
    }
    return 0;
}
