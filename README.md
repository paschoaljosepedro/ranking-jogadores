Sistema de Ranking de Jogadores

Um sistema de ranking de jogadores com interface gráfica desenvolvido em C usando OpenGL/GLUT. O programa permite gerenciar pontuações de jogadores, realizar buscas e ordenações, além de persistir os dados em arquivo.

Funcionalidades Principais
🎮 Cadastro de jogadores (nickname e pontuação)

🔍 Dois tipos de pesquisa:

Pesquisa Binária (por nome)

Pesquisa Exponencial (por pontuação)

🔄 Dois algoritmos de ordenação:

Selection Sort (por pontuação - maior para menor)

Merge Sort (por nome - ordem alfabética)

📊 Visualização do ranking completo

💾 Persistência dos dados em arquivo (ranking.txt)

Requisitos
Sistema operacional Linux (testado em distribuições baseadas em Debian)

Compilador GCC

Bibliotecas:

OpenGL

GLUT

GLU

Instalação
Instale as dependências:

bash
sudo apt-get install freeglut3 freeglut3-dev libglu1-mesa-dev mesa-common-dev
Clone o repositório:

bash
git clone https://github.com/seu-usuario/sistema-ranking.git
cd sistema-ranking
Compile o programa:

bash
gcc main.c -o ranking -lGL -lGLU -lglut
Execute:

bash
./ranking
Como Usar
Menu Principal
Pesquisar: Acessa submenu de pesquisa

Ordenar: Acessa submenu de ordenação

Adicionar: Abre formulário para cadastrar novo jogador

Exibir Rankings: Mostra a lista completa de jogadores

Sair: Salva os dados e encerra o programa

Controles
Teclas W/S ou setas ↑/↓: Navegação pelos menus

Enter: Seleciona opção/menu

ESC: Volta para o menu anterior

Tab: Alterna entre campos nos formulários

Backspace: Apaga caracteres durante a digitação

Estrutura do Código
text
sistema-ranking/
│
├── main.c                # Código fonte principal
├── Makefile              # [Opcional] Arquivo de compilação
├── README.md             # Este arquivo
├── ranking.txt           # Arquivo de dados (criado automaticamente)
└── screenshot.png        # [Opcional] Captura de tela do sistema
Licença
Este projeto está licenciado sob a Licença MIT - veja o arquivo LICENSE para detalhes.

Contribuições
Contribuições são bem-vindas! Sinta-se à vontade para abrir issues ou enviar pull requests.
