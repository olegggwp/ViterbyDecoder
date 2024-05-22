#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <chrono>
#include <random>
#include <set>

using namespace std;

template <int N>
class Code{
public:
    bool code[N];
    Code() {
        for (int i = 0; i < N; i++) {
            code[i] = false;
        }
    }
    Code(int a) {
        for (int i = 0; i < N; i++) {
            code[N-i-1] = (a >> i) & 1;
        }
    }
    Code(vector<bool> *code) {
        for (int i = 0; i < N; i++) {
            this->code[i] = (*code)[i];
        }
    }

    bool operator==(const Code<N>& other) const {
        for (int i = 0; i < N; ++i) {
            if (code[i] != other.code[i]) {
                return false;
            }
        }
        return true;
    }

    bool& operator[](int index) {
        return code[index];
    }
    bool operator[](int index) const {
        return code[index];
    }

};

template <int N>
    struct hash<Code<N>> {
        size_t operator()(const Code<N>& code) const {
            size_t h = 0;
            for (int i = 0; i < N; ++i) {
                h = (h << 1) | code[i];
            }
            return h;
        }
    };

template <int M>
vector<bool> pathToBools(vector<int> ints) {
    vector<bool> bools;
    for (int i = 1; i < ints.size(); i++) {
        Code<M> code = Code<M>(ints[i]);
        bools.push_back(code[M-1]);
    }
    return bools;
}

template <int M>
bool canGo(int a, int b) {
    Code<M> codeA = Code<M>(a);
    Code<M> codeB = Code<M>(b);
    for (int i = 0; i < M-1; i++) {
        if (codeA[i+1] != codeB[i]) {
            return false;
        }    
    }
    return true;
}

template <int K>
int hemmDist(Code<K> code1, Code<K> code2) {
    int dist = 0;
    for (int i = 0; i < K; i++) {
        dist += (code1[i] != code2[i]);
    }
    return dist;
}

int hemmDist(vector<bool> code1, vector<bool> code2) {
    int dist = 0;
    for (int i = 0; i < code1.size(); i++) {
        dist += (code1[i] != code2[i]);
    }
    return dist;
}


template <int M, int K>
class Coder {
private:
    unordered_map<Code<M+1>, Code<K>> stateToCode;
public:
    Coder(){}

    Coder(unordered_map<Code<M+1>, Code<K>> *stateToCode) {
        this->stateToCode = *stateToCode;
    }

    vector<bool> encode(vector<bool> in) {
        vector<bool> out = vector<bool>();
        for (int i = 0; i < in.size(); i++) {
            Code<M+1> state;
            for (int j = 0; j < M+1; j++) {
                state.code[j] = i - M + j < 0 ? 0 : in[i - M + j];
            }
            Code<K> outCode = stateToCode[state];
            for (int j = 0; j < K; j++) {
                out.push_back(outCode[j]);
            }
        }
        return out;
    }

