#include <string>
#include <vector>
#include <cstdint>
#include <algorithm> // Necessário para std::sort
#include <functional> // Necessário para std::less
#include "../include/table.hpp"
#include "../include/philosopher.hpp"

class Philosopher_Ordered : public Philosopher {
  private:
    // Vetor que armazenará os vizinhos e suas respectivas garrafas ordenadas
    std::vector<std::pair<uint64_t, Bottle*>> sorted_channels;
    std::vector<uint64_t> owned_bottles;

    // Função para inicializar a ordenação (chamada apenas na primeira sede)
    void SortChannels() {
      for (auto& neig_bot : channels) {
        sorted_channels.push_back(neig_bot);
      }
      
      // Ordena usando o endereço de memória do ponteiro da garrafa.
      // Isso cria uma "numeração" global única, pois a mesma garrafa compartilhada
      // tem o exato mesmo endereço para ambos os filósofos.
      std::sort(sorted_channels.begin(), sorted_channels.end(),
        [](const std::pair<uint64_t, Bottle*>& a, const std::pair<uint64_t, Bottle*>& b) {
          return std::less<Bottle*>()(a.second, b.second);
        }
      );
    }

  protected:
    void RequestBottles() override {
      // Inicializa a ordem das garrafas apenas uma vez
      if (sorted_channels.empty()) {
        SortChannels();
      }

      bool failed = false;
      
      // Tenta pegar as garrafas estritamente na ordem numérica (de endereços)
      for (const auto& neig_bot : sorted_channels) {
        if (neig_bot.second->mutex.try_lock()) {
          owned_bottles.push_back(neig_bot.first);
          
          // Se já conseguiu a quantidade desejada, para de tentar
          if (owned_bottles.size() >= wanted_bottles) {
            break;
          }
        } else {
          // REGRA DE OURO DA ORDENAÇÃO DE RECURSOS:
          // Se falhar ao pegar uma garrafa, NÃO tente pegar as próximas.
          // Se pulasse, poderia gerar um ciclo (deadlock). Marca como falha e sai do loop.
          failed = true;
          break; 
        }
      }

      // Se falhou em pegar alguma garrafa da sequência, ou não conseguiu todas
      if (failed || owned_bottles.size() < wanted_bottles) {
        // Libera todas as que conseguiu pegar até então
        ReleaseBottles();
        // Espera um tempo aleatório antes de tentar a sequência novamente
        sleep(rng.NextInt(0, neighboors));
      }

      acquired_bottles = owned_bottles.size();
    }

    void ReleaseBottles() override {
      for (auto&& bot_idx : owned_bottles)
        channels[bot_idx]->mutex.unlock();
      owned_bottles.clear();
    }

  public:
    Philosopher_Ordered(uint64_t seed) : Philosopher(seed) {}
};

#define PHILOSOPHER_CLASS Philosopher_Ordered
#include "../include/test.hpp"

int main(int argc, char** argv) {
  if (argc < 3)
    return 0;
  
  Table table;
  test_solution(&table, std::string(argv[1]), atoi(argv[2]));
}