#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <cctype>
#include <iomanip>
#include <algorithm>

using namespace std;

vector< vector< vector<double>>> jsonToVecs(const  string& jsonString, const string& filename) {
    vector< vector< vector<double>>> result;

    int pos = 1;

    while (pos < jsonString.length()) {
        pos = jsonString.find('[', pos);
        if (pos == string::npos) {
            break;
        }
        vector< vector<double>> svv;
        pos++;

        while (true) {
            pos = jsonString.find('[', pos);
            if (pos == string::npos) {
                break;
            }
            vector<double> sv;
            pos++;

            while (true) {
                double value;
                stringstream ss;

                pos = jsonString.find_first_of("0123456789.-]", pos);
                if (pos == string::npos || jsonString[pos] == ']') {
                    break;
                }

                ss << jsonString[pos];
                pos++;
                while (isdigit(jsonString[pos]) || jsonString[pos] == '.') {
                    ss << jsonString[pos];
                    pos++;
                }

                ss >> value;
                sv.push_back(value);
                if (jsonString[pos] == '[') {
                    cerr << "Файл " << filename << " имеет некорректное содержимое\n";
                    exit(EXIT_FAILURE);
                }
                pos = jsonString.find_first_of(",]", pos);

                if (pos == string::npos || jsonString[pos] == ']') {
                    break;
                }

                pos++;
            }

            svv.push_back(sv);
            pos++;
            while (jsonString[pos] == ' ' || jsonString[pos] == '\n')
                pos++;
            if (jsonString[pos] == ']') {
                break;
            }
            if (jsonString[pos] == '[') {
                cerr << "Файл " << filename << " имеет некорректное содержимое\n";
                exit(EXIT_FAILURE);
            }
            pos = jsonString.find(',', pos);
            pos++;
        }

        result.push_back(svv);
        pos++;
        if (pos == string::npos) {
            break;
        }
        while (jsonString[pos] == ' ' || jsonString[pos] == '\n') {
            pos++;
        }
        if (jsonString[pos] == ']') {
            break;
        }
        if (jsonString[pos] == '[') {
            cerr << "Файл " << filename << " имеет некорректное содержимое\n";
            exit(EXIT_FAILURE);
        }

        pos = jsonString.find(',', pos);
        pos++;
    }
    return result;
}

vector<double> parseVector(string line, string key) {
    vector<double> result;
    int startPos = line.find(key) + key.length();
    int endPos = line.find("]", startPos);
    string values = line.substr(startPos + 1, endPos - startPos);
    istringstream iss(values);
    double value;
    while (iss >> value) {
        result.push_back(value);
        if (iss.peek() == ',') {
            iss.ignore();
        }
    }
    return result;
}

class NNClass {
private:
    vector<int> nCount;
    vector<vector<vector<double>>> nnWeights;
public:
    NNClass(const vector<vector<vector<double>>>& vec);
    void nnTraining(const string& trainingFile, int n, const string& outputFile);
};

vector<vector<double>> matrTranspose(const vector<vector<double>> m) {
    int rows = m.size();
    int cols = m[0].size();
    vector<vector<double>> result(cols, vector<double>(rows, 0.0));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result[j][i] = m[i][j];
        }
    }
    return result;
}

vector< vector<double>> matrMultiplication(const  vector< vector<double>>& m1, const  vector< vector<double>>& m2) {
    int rows1 = m1.size();
    int cols1 = m1[0].size();
    int rows2 = m2.size();
    int cols2 = m2[0].size();
    vector< vector<double>> result(rows1, vector<double>(cols2, 0.0));
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            for (int k = 0; k < cols1; ++k) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return result;
}

NNClass::NNClass(const  vector< vector< vector<double>>>& vec) {
    this->nCount =  vector<int>();
    this->nnWeights =  vector< vector< vector<double>>>();

    for (const auto& layer : vec) {
         vector< vector<double>> weights;
        for (const auto& neuron : layer) {
            weights.push_back(neuron);
        }

        if (this->nCount.empty()) {
            this->nCount.push_back(weights[0].size());
        }
        else if (this->nCount.back() != weights[0].size()) {
            cerr << "Некорректное количество компонентов нейрона в слое\n";
            exit(EXIT_FAILURE);
        }

        this->nCount.push_back(weights.size());
        this->nnWeights.push_back(matrTranspose(weights));
    }
}