    vector<bool> decode(vector<bool> *in){
        int t = in->size() / K;
        int statesCnt = 1 << M;
        vector<Code<K>> observations(t);
        for (int i = 0; i < t; i++) {
            Code<K> observation;
            for (int j = 0; j < K; j++) {
                observation[j] = (*in)[i * K + j];
            }
            observations[i] = observation;
        }

        vector<vector<int>> tindex = vector<vector<int>>(statesCnt, vector<int>(t + 1));
        vector<vector<int>> tstate = vector<vector<int>>(statesCnt, vector<int>(t + 1, 1000000));

        //tstate в оригинале хранит какую то наибольшую вероятность
        //мы будем ранить минимальное расстояние хэмминга и делать соответственно не argmax а argmin

        for (int j = 0; j < statesCnt; j++) {
            tstate[j][0] = j == 0 ? 0 : 1000000;    // у нас распределение пи это вероятность 1 на s_0=0000
            tindex[j][0] = 0;
        }
        for (int i = 1; i <= t; i++) {
            for (int j = 0; j < statesCnt; j++) {
                int mx = 100000000;
                for (int k = 0; k < statesCnt; k++) {
                    // Code<M> statek = Code<M>(k);
                    // Code<M+1> statej = Code<M+1>((j<<1) + (k&1));
                    Code<M+1> statej = Code<M+1>((k<<1) + (j&1));
                    int r;
                    if (canGo<M>(k,j)) {
                        r = tstate[k][i - 1] 
                            + hemmDist<K>(stateToCode[statej], observations[i - 1]);

                        if (r < mx) {
                            mx = r;
                            tindex[j][i] = k;
                            // if (j == 1 && i == 1) {
                            //     cout << "55";
                            // }
                            tstate[j][i] = r;
                        }
                    }
                    // int r = tstate[k][i - 1] 
                    //     * (!canGo<M>(k,j)) 
                    //     * hemmDist<K>(stateToCode[statej], observations[i - 1]);
                }
            }
        }
        vector<int> path(t+1);
        int mx = 100000;
        for (int i = 0; i < statesCnt; i++) {
            if (tstate[i][t] < mx) {
                mx = tstate[i][t];
                path[t] = i;
            }
        }
        for (int i = t - 1; i >= 0; i--) {
            path[i] = tindex[path[i + 1]][i + 1];
        }
        return pathToBools<M>(path);   
    }
};


template <int M>
class Generator {
    public:
    bool g[M];

    //default constructor
    Generator() {
        for (int i = 0; i < M; i++) {
            g[i] = false;
        }
    }

    Generator(bool g[M]) {
        for (int i = 0; i < M; i++) {
            this->g[i] = g[i];
        }
    }

    bool generate(Code<M> in) {
        bool out = false;
        for (int i = 0; i < M; i++) {
            out = out ^ (in[i] & g[i]);
        }
        return out;
    }
};

template <int M, int K>
Coder<M-1, K> createCoder(Generator<M> g[K]) {
    unordered_map<Code<M>, Code<K>> stateToCode;
    for (int i = 0; i < (1 << M); i++) {
        Code<M> state;
        for (int j = 0; j < M; j++) {
            state[j] = (i >> j) & 1;
        }
        Code<K> outCode;
        for (int j = 0; j < K; j++) {
            outCode[j] = g[j].generate(state);
        }
        stateToCode[state] = outCode;
    }
    return Coder<M-1, K>(&stateToCode);
}

class Chanel{
private:
    Coder<2, 2> c;
public:
    Chanel() {
        bool generator1[] = {1, 1, 1};
        bool generator2[] = {1, 0, 1};
        Generator<3> generators[2];
        generators[0] = Generator<3>(generator1);
        generators[1] = Generator<3>(generator2); 
        c = createCoder<3, 2>(generators);
    }

    // возвращает количество допущенных ошибок
    int getFromChanell(vector<bool> *in, int errorsCnt) {
        mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
        vector<bool> b = c.encode(*in);
        set<int> errors;
        for (int i = 0; i < errorsCnt; i++) {
            while (true) {
                int x = rng() % in->size();
                if (errors.count(x) == 0) {
                    errors.insert(x);
                    break;
                }
            }
            for (int e : errors) {
                b[e] = !b[e];
            }
        }
        vector<bool> result = c.decode(&b);   
        int resultErrorsCnt = 0;
        for (int i = 0; i < result.size(); i++) {
            resultErrorsCnt += result[i] != (*in)[i];
        }
        return resultErrorsCnt;
    }

};

int main() {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    Chanel chanel;
    int len = 50;
    int bunch = 30;
    map<int, double> statistics;
    for (int rate = 0; rate < len; rate++) {
        vector<int> a(bunch);
        for(int j = 0; j < bunch; j++) {
            vector<bool> in(len);
            for (int k = 0; k < len; k++) {
                in[k] = rng() % 2;
            }
            a[j] = chanel.getFromChanell(&in, rate);
        }
        long double sum = 0;
        for(int j = 0; j < bunch; j++) {
            sum += a[j]; 
        }
        statistics[rate] = sum/bunch; 
        cout << rate << ", " << sum/bunch << "" << endl; 
    }



    return 0;
}