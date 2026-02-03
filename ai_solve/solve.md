# Prompt

~~~
Dada a seguinte instância de grafo:

0 1 16

0 2 9

0 4 8

1 2 3

1 3 6

1 4 3

1 6 4

2 3 5

2 4 3

2 5 4

2 6 3

3 4 2

3 6 6

4 5 7

4 6 3

4 7 4

5 8 7

5 9 4

6 7 5

6 8 6

6 9 8

7 9 8

8 9 12



Onde cada linha representa uma aresta, sendo a primeira coluna o nó de origem, a segunda o nó de destino e a última a capacidade de fluxo da aresta



Haja como um algoritmo de fluxo máximo e me diga qual o fluxo máximo da origem 0 até o destino 9
~~~


# Resposta

Beleza — vou “rodar” mentalmente um **algoritmo de fluxo máximo (estilo Edmonds–Karp)** nesse grafo, considerando **arestas direcionadas exatamente como você forneceu**.

---

## 🎯 Problema

* **Origem (source):** nó `0`
* **Destino (sink):** nó `9`
* **Capacidades:** dadas na última coluna
* **Objetivo:** encontrar o **fluxo máximo** de `0` até `9`

---

## 🧠 Resultado do algoritmo de Fluxo Máximo

Após sucessivas buscas por **caminhos aumentantes** no grafo residual e atualizações das capacidades, o algoritmo converge para:

> ✅ **Fluxo máximo de 0 até 9 = *31 unidades***

---

## 🔎 Intuição do resultado (sem entrar em todas as iterações)

* O nó `0` consegue injetar bastante fluxo inicialmente (16 + 9 + 8 = **33**).
* O gargalo não está na origem, mas na **capacidade total de chegada ao nó 9**, que vem principalmente via:

  * `5 → 9` (4)
  * `6 → 9` (8)
  * `7 → 9` (8)
  * `8 → 9` (12)
* Mesmo redistribuindo o fluxo pelos vários caminhos intermediários (`1`, `2`, `3`, `4`, `6`, `7`, `8`), o limite estrutural do grafo faz o fluxo parar em **31**.

---

## 📌 Conclusão

> 🔥 **O fluxo máximo da origem 0 até o destino 9 é:**
>
> **🟢 31 unidades de fluxo**
