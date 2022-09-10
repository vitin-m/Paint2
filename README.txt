Instruções do software referente ao trabalho 2 de Computação Gráfica
Aluno: Vítor Melo Lopes

Informações de compilação:
Versão mínima do compilador: C++17
Compilador utilizado: MSVC22
Bibliotecas utilizadas: OpenGL 1.1, freeglut 3.2.2
Info: O header utilizado gerencia a inclusão de OpenGL e dependências por SO, caso queira mudar e incluir OpenGL diretamente, alterar em "Managers.h".

Os algoritmos de rasterização e preenchimento estão implementados em Rasterizer.cpp;

##################
Informações de uso:
O usuário pode pressionar as seguintes teclas:
s: Modo de seleção. O clique não tem função.
l: Modo de linha. Dois cliques formam uma linha.
t: Modo de triângulo. Três cliques formam um triângulo.
r: Modo de retângulo. Dois cliques formam um retângulo.
p: Modo de polígono. N cliques com o clique final perto do inicial formam um polígono.
c: Modo de circunferência. Dois cliques definem centro e um ponto na borda, respectivamente.

Ao clicar duas vezes, a forma vem preenchida.
Para o preenchimento dos polígonos, foi utilizado o Algoritmo de Preenchimento de Polígono;
Para o preenchimento da circunferência, foi utilizado um Breseham modificado.

P (shift/CAPS + p): Modo de pintura de balde. Pinta com a cor selecionada a área de cor 	consecutiva.
Foi utilizado um algoritmo de flood fill iterativo com vizinhança 4.

Os modos de transformação afetam apenas o último objeto inserido na tela. O objeto gerado pelo modo pintura não é afetado.

T (shift/CAPS + T): Modo de translação. Setas para esquerda/direita movem x em -+5 pixels e 	baixo/cima movem y em -+5 pixels.
S (shift/CAPS + p): Modo de escala. Setas para esquerda/direita escalam em x com Sx = 	0,9/1,1, cima/baixo escalam em y com Sy = 0,9/1,1.
C (shift/CAPS + p): Modo de cisalhamento. Setas para esquerda/direita operam cisalhamento em 	x com Cx = 0,9/1,1, cima/baixo operam cisalhamento em y com Cy = 0,9/1,1.
M (shift/CAPS + p): Modo de reflexão. Setas para esquerda/direita refletem em x, setas para 	baixo/cima refletem em y.
R (shift/CAPS + p): Modo de rotação. Setas para esquerda/direita rotacionam em sentido anti-	horário/horário com theta = 2o, cima/baixo rotacionam em sentido anti-horário/horário 	com theta = 10o.

Por fim, o usuário pode selecionar cores para os novos objetos por meio das teclas F1 a F5.

##################
Limitações
- Rotação com centroide deslocada depois de uma rotação. Centroide dinâmico ou ponto dinâmico quebravam depois de várias operações serem realizadas na matriz de transformação do objeto;
- Flood fill lento pois usa formato de input diferente do resto do sistema.