void NNClass::nnTraining(const  string& trainingFile, int n, const string& outputFile) {
    ifstream fs(trainingFile);
    if (!fs.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return;
    }

    pair <vector<double>, vector<double>> trData;
    vector<pair <vector<double>, vector<double>>> nnTrainingVec;
    string line;
    while (getline(fs, line)) {
        vector<double> file_i = parseVector(line, "\"i\":");
        vector<double> file_o = parseVector(line, "\"o\":");

        if (file_i.empty() || file_o.empty()) {
            cerr << "Некорректное содержимое файла: " << trainingFile << endl;
            exit(EXIT_FAILURE);
        }

        nnTrainingVec.push_back(make_pair(file_i, file_o));
    }

    fs.close();

    if (nnTrainingVec.size() == 0) {
        cerr << "Некорректное содержимое файла: " << trainingFile << endl;
        return;
    }

    int trainingDataIndex = 0;
    double k = 0.01;

    ofstream outFile(outputFile, ios::out, ios::trunc);
    if (!outFile.is_open()) {
        cerr << "Ошибка открытия файла\n";
        exit(EXIT_FAILURE);
    }
    outFile << setprecision(16);

    for (int iter = 1; iter <= n; iter++) {
        auto funcAct = [](double x) -> double {
            return x / (1 + abs(x));
        };
        auto funcActProizv = [](double x) -> double {
            double absX = abs(x);
            return 1.0 / ((1 + absX) * (1 + absX));
        };
        pair <double, double> nValue;
        vector< vector<pair <double, double>>> nValues;
        pair <vector<double>, vector<double>> thisTrainingData = nnTrainingVec[trainingDataIndex];
        vector<double> input = thisTrainingData.first;

        vector<pair <double, double>> layerNValues;
        for (double inp1 : input) {
            layerNValues.push_back(make_pair(inp1, inp1));
        }
        nValues.push_back(layerNValues);

        vector< vector<double>> resMult;
        resMult.push_back(input);

        for (int layerIndex = 0; layerIndex < nnWeights.size(); layerIndex++) {
            layerNValues.clear();
            resMult = matrMultiplication(resMult, nnWeights[layerIndex]);

            for (int i = 0; i < resMult[0].size(); i++) {
                pair <double, double> pairNVal;
                pairNVal.first = resMult[0][i];
                pairNVal.second = funcAct(pairNVal.first);
                resMult[0][i] = pairNVal.second;
                layerNValues.push_back(pairNVal);
            }
            nValues.push_back(layerNValues);
        }

        vector< vector< vector<double>>> weightUpdates;
        vector<double> nLayerErr;

        int i = nValues.size() - 1;
        for (int m = 0; m < nValues[i].size(); m++)
            nLayerErr.push_back((thisTrainingData.second[m] - nValues[i][m].second) * funcActProizv(nValues[i][m].first));

        vector< vector<double>> wu2;
        vector< vector<double>> wi = nnWeights[i - 1];

        for (int m = 0; m < wi[0].size(); m++) {
            vector<double> deltaWij;
            for (int z = 0; z < wi.size(); z++) {
                deltaWij.push_back(k * nLayerErr[m] * nValues[i - 1][z].second);
            }
            wu2.push_back(deltaWij);
        }
        weightUpdates.push_back(matrTranspose(wu2));

        for (int layerIndex = i - 1; layerIndex >= 1; layerIndex--) {
            wi = nnWeights[layerIndex];
            vector<double> nLayerErrIn;

            for (int m = 0; m < wi.size(); m++) {
                double nLayerErrIJ = 0;
                for (int z = 0; z < wi[m].size(); z++) {
                    nLayerErrIJ += nLayerErr[z] * wi[m][z];
                }
                nLayerErrIn.push_back(nLayerErrIJ);
            }

            nLayerErr.clear();

            for (int m = 0; m < nValues[layerIndex].size(); m++) {
                nLayerErr.push_back(nLayerErrIn[m] * funcActProizv(nValues[layerIndex][m].first));
            }

            wi = nnWeights[layerIndex - 1];
            wu2.clear();

            for (int i1 = 0; i1 < wi.size(); i1++) {
                vector<double> wuIJ;
                for (int i2 = 0; i2 < wi[i1].size(); i2++) {
                    wuIJ.push_back(k * nLayerErr[i2] * nValues[layerIndex - 1][i1].second);
                }
                wu2.push_back(wuIJ);
            }

            weightUpdates.push_back(wu2);
        }

        for (int i1 = 0; i1 < nnWeights.size(); i1++) {
            wi = nnWeights[i1];
            vector< vector<double>> uwVec = weightUpdates[nnWeights.size() - i1 - 1];

            for (int i2 = 0; i2 < wi.size(); i2++) {
                vector<double> uwIJ = wi[i2];
                const  vector<double>& uwIJvec = uwVec[i2];
                for (int i3 = 0; i3 < uwIJ.size(); i3++) {
                    uwIJ[i3] += uwIJvec[i3];
                }
            }
        }

        outFile << "" << iter << " : ";
        for (int m = 0; m < nLayerErr.size(); m++) {
            if (nLayerErr[m] == 0)
                outFile << "0.0 ";
            else
                outFile << nLayerErr[m] << " ";
        }
        outFile << "\n\n";

        trainingDataIndex = (trainingDataIndex + 1) % nnTrainingVec.size();
    }
    outFile.close();
}

string findInStr(string const& str, int n) {
    if (str.length() < n) {
        return str;
    }
    return str.substr(0, n);
}

vector< vector< vector<double>>> jsonMatrixToVec(const string &filename) {
    vector< vector< vector<double>>> res;
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка открытия файла\n";
        return res;
    }
    string jsonString((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    res = jsonToVecs(jsonString, filename);
    return res;
}

int main(int argc, char* argv[]) {
    cout << setprecision(16);
    setlocale(LC_ALL, "rus");
    string input1, input2, output1, output2;
    int n_count;
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
            if (ifStr2 == "iterats=") {
                n_count = stoi(subStr2);
            }
        }
    }

    vector< vector< vector<double>>> vec = jsonMatrixToVec(input1);
    NNClass n(vec);
    n.nnTraining(input2, n_count, output1);
    return 0;
}