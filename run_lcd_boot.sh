#!/bin/bash

# Define onde estão os seus arquivos (MUDE AQUI SE FOR OUTRA PASTA)
WORK_DIR="/home/kake/fpga_driver/source"

cd $WORK_DIR

# 1. Carrega o Driver (se ainda não estiver carregado)
# Verifica se o arquivo de dispositivo existe
if [ ! -c /dev/altera_pcie ]; then
    echo "Carregando driver PCIe..."
    insmod altera_pcie.ko
    chmod 666 /dev/altera_pcie
fi

# 2. Espera um pouco para garantir que o driver subiu
sleep 1

# 3. Executa o mostrador de IP
# O loop garante que, se o IP demorar para chegar (DHCP lento),
# ele atualiza a tela a cada 5 segundos.
while true; do
    ./show_ip
    sleep 5
done
