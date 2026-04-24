# PCP - Bar dos Filósofos

## Jantar do Filósofos
O jantar dos filósofos é um problema sobre a sincronização na comunicação entre processos e threads em sistemas operacionais.
O problema foi proposto por Edsger W. Dijkstra em 1965 e é considerado um dos problemas clássicos sobre sistemas operacionais.

O problema consiste em cinco filósofos sentados ao redor de uma mesa circular para o jantar, onde cada filósofo possui um prato para comer macarrão.
Os filósofos possuem garfos e cada um precisa de 2 garfos para comer, porém para cada par de filósofos adjacentes existe apenas um garfo.
Um filósofo possui duas ações: filosofar ou comer. Quando um filósofo ficar com fome ele irá tentar pegar o garfo à sua direita e à sua esquerda.
Se ele conseguir pegar os dois garfos, ele irá comer a comida em um tempo determinado e irá colocar os garfos sobre a mesa novamente, voltando a pensar.

## Problema geral
No Bar dos Filósofos, não há restrição de estrutura, cada filósofo é um nó de um grafo que compatilha uma garrafa com cada uma de suas arestas.
Um filósofo tem 3 ações: filosofar, pedir garrafas ou beber. A cada rodada um filósofo pode pedir uma quantidade qualquer de garrafas de 2 até a quantidade de vizinhos que possui.
Se ele conseguir todas as garrafas que quer ele bebe e depois volta a filosofar.

# Soluções
Soluções podem ser implementadas com um arquivo que segue a seguinte estrutura:
#### solution.cpp
``` c++
#include "../include/philosopher.hpp"
class Philosopher_Derived : public Philosopher {
  protected:
    void RequestBottles() override { /* Implementação */ }
    void ReleaseBottles() override { /* Implementação */ }
  public:
    Philosopher_Derived(unsigned long seed) : Philosopher(seed) {}
};
#define PHILOSOPHER_CLASS Philosopher_Derived // <- same name as the derived class'
#include "../include/test.hpp"

int main(int argc, char** argv) {
  if (argc < 3)
    return 0;
  
  Table table;
  test_solution(&table, std::string(argv[1]), atoi(argv[2]));
}
```

# Compilar e usar
`g++ solves/solution.cpp -O2 -o solution.out`

`./solution.out path/to/graph_file.txt how_many_times_each_philosopher_needs_to_drink`
