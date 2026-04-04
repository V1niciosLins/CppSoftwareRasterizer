# C++ Software Rasterizer

Um motor de renderização 3D construído 100% do zero, operando exclusivamente na CPU. Nenhuma API gráfica (OpenGL, Vulkan, DirectX) foi utilizada para o cálculo geométrico. Toda a matemática de matrizes, projeção, rasterização de triângulos e mapeamento de textura foi escrita no metal, manipulando diretamente os arrays de pixels e repassando o framebuffer final para a SDL2 exibir na tela.

## Arquitetura & Features
- **Pipeline Matemático Completo:** Matrizes de Model, View e Projection (MVP) implementadas do zero.
- **Rasterização Baricêntrica:** Preenchimento de triângulos interpolando vértices na unha.
- **Z-Buffering:** Teste de profundidade infinito para oclusão de geometria (cravando o teste `<` pixel a pixel).
- **Back-Face Culling:** Rejeição de polígonos ocultos otimizando a carga do rasterizador.
- **Parsers Customizados:** Leitura nativa de geometrias `.obj` e propriedades de material `.mtl` (cor de fallback e UVs).
- **Texture Mapping:** Leitura de imagens (via `stb_image`) com limites de memória blindados (Clamp/Wrapping).
- **Câmera Livre (6-DOF):** Navegação espacial em tempo real extraindo o estado bruto do hardware de teclado.
- **Multithreading Bruto:** Renderização paralelizada no eixo Y usando OpenMP, esgotando 100% dos núcleos do processador sem *Race Conditions*.

## Dependências
- Compilador C++17 ou superior
- CMake
- SDL2 (Estritamente para instanciar a janela da SO e ler o teclado)

## Compilação (Build)
O projeto utiliza CMake para geração do binário. Rode os comandos abaixo na raiz do repositório:

```bash
# Gera os arquivos de build
cmake -B build

# Compila o projeto otimizado
cmake --build build

## Execução:

Inicie o motor pelo terminal. O projeto suporta a injeção de rotas de modelos e texturas via argumentos (ajuste conforme o seu binário):

Controles de Câmera:

    W / S: Move no eixo Z (Frente / Trás)

    A / D: Move no eixo X (Esquerda / Direita)

    Espaço: Move no eixo Y (Sobe)

    Left Shift: Move no eixo Y (Desce)
