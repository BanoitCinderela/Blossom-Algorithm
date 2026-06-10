/*
 * Algoritmo de Emparelhamento de Edmonds (Blossom Algorithm) - 1965
 * Disciplina: Teoria dos Grafos - IF Goiano Campus Rio Verde
 *
 * Funcionalidades:
 *  - Le grafo de arquivo texto (vertices e arestas)
 *  - Encontra emparelhamento maximo em grafo geral
 *  - Salva resultado em arquivo texto
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <map>
#include <string>
using namespace std;

// =============================================
// ESTRUTURA DO GRAFO (modelo GrafoCC)
// =============================================
struct Grafo {
    int numVertices;
    vector<vector<int>> adj; // lista de adjacencia

    Grafo(int n) {
        numVertices = n;
        adj.resize(n + 1); // vertices de 1 a n
    }

    void adicionarAresta(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

// =============================================
// LER GRAFO DE ARQUIVO
// Formato do arquivo:
//   Linha 1: numero de vertices
//   Linha 2: numero de arestas
//   Demais linhas: u v  (cada aresta)
// =============================================
Grafo lerArquivo(const string& nomeArquivo) {
    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel abrir o arquivo " << nomeArquivo << endl;
        exit(1);
    }

    int n, m;
    arquivo >> n >> m;

    Grafo g(n);

    for (int i = 0; i < m; i++) {
        int u, v;
        arquivo >> u >> v;
        g.adicionarAresta(u, v);
    }

    arquivo.close();
    cout << "Grafo lido com sucesso! Vertices: " << n << ", Arestas: " << m << endl;
    return g;
}

// =============================================
// ALGORITMO DE EDMONDS (BLOSSOM)
// Encontra emparelhamento maximo em grafo geral
// =============================================

// Encontra a raiz de um vertice no conjunto disjunto (union-find)
int encontrarRaiz(vector<int>& pai, int x) {
    if (pai[x] != x)
        pai[x] = encontrarRaiz(pai, pai[x]);
    return pai[x];
}

// Algoritmo principal de Edmonds
vector<int> emparelhamentoEdmonds(Grafo& g) {
    int n = g.numVertices;
    vector<int> match(n + 1, -1); // match[v] = parceiro de v, -1 se livre

    // Tenta aumentar o emparelhamento para cada vertice livre
    for (int inicio = 1; inicio <= n; inicio++) {
        if (match[inicio] != -1) continue; // ja emparelhado

        // BFS para encontrar caminho aumentante
        vector<int> pai(n + 1, -1);
        vector<int> base(n + 1);
        vector<bool> usadoNaBFS(n + 1, false);

        for (int i = 1; i <= n; i++) base[i] = i;

        usadoNaBFS[inicio] = true;
        queue<int> fila;
        fila.push(inicio);

        bool encontrou = false;

        while (!fila.empty() && !encontrou) {
            int v = fila.front();
            fila.pop();

            for (int u : g.adj[v]) {
                // Ignora se mesma base (parte de uma flor)
                if (encontrarRaiz(base, v) == encontrarRaiz(base, u))
                    continue;

                if (u == inicio || (match[u] != -1 && pai[match[u]] != -1)) {
                    // Encontrou uma flor (blossom) - contrai
                    int raizV = encontrarRaiz(base, v);
                    int raizU = encontrarRaiz(base, u);

                    // Marca vertices da flor
                    vector<bool> naFlor(n + 1, false);
                    naFlor[raizV] = naFlor[raizU] = true;

                    // Sobe pelos caminhos ate encontrar base comum
                    int tempV = raizV, tempU = raizU;
                    while (tempV != tempU) {
                        if (tempV != inicio) {
                            naFlor[tempV] = true;
                            tempV = encontrarRaiz(base, pai[match[tempV]]);
                        }
                        if (tempU != inicio) {
                            naFlor[tempU] = true;
                            tempU = encontrarRaiz(base, pai[match[tempU]]);
                        }
                    }
                    int baseDaFlor = tempV;

                    // Contrai a flor: todos apontam para a base
                    for (int i = 1; i <= n; i++) {
                        if (naFlor[encontrarRaiz(base, i)]) {
                            base[i] = baseDaFlor;
                            if (!usadoNaBFS[i]) {
                                usadoNaBFS[i] = true;
                                fila.push(i);
                            }
                        }
                    }

                } else if (pai[u] == -1) {
                    // Vertice nao visitado
                    pai[u] = v;

                    if (match[u] == -1) {
                        // Encontrou vertice livre - aumenta o caminho
                        int cur = u;
                        while (cur != -1) {
                            int prev = pai[cur];
                            int antPrev = match[prev];
                            match[cur] = prev;
                            match[prev] = cur;
                            cur = antPrev;
                        }
                        encontrou = true;
                        break;
                    } else {
                        // Continua BFS pelo parceiro
                        usadoNaBFS[match[u]] = true;
                        fila.push(match[u]);
                    }
                }
            }
        }
    }

    return match;
}

// =============================================
// SALVAR RESULTADO EM ARQUIVO
// =============================================
void salvarResultado(const string& nomeArquivo, const vector<int>& match, int n) {
    ofstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        cout << "Erro: nao foi possivel criar o arquivo " << nomeArquivo << endl;
        return;
    }

    arquivo << "=== RESULTADO DO EMPARELHAMENTO DE EDMONDS ===" << endl;
    arquivo << endl;

    int totalPares = 0;
    vector<bool> impresso(n + 1, false);

    arquivo << "Pares emparelhados:" << endl;
    for (int v = 1; v <= n; v++) {
        if (match[v] != -1 && !impresso[v] && !impresso[match[v]]) {
            arquivo << "  Vertice " << v << " <-> Vertice " << match[v] << endl;
            impresso[v] = impresso[match[v]] = true;
            totalPares++;
        }
    }

    arquivo << endl;
    arquivo << "Vertices nao emparelhados:" << endl;
    bool algumLivre = false;
    for (int v = 1; v <= n; v++) {
        if (match[v] == -1) {
            arquivo << "  Vertice " << v << endl;
            algumLivre = true;
        }
    }
    if (!algumLivre) arquivo << "  Nenhum" << endl;

    arquivo << endl;
    arquivo << "Tamanho do emparelhamento maximo: " << totalPares << " pares" << endl;

    arquivo.close();
    cout << "Resultado salvo em " << nomeArquivo << endl;
}

// =============================================
// MAIN
// =============================================
int main() {
    string arquivoEntrada, arquivoSaida;

    cout << "==========================================" << endl;
    cout << " Algoritmo de Emparelhamento de Edmonds  " << endl;
    cout << " Teoria dos Grafos - IF Goiano           " << endl;
    cout << "==========================================" << endl;
    cout << endl;

    cout << "Digite o nome do arquivo de entrada (ex: grafo.txt): ";
    cin >> arquivoEntrada;

    cout << "Digite o nome do arquivo de saida (ex: resultado.txt): ";
    cin >> arquivoSaida;

    // Le o grafo
    Grafo g = lerArquivo(arquivoEntrada);

    // Executa o algoritmo
    cout << "Executando algoritmo de Edmonds..." << endl;
    vector<int> match = emparelhamentoEdmonds(g);

    // Mostra no terminal
    cout << endl << "=== RESULTADO ===" << endl;
    int total = 0;
    vector<bool> impresso(g.numVertices + 1, false);
    for (int v = 1; v <= g.numVertices; v++) {
        if (match[v] != -1 && !impresso[v] && !impresso[match[v]]) {
            cout << "Vertice " << v << " <-> Vertice " << match[v] << endl;
            impresso[v] = impresso[match[v]] = true;
            total++;
        }
    }
    cout << "Tamanho do emparelhamento maximo: " << total << " pares" << endl;

    // Salva resultado
    salvarResultado(arquivoSaida, match, g.numVertices);

    return 0;
}
