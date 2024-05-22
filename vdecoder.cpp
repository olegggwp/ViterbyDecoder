#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>

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
            // Здесь вы должны реализовать свою функцию хеширования.
            // Это просто пример, который может не работать для вашего случая.
            size_t h = 0;
            for (int i = 0; i < N; ++i) {
                h = (h << 1) | code[i];
            }
            return h;
        }
    };


template <int M, int K>
class Coder {
private:
    unordered_map<Code<M>, Code<K>> stateToCode;
public:

    Coder(unordered_map<Code<M>, Code<K>> *stateToCode) {
        this->stateToCode = *stateToCode;
    }

    vector<bool> encode(vector<bool> in) {
        vector<bool> out = vector<bool>();
        for (int i = 0; i < in.size(); i++) {
            Code<M> state;
            for (int j = 0; j < M; j++) {
                state.code[j] = i < j ? 0 : in[i - j];
            }
            Code<K> outCode = stateToCode[state];
            for (int j = 0; j < K; j++) {
                out.push_back(outCode[j]);
            }
        }
        return out;
    }

    vector<bool> decode(vector<bool> *in){
        vector<bool> out = vector<bool>();
        int t = in->size() / K;
        vector<Code<K>> observations(t);
        for (int i = 0; i < t; i++) {
            Code<K> observation;
            for (int j = 0; j < K; j++) {
                observation[j] = (*in)[i * K + j];
            }
            observations[i] = observation;
        }

        vector<vector<int>> tindex;
        vector<vector<int>> tstate;

        return out;
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
Coder<M, K> createCoder(Generator<M> g[K]) {
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
    return Coder<M, K>(&stateToCode);
}



int main() {
    // vector<bool> v = {1, 1, 0};
    // Code<3> a(&v);
    // for (int i = 0; i < 3; i++) {
    //     cout << a.code[i] << endl;
    // }
    bool generator1[] = {1, 1, 1};
    bool generator2[] = {1, 0, 1};
    Generator<3> generators[2];
    generators[0] = Generator<3>(generator1);
    generators[1] = Generator<3>(generator2); 
    // = {Generator<3>(generator1), Generator<3>(generator2)};
    Coder<3, 2> c = createCoder<3, 2>(generators);

    vector<bool> in = {1, 1, 0};
    vector<bool> out = c.encode(in);
    for (int i = 0; i < out.size(); i++) {
        cout << out[i];
    }

    return 0;
}