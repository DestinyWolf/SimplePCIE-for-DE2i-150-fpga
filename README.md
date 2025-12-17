# Simple PCIe um driver de comunicação entre fpga e processador para o kit de desenvolvimento DE2i-150

Este repositorio tem como objetivo abrigar o driver utilizado para comunicação entre o processador atom e a fpga contida no kit de desenvolvimento DE2i-150 ambos foram desenvolvidos para serem utilizados no ubuntu server 24.03 lts
os arquivos contidos nesse repositorio podem ser utilizados por qualquer pessoa desde que deem os devidos creditos ao criador

>[!warning]
>No presente momento a biblioteca e o module kenel(driver) foram feitos para trabalhar com o ioctl, o que diminui drasticamente o desempenho tendo em vista que a comunicação entre fpga e o processador intel ocorrem por meio de um baramento PCIe.

Para futuras melhorias, acredito que adicição de uma forma do driver trabalhar com DMA (Direct Memory Access) seria um grande avanço tendo em vista que essa mudança permitiria trabalhar com mais taxa de transmissão de dados assim como a transmissão de blocos maiores de dados entre fpga e processador.
