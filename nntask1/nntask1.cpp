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
};


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
        while (iss >> nextChar) {
            if (nextChar != '(' && findOpen) {
                cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                isGoodResult = false;
                while (nextChar != '(') {
                    iss >> nextChar;
                }
            }
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
                    continue;
                }
                if (nextChar != ',') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
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
                    continue;
                }

                if (nextChar != ',') {
                    cout << "Ошибка в строке " + to_string(lineNumber) + ":  неправильно задана компонента. Формат: (a, b, n)\n";
                    isGoodResult = false;
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

void writeVertices(const set<int>& elems, ofstream& outFile) {
    for (const auto& elem : elems) {
        outFile << "    <vertex>v" << elem << "</vertex>\n";
    }
}

void writeEdges(const vector<Vert>& edges, ofstream& outFile) {
    for (const auto& edge : edges) {
        outFile << "    <arc>\n";
        outFile << "        <from>v" << edge.a << "</from>\n";
        outFile << "        <to>v" << edge.b << "</to>\n";
        outFile << "        <order>" << edge.n << "</order>\n";
        outFile << "    </arc>\n";
    }
}

void graphToXML(const string& fileName, Graph& graph, set<int>& elems) {
    ofstream outFile(fileName, ios::out | ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return;
    }

    outFile << "<graph>\n";

    writeVertices(elems, outFile);

    const auto& edges = graph.getEdges();
    writeEdges(edges, outFile);

    outFile << "</graph>\n";

    outFile.close();
}

string findInStr(string const& str, int n) {
    if (str.length() < n) {
        return str;
    }
    return str.substr(0, n);
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
    if (isGoodResult) {
        uniqueA.insert(uniqueB.begin(), uniqueB.end());
        graphToXML(output1, graph, uniqueA);
    }
    return 0;
}
