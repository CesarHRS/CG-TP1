
# Jogo Educativo (CG-TP1)

Feito por:

César Henrique Resende Soares | 20223006601
Lara


## Estrutura de Arquivos

O projeto está organizado com a seguinte hierarquia de diretórios, conforme a imagem fornecida:

  * `assets/sounds/`: Armazena todos os arquivos de áudio (`.mp3`) utilizados pelo jogo.
  * `src/`: Contém todo o código-fonte C++.
  * `Makefile`: Define as regras de compilação, execução e limpeza para ambientes Linux/Unix.
  * `build-windows.bat`: Script auxiliar para compilação em ambiente Windows.
  * `.gitignore`: Especifica os arquivos que devem ser ignorados pelo Git.
  * `jogo_educativo`: O arquivo executável final gerado após a compilação.

## Dependências

O projeto requer as bibliotecas de desenvolvimento do OpenGL, GLUT e da biblioteca de áudio **SDL2\_mixer**.

### Instalação em Distros Debian

Para compilar o projeto em sistemas baseados em Debian, é necessário instalar os seguintes pacotes de desenvolvimento:

```bash
sudo apt-get update
sudo apt-get install freeglut3-dev libglu1-mesa-dev libgl1-mesa-dev libsdl2-dev libsdl2-mixer-dev
```

*(Nota: Para Arch Linux, os pacotes equivalentes seriam `freeglut`, `glu`, `sdl2` e `sdl2_mixer` e o comando para instalar seria:

```bash
sudo pacman -S freeglut glu sd12 sd12_mixer
```
)

## Compilação e Execução

O `Makefile` fornecido automatiza o processo em ambientes Linux.

### 1\. Compilar o Projeto

Para compilar o código-fonte e gerar o executável `jogo_educativo`, execute:

```bash
make
```

Os comandos `make all` ou `make build` executam a mesma ação.

### 2\. Executar o Jogo

Para executar o programa após a compilação:

```bash
make run
```

### 3\. Limpar o Projeto

Para remover o executável e todos os arquivos-objeto (`.o`) gerados:

```bash
make clean
```

-----

### Comandos para Windows

O `Makefile` também define *targets* para compilação em ambiente Windows (requer um *toolchain* como MinGW/MSYS2):

  * **Compilar:** `make build-windows`
  * **Executar:** `make run-windows`
  * **Limpar:** `make clean-windows`